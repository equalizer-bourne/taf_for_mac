#include "util/tc_option.h"
#include "util/tc_common.h"
#include "nodeF/taf_nodeF.h"
#include "servant/Application.h"
#include "servant/AppProtocol.h"
#include "servant/AdminServant.h"
#include "servant/ServantHandle.h"
#include "servant/BaseF.h"
#include "servant/AppCache.h"
#include "servant/NotifyObserver.h"

#include <signal.h>

namespace taf
{

static void sighandler( int sig_no )
{
    Application::terminate();
}

std::string ServerConfig::Application;      //Ӧ������
std::string ServerConfig::ServerName;       //��������,һ���������ƺ�һ�����������ʶ
std::string ServerConfig::LocalIp;          //����IP
std::string ServerConfig::BasePath;         //Ӧ�ó���·�������ڱ���Զ��ϵͳ���õı���Ŀ¼
std::string ServerConfig::DataPath;         //Ӧ�ó���·�������ڱ�������
std::string ServerConfig::Local;            //�����׽���
std::string ServerConfig::Node;             //����node��ַ
std::string ServerConfig::Log;              //��־���ĵ�ַ
std::string ServerConfig::Config;           //�������ĵ�ַ
std::string ServerConfig::Notify;           //��Ϣ֪ͨ����
std::string ServerConfig::LogPath;          //logpath
int         ServerConfig::LogSize;          //log��С(�ֽ�)
int         ServerConfig::LogNum;           //log����()
std::string ServerConfig::ConfigFile;       //��������ļ�·��
int         ServerConfig::ReportFlow;       //�Ƿ������ϱ����нӿ�stat���� 0���ϱ� 1�ϱ� (���ڷ�tafЭ���������ͳ��)

static string outfill(const string& s, char c = ' ', int n = 29)
{
    return (s + string(abs(n - (int)s.length()), c));
}

#define OUT_LINE        (outfill("", '-', 50))
#define OUT_LINE_LONG   (outfill("", '=', 50))

///////////////////////////////////////////////////////////////////////////////////////////
TC_Config                       Application::_conf;
TC_EpollServerPtr               Application::_epollServer  = NULL;
CommunicatorPtr                 Application::_communicator = NULL;

///////////////////////////////////////////////////////////////////////////////////////////
Application::Application()
{
}

Application::~Application()
{
    terminate();
}

TC_Config& Application::getConfig()
{
    return _conf;
}

TC_EpollServerPtr& Application::getEpollServer()
{
    return _epollServer;
}

CommunicatorPtr& Application::getCommunicator()
{
    return _communicator;
}

void Application::waitForShutdown()
{
    _epollServer->waitForShutdown();

    destroyApp();

    TafRemoteNotify::getInstance()->report("stop", true);
}

void Application::terminate()
{
    if(_epollServer)
    {
        _epollServer->terminate();
    }
}

bool Application::cmdViewStatus(const string& command, const string& params, string& result)
{
    LOG->info() << "Application::cmdViewStatus:" << command << " " << params << endl;

    ostringstream os;

    os << OUT_LINE_LONG << endl;

    os << outfill("[proxy config]:") << endl;

    outClient(os);

    os << OUT_LINE << "\n" << outfill("[server config]:") << endl;

    outServer(os);

    os << OUT_LINE << endl;

    outAllAdapter(os);

    result = os.str();

    return true;
}

bool Application::cmdSetLogLevel(const string& command, const string& params, string& result)
{
    LOG->debug() << "Application::cmdSetLogLevel:" << command << " " << params << endl;

    string level = TC_Common::trim(params);

    int ret = TafRollLogger::getInstance()->logger()->setLogLevel(level);

    if(ret == 0)
    {
        result = "set log level [" + level + "] ok";

        AppCache::getInstance()->set("logLevel",level);
    }
    else
    {
        result = "set log level [" + level + "] error";
    }

    return true;
}

bool Application::cmdLoadConfig(const string& command, const string& params, string& result)
{
    LOG->info() << "Application::cmdLoadConfig:" << command << " " << params << endl;

    string filename = TC_Common::trim(params);

    if (TafRemoteConfig::getInstance()->addConfig(filename, result))
    {
        TafRemoteNotify::getInstance()->report(result);

        return true;
    }

    TafRemoteNotify::getInstance()->report(result);

    return true;
}

bool Application::cmdConnections(const string& command, const string& params, string& result)
{
    LOG->info() << "Application::cmdConnections:" << command << " " << params << endl;

    ostringstream os;

    os << OUT_LINE_LONG << endl;

    map<int, TC_EpollServer::BindAdapterPtr> m = _epollServer->getListenSocketInfo();

    for(map<int, TC_EpollServer::BindAdapterPtr>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        vector<TC_EpollServer::ConnStatus> v = it->second->getConnStatus();

        os << OUT_LINE << "\n" << outfill("[adater:" + it->second->getName() + "] [connections:" + TC_Common::tostr(v.size())+ "]") << endl;

        os  << outfill("conn-uid", ' ', 15)
            << outfill("ip:port", ' ', 25)
            << outfill("last-time", ' ', 25)
            << outfill("timeout", ' ', 10) << endl;

        for(size_t i = 0; i < v.size(); i++)
        {
            os  << outfill(TC_Common::tostr<uint32_t>(v[i].uid), ' ', 15)
                << outfill(v[i].ip + ":" + TC_Common::tostr(v[i].port), ' ', 25)
                << outfill(TC_Common::tm2str(v[i].iLastRefreshTime,"%Y-%m-%d %H:%M:%S"), ' ', 25)
                << outfill(TC_Common::tostr(v[i].timeout), ' ', 10) << endl;
        }
    }
    os << OUT_LINE_LONG << endl;

    result = os.str();

    return true;
}

bool Application::cmdViewVersion(const string& command, const string& params, string& result)
{
    result = "$" + string(TAF_VERSION) + "$";
    return true;
}

bool Application::cmdLoadProperty(const string& command, const string& params, string& result)
{
    try
    {
        LOG->info() << "Application::cmdLoadProperty:" << command << " " << params << endl;

        //���½��������ļ�
        _conf.parseFile(ServerConfig::ConfigFile);

        string sResult = "";

        //����ͨѶ������
        _communicator->setProperty(_conf);

        _communicator->reloadProperty(sResult);

        //����Զ�̶���
        ServerConfig::Log = _conf.get("/taf/application/server<log>");

        TafTimeLogger::getInstance()->setLogInfo(_communicator, ServerConfig::Log, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::LogPath);

        ServerConfig::Config = _conf.get("/taf/application/server<config>");

        TafRemoteConfig::getInstance()->setConfigInfo(_communicator, ServerConfig::Config, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::BasePath);

        ServerConfig::Notify = _conf.get("/taf/application/server<notify>");

        TafRemoteNotify::getInstance()->setNotifyInfo(_communicator, ServerConfig::Notify, ServerConfig::Application, ServerConfig::ServerName);

        result = "loaded config items:\r\n" + sResult + 
                 "log=" + ServerConfig::Log + "\r\n" + 
                 "config=" + ServerConfig::Config + "\r\n" + 
                 "notify=" + ServerConfig::Notify + "\r\n";
    }
    catch (TC_Config_Exception & ex)
    {
        result = "load config " + ServerConfig::ConfigFile + " error:" + ex.what();
    }
    catch (exception &ex)
    {
        result = ex.what();
    }
    return true;
}
    
bool Application::cmdViewAdminCommands(const string& command, const string& params, string& result)
{
    LOG->info() << "Application::cmdViewAdminCommands:" << command << " " << params << endl;

    result =result +  NotifyObserver::getInstance()->viewRegisterCommand();

    return true;
}

bool Application::cmdSetDyeing(const string& command, const string& params, string& result)
{
    vector<string> vDyeingParams = TC_Common::sepstr<string>(params, " ");

    if(vDyeingParams.size() == 2 || vDyeingParams.size() == 3)
    {
        ServantHelperManager::getInstance()->setDyeing(vDyeingParams[0], vDyeingParams[1], vDyeingParams.size() == 3 ? vDyeingParams[2] : "");
        
        result = "DyeingKey="       + vDyeingParams[0] + "\r\n" +
                 "DyeingServant="   + vDyeingParams[1] + "\r\n" +
                 "DyeingInterface=" + (vDyeingParams.size() == 3 ? vDyeingParams[2] : "") + "\r\n";
    }
    else
    {
        result = "Invalid parameters.Should be: dyeingKey dyeingServant [dyeingInterface]";
    }
    return true;
}

bool Application::cmdCloseCout(const string& command, const string& params, string& result)
{
    LOG->info() << "Application::cmdCloseCout:" << command << " " << params << endl;

    string s = TC_Common::lower(TC_Common::trim(params));

    if(s == "yes")
    {
        AppCache::getInstance()->set("closeCout","1");
    }
    else
    {
        AppCache::getInstance()->set("closeCout","0");
    }

    result = "set closeCout  [" + s + "] ok";

    return true;
}

void Application::outAllAdapter(ostream &os)
{
    map<int, TC_EpollServer::BindAdapterPtr> m = _epollServer->getListenSocketInfo();

    for(map<int, TC_EpollServer::BindAdapterPtr>::const_iterator it = m.begin(); it != m.end(); ++it)
    {
        outAdapter(os, ServantHelperManager::getInstance()->getAdapterServant(it->second->getName()),it->second);

        os << OUT_LINE << endl;
    }
}

bool Application::addConfig(const string &filename)
{
    string result;

    if (TafRemoteConfig::getInstance()->addConfig(filename, result, false))
    {
        TafRemoteNotify::getInstance()->report(result);

        return true;
    }
    TafRemoteNotify::getInstance()->report(result);

    return true;
}

bool Application::addAppConfig(const string &filename)
{
    string result = "";

    // true-ֻ��ȡӦ�ü�������
    if (TafRemoteConfig::getInstance()->addConfig(filename, result, true))
    {
        TafRemoteNotify::getInstance()->report(result);

        return true;
    }

    TafRemoteNotify::getInstance()->report(result);

    return true;
}

void Application::setHandle(TC_EpollServer::BindAdapterPtr& adapter)
{
    adapter->setHandle<ServantHandle>();
}

void Application::main(int argc, char *argv[])
{
    try
    {
        TC_Common::ignorePipe();

        //���������ļ�
        parseConfig(argc, argv);

        //��ʼ��Proxy����
        initializeClient();

        //��ʼ��Server����
        initializeServer();

        vector<TC_EpollServer::BindAdapterPtr> adapters;

        //�󶨶���Ͷ˿�
        bindAdapter(adapters);

        //ҵ��Ӧ�õĳ�ʼ��
        initialize();

        //�������adapter
        outAllAdapter(cout);

        //����HandleGroup���飬�����߳�
        for (size_t i = 0; i < adapters.size(); ++i)
        {
            string name = adapters[i]->getName();

            string groupName = adapters[i]->getHandleGroupName();

            if(name != groupName)
            {
                TC_EpollServer::BindAdapterPtr ptr = _epollServer->getBindAdapter(groupName);

                if (!ptr)
                {
                    throw runtime_error("[TAF][adater `" + name + "` setHandle to group `" + groupName + "` fail!");
                }

            }
            setHandle(adapters[i]);

            /*
            if (name == groupName)
            {
                setHandle(adapters[i]);

                continue;
            }

            TC_EpollServer::BindAdapterPtr ptr = _epollServer->getBindAdapter(groupName);

            if (!ptr)
            {
                throw runtime_error("[TAF][adater `" + name + "` setHandle to group `" + groupName + "` fail!");
            }

            adapters[i]->setHandle(ptr);
            */
        }

        //����ҵ�����߳�
        _epollServer->startHandle();

        cout << "\n" << outfill("[initialize server] ", '.')  << " [Done]" << endl;

        cout << OUT_LINE_LONG << endl;

        //��̬���������ļ�
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_LOAD_CONFIG, Application::cmdLoadConfig);

        //��̬������־�ȼ�
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_SET_LOG_LEVEL, Application::cmdSetLogLevel);

        //�鿴����״̬
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_VIEW_STATUS, Application::cmdViewStatus);

        //�鿴��ǰ����״̬
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_CONNECTIONS, Application::cmdConnections);

        //�鿴�����TAF�汾
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_VIEW_VERSION, Application::cmdViewVersion);

        //���������ļ��е�������Ϣ
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_LOAD_PROPERTY, Application::cmdLoadProperty);

        //�鿴����֧�ֵĹ�������
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_VIEW_ADMIN_COMMANDS, Application::cmdViewAdminCommands);

        //����Ⱦɫ��Ϣ
        TAF_ADD_ADMIN_CMD_PREFIX(TAF_CMD_SET_DYEING, Application::cmdSetDyeing);

        //�ϱ��汾
        TAF_REPORTVERSION(TAF_VERSION);

        //����������node, ��ʾ������
        TAF_KEEPALIVE("");

        //���͸�notify��ʾ����������
        TafRemoteNotify::getInstance()->report("restart");

        //ctrl + c�ܹ�������������
        signal(SIGINT, sighandler);

      
        if(_conf.get("/taf/application/server<closecout>",AppCache::getInstance()->get("closeCout")) != "0")
        {
            // �ض���stdin��stdout��stderr
            int fd = open("/dev/null", O_RDWR );    
            if(fd != -1)
            {
               dup2(fd, 0);
               dup2(fd, 1);
               dup2(fd, 2); 
            }
            else
            {
               close(0);
               close(1);
               close(2); 
            }
        }
    }
    catch (exception &ex)
    {
        TafRemoteNotify::getInstance()->report("exit: " + string(ex.what()));

        cout << "[main exception]:" << ex.what() << endl;

        terminate();
    }

    //��ʼ����Ϻ�, ��־���޸�Ϊ�첽
    TafRollLogger::getInstance()->sync(false);
}

void Application::parseConfig(int argc, char *argv[])
{
    TC_Option op;

    op.decode(argc, argv);

    //ֱ����������TAF�汾
    if(op.hasParam("version"))
    {
        cout << "TAF:" << TAF_VERSION << endl;
        exit(0);
    }

    //���������ļ�
    ServerConfig::ConfigFile = op.getValue("config");

    if(ServerConfig::ConfigFile == "")
    {
        cerr << "start server with config, for example: " << argv[0] << " --config=config.conf" << endl;

        exit(0);
    }

    _conf.parseFile(ServerConfig::ConfigFile);
}

TC_EpollServer::BindAdapter::EOrder Application::parseOrder(const string &s)
{
    vector<string> vtOrder = TC_Common::sepstr<string>(s,";, \t", false);

    if(vtOrder.size() != 2)
    {
        cerr << "invalid order '" << TC_Common::tostr(vtOrder) << "'."<< endl;
        
        exit(0);
    }
    if((TC_Common::lower(vtOrder[0]) == "allow")&&(TC_Common::lower(vtOrder[1]) == "deny"))
    {
        return TC_EpollServer::BindAdapter::ALLOW_DENY;
    }
    if((TC_Common::lower(vtOrder[0]) == "deny")&&(TC_Common::lower(vtOrder[1]) == "allow"))
    {
         return TC_EpollServer::BindAdapter::DENY_ALLOW;
    }
    
     cerr << "invalid order '" << TC_Common::tostr(vtOrder) << "'."<< endl;
             
     exit(0);    
}

void Application::initializeClient()
{
    cout << "\n" << OUT_LINE_LONG << endl;

    //��ʼ��ͨ����
    _communicator = CommunicatorFactory::getInstance()->getCommunicator(_conf);
    
    cout << outfill("[proxy config]:") << endl;
    
    //���
    outClient(cout);
}

void Application::outClient(ostream &os)
{
    os << outfill("locator")                     << _communicator->getProperty("locator") << endl;
    os << outfill("sync-invoke-timeout")         << _communicator->getProperty("sync-invoke-timeout") << endl;
    os << outfill("async-invoke-timeout")        << _communicator->getProperty("async-invoke-timeout") << endl;
    os << outfill("refresh-endpoint-interval")   << _communicator->getProperty("refresh-endpoint-interval") << endl;
    os << outfill("stat")                        << _communicator->getProperty("stat") << endl;
    os << outfill("property")                    << _communicator->getProperty("property") << endl;
    os << outfill("report-interval")             << _communicator->getProperty("report-interval") << endl;
    os << outfill("sample-rate")                 << _communicator->getProperty("sample-rate") << endl;
    os << outfill("max-sample-count")            << _communicator->getProperty("max-sample-count") << endl;
    os << outfill("sendthread")                  << _communicator->getProperty("sendthread") << endl;
    os << outfill("recvthread")                  << _communicator->getProperty("recvthread") << endl;
    os << outfill("asyncthread")                 << _communicator->getProperty("asyncthread") << endl;
    os << outfill("modulename")                  << _communicator->getProperty("modulename") << endl;
}

string Application::toDefault(const string &s, const string &sDefault)
{
    if(s.empty())
    {
        return sDefault;
    }
    return s;
}

void Application::addServantProtocol(const string& servant, const TC_EpollServer::protocol_functor& protocol)
{
    string adapterName = ServantHelperManager::getInstance()->getServantAdapter(servant);

    if (adapterName == "")
    {
        throw runtime_error("[TAF]addServantProtocol fail, no found adapter for servant:" + servant);
    }
    getEpollServer()->getBindAdapter(adapterName)->setProtocol(protocol);
}

void Application::initializeServer()
{
    cout << OUT_LINE << "\n" << outfill("[server config]:") << endl;

    ServerConfig::Application  = toDefault(_conf.get("/taf/application/server<app>"), "UNKNOWN");

    //ȱʡ���ý�������
    string exe = "";

    try
    {
        exe = TC_File::extractFileName(TC_File::getExePath());
    }
    catch(TC_File_Exception & ex)
    {
        //ȡʧ����ʹ��ip���������
        exe = _conf.get("/taf/application/server<localip>");
    }

    ServerConfig::ServerName        = toDefault(_conf.get("/taf/application/server<server>"), exe);
    ServerConfig::BasePath          = toDefault(_conf.get("/taf/application/server<basepath>"), ".") + "/";
    ServerConfig::DataPath          = toDefault(_conf.get("/taf/application/server<datapath>"), ".") + "/";
    ServerConfig::LogPath           = toDefault(_conf.get("/taf/application/server<logpath>"),  ".") + "/";
    ServerConfig::LogSize           = TC_Common::toSize(toDefault(_conf.get("/taf/application/server<logsize>"), "52428800"), 52428800);
    ServerConfig::LogNum            = TC_Common::strto<int>(toDefault(_conf.get("/taf/application/server<lognum>"), "10"));
    ServerConfig::LocalIp           = _conf.get("/taf/application/server<localip>");
    ServerConfig::Local             = _conf.get("/taf/application/server<local>");
    ServerConfig::Node              = _conf.get("/taf/application/server<node>");
    ServerConfig::Log               = _conf.get("/taf/application/server<log>");
    ServerConfig::Config            = _conf.get("/taf/application/server<config>");
    ServerConfig::Notify            = _conf.get("/taf/application/server<notify>");
    ServerConfig::ReportFlow        = _conf.get("/taf/application/server<reportflow>")=="0"?0:1;

    if(ServerConfig::LocalIp.empty())
    {
        vector<string> v = TC_Socket::getLocalHosts();

        ServerConfig::LocalIp = "127.0.0.1";
        //��ȡ��һ����127.0.0.1��IP
        for(size_t i = 0; i < v.size(); i++)
        {
            if(v[i] != "127.0.0.1")
            {
                ServerConfig::LocalIp = v[i];
                break;
            }
        }
    }

    //�����Ϣ
    outServer(cout);

    _epollServer = new TC_EpollServer();

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ�������ļ�cache
    cout << OUT_LINE << "\n" << outfill("[set file cache ]") << "OK" << endl;
    AppCache::getInstance()->setCacheInfo(ServerConfig::DataPath+ServerConfig::ServerName+".tafdat",0);
        
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ������Log
    cout << OUT_LINE << "\n" << outfill("[set roll logger] ") << "OK" << endl;
    TafRollLogger::getInstance()->setLogInfo(ServerConfig::Application, ServerConfig::ServerName, ServerConfig::LogPath, ServerConfig::LogSize, ServerConfig::LogNum, _communicator, ServerConfig::Log);
    _epollServer->setLocalLogger(TafRollLogger::getInstance()->logger());

    //��ʼ������־Ϊͬ��
    TafRollLogger::getInstance()->sync(true);

    //������־����
    string level = AppCache::getInstance()->get("logLevel");

    if(!level.empty())
    {
        TafRollLogger::getInstance()->logger()->setLogLevel(level);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ����LogServer����
    cout << OUT_LINE << "\n" << outfill("[set time logger] ") << "OK" << endl;
    TafTimeLogger::getInstance()->setLogInfo(_communicator, ServerConfig::Log, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::LogPath);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ�����������Ĵ���
    cout << OUT_LINE << "\n" << outfill("[set remote config] ") << "OK" << endl;
    TafRemoteConfig::getInstance()->setConfigInfo(_communicator, ServerConfig::Config, ServerConfig::Application, ServerConfig::ServerName, ServerConfig::BasePath);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ������Ϣ���Ĵ���
    cout << OUT_LINE << "\n" << outfill("[set remote notify] ") << "OK" << endl;
    TafRemoteNotify::getInstance()->setNotifyInfo(_communicator, ServerConfig::Notify, ServerConfig::Application, ServerConfig::ServerName);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ����Node�Ĵ���
    cout << OUT_LINE << "\n" << outfill("[set node proxy]") << "OK" << endl;
    TafNodeFHelper::getInstance()->setNodeInfo(_communicator, ServerConfig::Node, ServerConfig::Application, ServerConfig::ServerName);
       
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //��ʼ���������
    cout << OUT_LINE << "\n" << outfill("[set admin adapter]") << "OK" << endl;

    if(!ServerConfig::Local.empty())
    {
        ServantHelperManager::getInstance()->addServant<AdminServant>("AdminObj");

        ServantHelperManager::getInstance()->setAdapterServant("AdminAdapter", "AdminObj");

        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(_epollServer.get());

        lsPtr->setName("AdminAdapter");

        lsPtr->setEndpoint(ServerConfig::Local);

        lsPtr->setMaxConns(TC_EpollServer::BindAdapter::DEFAULT_MAX_CONN);

        lsPtr->setQueueCapacity(TC_EpollServer::BindAdapter::DEFAULT_QUEUE_CAP);

        lsPtr->setQueueTimeout(TC_EpollServer::BindAdapter::DEFAULT_QUEUE_TIMEOUT);

        lsPtr->setProtocolName("taf");

        lsPtr->setProtocol(AppProtocol::parse);

        lsPtr->setHandleGroupName("AdminAdapter");

        lsPtr->setHandleNum(1);

        lsPtr->setHandle<ServantHandle>();

        _epollServer->bind(lsPtr);
    }
}

void Application::outServer(ostream &os)
{
    os << outfill("Application")        << ServerConfig::Application << endl;
    os << outfill("ServerName")         << ServerConfig::ServerName << endl;
    os << outfill("BasePath")           << ServerConfig::BasePath << endl;
    os << outfill("DataPath")           << ServerConfig::DataPath << endl;
    os << outfill("LocalIp")            << ServerConfig::LocalIp << endl;
    os << outfill("Local")              << ServerConfig::Local << endl;
    os << outfill("LogPath")            << ServerConfig::LogPath << endl;
    os << outfill("LogSize")            << ServerConfig::LogSize << endl;
    os << outfill("LogNum")             << ServerConfig::LogNum << endl;
    os << outfill("Log")                << ServerConfig::Log << endl;
    os << outfill("Node")               << ServerConfig::Node << endl;
    os << outfill("Config")             << ServerConfig::Config << endl;
    os << outfill("Notify")             << ServerConfig::Notify << endl;
    os << outfill("CloseCout")          << TC_Common::tostr(_conf.get("/taf/application/server<closecout>",AppCache::getInstance()->get("closeCout")) == "0"?0:1)<< endl;
    os << outfill("ReportFlow")         << ServerConfig::ReportFlow<< endl;    
}

void Application::bindAdapter(vector<TC_EpollServer::BindAdapterPtr>& adapters)
{
    string sPrefix = ServerConfig::Application + "." + ServerConfig::ServerName + ".";

    vector<string> adapterName;

    map<string, ServantHandle*> servantHandles;

    if (_conf.getDomainVector("/taf/application/server", adapterName))
    {
        for (size_t i = 0; i < adapterName.size(); i++)
        {
            string servant = _conf.get("/taf/application/server/" + adapterName[i] + "<servant>");

            checkServantNameValid(servant, sPrefix);

            //TC_EpollServer::BindAdapterPtr bindAdapter = bind("/taf/application/server", adapterName[i], servant);

            ServantHelperManager::getInstance()->setAdapterServant(adapterName[i], servant);

            TC_EpollServer::BindAdapterPtr bindAdapter = new TC_EpollServer::BindAdapter(_epollServer.get());

            string sLastPath = "/taf/application/server/" + adapterName[i];

            bindAdapter->setName(adapterName[i]);

            bindAdapter->setEndpoint(_conf[sLastPath + "<endpoint>"]);

            bindAdapter->setMaxConns(TC_Common::strto<int>(_conf.get(sLastPath + "<maxconns>", "128")));

            bindAdapter->setOrder(parseOrder(_conf.get(sLastPath + "<order>","allow,deny")));

            bindAdapter->setAllow(TC_Common::sepstr<string>(_conf[sLastPath + "<allow>"], ";,", false));

            bindAdapter->setDeny(TC_Common::sepstr<string>(_conf.get(sLastPath + "<deny>",""), ";,", false));

            bindAdapter->setQueueCapacity(TC_Common::strto<int>(_conf.get(sLastPath + "<queuecap>", "1024")));

            bindAdapter->setQueueTimeout(TC_Common::strto<int>(_conf.get(sLastPath + "<queuetimeout>", "10000")));

            bindAdapter->setProtocolName(_conf.get(sLastPath + "<protocol>", "taf"));

            if (bindAdapter->isTafProtocol())
            {
                bindAdapter->setProtocol(AppProtocol::parse);
            }

            bindAdapter->setHandleGroupName(_conf.get(sLastPath + "<handlegroup>", adapterName[i]));

            bindAdapter->setHandleNum(TC_Common::strto<int>(_conf.get(sLastPath + "<threads>", "0")));
            
            bindAdapter->setShm(TC_Common::strto<int>(_conf.get(sLastPath + "<shmkey>", "0")), TC_Common::strto<int>(_conf.get(sLastPath + "<shmcap>", "0")));
            
            bindAdapter->initialize();

            _epollServer->bind(bindAdapter);

            adapters.push_back(bindAdapter);

            //����ȡƽ��ֵ
            if(!_communicator->getProperty("property").empty())
            {
                _communicator->getStatReport()->createPropertyReport(bindAdapter->getName() + ".queue", PropertyReport::avg());
            }
        }
    }
}

void Application::checkServantNameValid(const string& servant, const string& sPrefix)
{
    if((servant.length() <= sPrefix.length()) || (servant.substr(0, sPrefix.length()) != sPrefix))
    {
        ostringstream os;

        os << "Servant '" << servant << "' error: must be start with '" << sPrefix << "'";

        TafRemoteNotify::getInstance()->report("exit:" + os.str());

        cout << os.str() << endl;

        terminate();

        exit(-1);
    }
}

void Application::outAdapter(ostream &os, const string &v, TC_EpollServer::BindAdapterPtr lsPtr)
{
    os << outfill("name")             << lsPtr->getName() << endl;
    os << outfill("servant")          << v << endl;
    os << outfill("endpoint")         << lsPtr->getEndpoint().toString() << endl;
    os << outfill("maxconns")         << lsPtr->getMaxConns() << endl;
    os << outfill("queuecap")         << lsPtr->getQueueCapacity() << endl;
    os << outfill("queuetimeout")     << lsPtr->getQueueTimeout() << "ms" << endl;
    os << outfill("order")            << (lsPtr->getOrder()==TC_EpollServer::BindAdapter::ALLOW_DENY?"allow,deny":"deny,allow") << endl;
    os << outfill("allow")            << TC_Common::tostr(lsPtr->getAllow()) << endl;
    os << outfill("deny")             << TC_Common::tostr(lsPtr->getDeny()) << endl;
    os << outfill("queuesize")        << lsPtr->getRecvBufferSize() << endl;
    os << outfill("connections")      << lsPtr->getNowConnection() << endl;
    os << outfill("protocol")         << lsPtr->getProtocolName() << endl;
    os << outfill("handlegroup")      << lsPtr->getHandleGroupName() << endl;
    os << outfill("handlethread")     << lsPtr->getHandleNum() << endl;
    os << outfill("shmkey")           << lsPtr->getShmKey() << endl;
    os << outfill("shmcap")           << lsPtr->getShmSize() << endl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
}

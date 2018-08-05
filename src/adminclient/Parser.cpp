#include "Parser.h"
#include "servant/Application.h"

#ifdef HAVE_READLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif

extern FILE* yyin;
extern int yydebug;

using namespace std;
//using namespace taf;


Parser* parser;

Parser::Parser(const AdminRegPrx& admin, const NodePrx & node) :
    _admin(admin),
    _node(node)
{
}

ParserPtr Parser::createParser(AdminRegPrx admin, NodePrx node)
{
    return new Parser(admin, node);
}

void Parser::usage()
{
    cout <<
        "##����                                 \t\t ����\n"
        "help                                   \t\t ��ӡ������Ϣ.\n"
        "exit, quit                             \t\t �˳�.\n"
        "\n"
        "app list                               \t\t Ӧ���б�.\n"
        "\n"
        "node list                              \t\t �ڵ��б�.\n"
        "node ping NODE                         \t\t ping�ڵ�.\n"
        "node shutdown NODE                     \t\t ֹͣ�ڵ�.\n"
        "node server start APP.SERVER           \t\t ͨ��node��������(��֪ͨ���أ���������).\n"
        "node server stop  APP.SERVER           \t\t ͨ��nodeֹͣ����(��֪ͨ���أ���������).\n"
        "\n"
        "server list   [more]                   \t\t server�б�.\n"
        "server state  APP.SERVER_NODE          \t\t server״̬.\n"
        "server start  APP.SERVER_NODE          \t\t ����server.\n"
        "server stop   APP.SERVER_NODE          \t\t ֹͣserver.\n"
        "server restart APP.SERVER_NODE         \t\t ����server.\n"
        "server load   APP.SERVER_NODE          \t\t server���¼�������.\n"
        "server notify APP[.SERVER][_NODE] MESSAGE  \t ֪ͨserver.\n"
        "server notify APP1.SERVER1_NODE1 ... MESSAGE  \t ֪ͨ���server.\n"
        "server patch  APP.SERVER_NODE          \t\t ����server.\n"
        "server patch  subborn [APP.SERVER_]NODE SRC DST\t ��node����ָ���ļ�.\n"
        "server patch  percent APP.SERVER_NODE  \t\t ����server����.\n"      
        "\n"
        "profile get   PROFILE                  \t\t ��ȡģ��.\n"
        "\n"
        ;
}

void Parser::getAllApplicationNames()
{
    try
    {
        string sResult;
        cout << "##Ӧ�÷����б�" <<endl;
        vector<string> vApps =  _admin->getAllApplicationNames(sResult);
        if(vApps.size() == 0)
        {
            cout<<sResult<<endl;
        }
        for(unsigned i=0; i<vApps.size(); i++)
        {
            cout<<vApps[i]<<endl;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::getAllNodeNames()
{
    try
    {
        string sResult;
        cout << "##�ڵ��б�" <<endl;
        vector<string> vNodes = _admin->getAllNodeNames(sResult);

        if(vNodes.size() == 0)
        {
            cout<<sResult<<endl;
        }
        for(unsigned i=0; i<vNodes.size(); i++)
        {
            cout<< vNodes[i] <<endl;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::pingNode(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: node ping NODE");
        return;
    }

    try
    {
        string sResult;
        cout << "##�ڵ�״̬" << endl;
        if(_admin->pingNode(args.front(), sResult))
        {
            cout << "node '"<< args.front() <<"' is up" << endl;
        }
        else
        {
            cout << "node '"<< args.front() <<"' is down. " << sResult << endl;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::shutdownNode(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: node shutdown NODE");
        return;
    }
    try
    {
        string sResult;
        cout << "##shutdown�ڵ���" << endl;
        cout<< (_admin->shutdownNode(args.front(), sResult) == 0 ? "�ɹ�" : "ʧ��."+sResult ) <<endl;
    }
    catch(const TafException& ex)
    {
        cout<<"ʧ��:" << ex.what()<<endl;
    }
}


void Parser::startServerByNode(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: node server start APP.SERVER");
        return;
    }
    try
    {
        string sResult;
        cout << "##stateServerByNode���" << endl;

        vector<string> vServerArgs = serverIdParse(args[0] + "_");
        cout<< _node->startServer(vServerArgs[0], vServerArgs[1], sResult)<<endl;
        cout<< sResult << endl;
    }
    catch(const TafException& ex)
    {
        cout<<"ʧ��:" << ex.what()<<endl;
    }
}


void Parser::stopServerByNode(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: node server stop APP.SERVER");
        return;
    }
    try
    {
        string sResult;
        cout << "##stopServerByNode���" << endl;

        vector<string> vServerArgs = serverIdParse(args[0] + "_");
        cout<< _node->stopServer(vServerArgs[0], vServerArgs[1], sResult)<<endl;
        cout<< sResult << endl;
    }
    catch(const TafException& ex)
    {
        cout<<"ʧ��:" << ex.what()<<endl;
    }
}


void Parser::getAllServerIds(const vector<string>& args)
{
    //Ĭ�ϲ��ֶ�չʾ�����б�

    bool sliced     = false;

    //�����÷ֶ�չʾʱ Ĭ��0�п�ʼ 1��20��չʾ
    unsigned iLineNum    = 0;
    unsigned iCount      = 20;
    static unsigned giLastLineNum = 0;

    if(args.size() >= 1)
    {
        if(args[0] != "more")
        {
            invalidCommand("Usage: server list [more [ linenum [ count ] ]");
            return;
        }
        sliced = true;
        iLineNum = args.size() >=2?TC_Common::strto<int>(args[1]):giLastLineNum;
        if(args.size() >=3) iCount = TC_Common::strto<int>(args[2]);
    }

    try
    {
        cout.flags (ios::left);

        string sResult;
        _vServer = _admin->getAllServerIds(sResult);

        if(_vServer.size() == 0)
        {
            cout<<sResult<<endl;
            return;
        }
        sort(_vServer.begin(),_vServer.end());
        vector<vector<string> > vServer;

        if(sliced == false)
        {
            _vServer.swap(vServer);
        }
        else
        {
            if(iLineNum >= _vServer.size()-1)
            {
                iLineNum = 0;
            }
            for(unsigned i = iLineNum; i<_vServer.size(); i++)
            {
                vServer.push_back(_vServer[i]);
                giLastLineNum = i;
                if(vServer.size() >= iCount) break;
            }   
        }

        unsigned *length = new unsigned[vServer[0].size()];
        memset(length, 0, sizeof(unsigned) * vServer[0].size());
        for(unsigned i=0; i<vServer.size(); i++)
        {
            for(unsigned j=0; j<vServer[i].size(); j++)
            {
                if(length[j] < 16) length[i] = 16;
                length[j] = (vServer[i][j].length()+5 > length[j]) ? vServer[i][j].length()+5 : length[j];
            }
        }
        cout << setw(length[0]) << "##server����"<< setw(length[1])
                                << "������״̬" << setw(length[2]) 
                                << "Ŀǰ״̬(������db)"<< setw(length[3])
                                << "��������"<<endl;

        for(unsigned i=0; i<vServer.size(); i++)
        {
            for(unsigned j=0; j<vServer[i].size(); j++)
            {
                cout<<setw(length[j])<<vServer[i][j];
            }
            cout<<endl;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::stateServer(const vector<string>& args)
{
    cout.flags (ios::left);

    if(args.size() != 1)
    {
        invalidCommand("Usage: server state APP.SERVER_NODE");
        return;
    }

    try
    {
        string sResult;
        vector<string> vServerArgs = serverIdParse(args[0]);
        ServerStateDesc state;
        int iRet = _admin->getServerState(vServerArgs[0], vServerArgs[1], vServerArgs[2], state, sResult);
        if(iRet == 0)
        {
            cout << setw(24) <<"##�����ص�����״̬";
            cout << setw(24) <<"�����صĵ�ǰ״̬";
            cout << setw(24) <<"�ڽڵ�ĵ�ǰ״̬";
            cout << setw(24) <<"����id" << endl;
            cout << setw(24) << state.settingStateInReg;
            cout << setw(24) << state.presentStateInReg;
            cout << setw(24) << state.presentStateInNode;
            cout << setw(24) << state.processId << endl;
        }
        else
        {
            cout << sResult << endl;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::startServer(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: server start APP.SERVER_NODE");
        return;
    }

    try
    {
        string sResult;
        cout<< "##����server���(0=�ɹ�)" <<endl;
        vector<string> vServerArgs = serverIdParse(args[0]);
        cout<< _admin->startServer(vServerArgs[0], vServerArgs[1], vServerArgs[2], sResult)<<endl;
        cout<< sResult << endl;
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::stopServer(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: server stop APP.SERVER_NODE");
        return;
    }

    try
    {
        string sResult;
        cout<< "##ֹͣserver���(0=�ɹ�) " <<endl;
        vector<string> vServerArgs = serverIdParse(args[0]);
        cout<< _admin->stopServer(vServerArgs[0], vServerArgs[1], vServerArgs[2], sResult)<<endl;
        cout<< sResult << endl;
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::restartServer(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: server restart APP.SERVER_NODE");
        return;
    }

    try
    {
        string sResult;
        cout<< "##����server���(0=�ɹ�) " <<endl;
        vector<string> vServerArgs = serverIdParse(args[0]);
        cout<< _admin->restartServer(vServerArgs[0], vServerArgs[1], vServerArgs[2], sResult)<<endl;
        cout<< sResult << endl;
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::notifyServer(const vector<string>& args)
{
    if(args.size() < 2)
    {
        invalidCommand("Usage: server notify APP[.SERVER][_NODE] MESSAGE");
        return;
    }
    try
    {
        string sResult;
        vector<string> vServerId;
        bool  bBatch = false;
        //��Ϊ����֪ͨ server notify APP msg �� server notify APP.SERVER msg ��ʽ
        if( args.size() == 2 && (args[0].find("_") == string::npos || args[0].find(".") == string::npos))
        {
            bBatch = true;
            string  s;
            vector<vector<string> > vServer = _admin->getAllServerIds(s);
            if(vServer.size() == 0)
            {
                cout<< s << endl;
                return;
            }
            for(unsigned i = 0; i< vServer.size();i++)
            {
                vector<string> v = serverIdParse(vServer[i].at(0));
                if(v[0] == args[0]  || v[0]+"."+v[1] == args[0])
                vServerId.push_back(vServer[i].at(0));
            }

            if(vServerId.size() == 0)
            {
               invalidCommand("Usage: server notify APP[.SERVER][_NODE] MESSAGE");                
               return;
            }
        }
        else
        {
            for(unsigned i = 0; i< args.size()-1;i++)
            {
                vServerId.push_back(args[i]);
            }
        }

        for(unsigned i=0; i< vServerId.size();i++)
        {
            vector<string> vServerArgs = serverIdParse(vServerId[i]);
            //node ָ��
            if(bBatch )
            {              
                cout.fill('-'); 
                cout.width(60);
                cout<<endl<<"("+TC_Common::tostr(i+1)+"'th in "+TC_Common::tostr(vServerId.size())+")"<<endl;
             
            }
            cout<< "##֪ͨ"<<vServerId[i]<<"���(0=�ɹ�) " <<endl;      
            cout<< _admin->notifyServer(vServerArgs[0], vServerArgs[1], vServerArgs[2], args[args.size()-1], sResult)<<endl;               
            cout<< sResult << endl;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
    cout<<setfill(' '); 
}

void Parser::patchServer(const vector<string>& args)
{
    if(args.size() < 1)
    {
        invalidCommand("Usage: server patch APP.SERVER_NODE [VERSION]");
        return;
    }

    try
    {
        int iRet =  -1;
        string sResult;
        string sSevrerVersion = args.size()>1?args[1]:"";
        vector<string> vServerArgs = serverIdParse(args[0]);
        iRet = _admin->patchServer(vServerArgs[0], vServerArgs[1], vServerArgs[2], false, sSevrerVersion, "", sResult);
        cout.setf(ios::left,ios::adjustfield);
        cout<<setw(24)<<"##����server(0=�ɹ�) " ;
        cout<<setw(24)<< "�������"<<endl;
        cout<<setw(24)<<iRet;
        cout<<setw(24)<<sResult << endl<<endl;


        //���1.5.0�汾��node ���ý��Ȳ鿴
        string sNodeVersion = "";
        iRet = _admin->getNodeVesion(vServerArgs[2],sNodeVersion,sResult);
        if(iRet != 0)
        {
            return;
        }
        vector<int> vi = TC_Common::sepstr<int>(sNodeVersion,".");
        if(vi.size() < 3)
        {
            return;
        }
        //�жϰ汾
        if(vi[0] < 1 || (vi[0] == 1 && vi[1] < 5) || (vi[0] == 1 && vi[1] == 5 && vi[2] == 0))
        {
            return;
        }   
        vector<string> vs;
        vs.push_back(args[0]);
        vs.push_back("-1");
        getPatchPercent(vs);
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::patchSubborn(const vector<string>& args)
{
    if(args.size() != 3)
    {
        invalidCommand("Usage: server patch subborn APP.SERVER_NODE|NODE SRC_FILE DST_FILE");
        return;
    }

    //��ȡ����
    string sApplication, sServerName, sNodeName;

    try
    {
        vector<string> vServerArgs = serverIdParse(args[0]);
        sApplication = vServerArgs[0];
        sServerName  = vServerArgs[1];
        sNodeName    = vServerArgs[2];
    }
    catch (TafException & ex)
    {
        sNodeName = args[0];
    }
    catch (...)
    {
        std::cout << "unkown exception:split APP.SERVER_NODE|NODE error" << std::endl;
        return ;
    }

    //��������
    try
    {
        string sResult;
        int iRet = _admin->patchSubborn(sApplication, sServerName, sNodeName, args[1], args[2], sResult);

        cout.setf(ios::left,ios::adjustfield);
        cout<<setw(24)<<"##�����ļ�(0=�ɹ�) " ;
        cout<<setw(24)<< "�������"<<endl;
        cout<<setw(24)<<iRet;
        cout<<setw(24)<<sResult << endl<<endl;

        if (!sApplication.empty() && !sServerName.empty())
        {
            vector<string> vs;
            vs.push_back(args[0]);
            vs.push_back("-1");
            getPatchPercent(vs);
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::getPatchPercent(const vector<string>& args)
{
    if(args.size() < 1)
    {
        invalidCommand("Usage: server patch percent APP.SERVER_NODE [COUNT]");
        return;
    }
    try
    {

        int iRet        = 0;
        PatchInfo tPatchInfo;
        int iCount      = args.size()>=2?TC_Common::strto<int>(args[1]):5;
        string sResult;
        vector<string> vServerArgs = serverIdParse(args[0]);
        cout.setf(ios::left,ios::adjustfield);
        cout<<setw(24)<< "##��ȡ����(0=�ɹ�)";
        cout<<setw(24)<< "���Ȱٷֱ�";
        cout<<setw(24)<< "����޸�ʱ��";
        cout<<setw(24)<< "�汾";
        cout<<setw(24)<< "�������"<<endl;
        

        while(1)
        {
            int i   = 0;
            iRet    = _admin->getPatchPercent(vServerArgs[0], vServerArgs[1], vServerArgs[2], tPatchInfo);
            cout<<setw(24)<<iRet;
            cout<<setw(24)<<tPatchInfo.iPercent;
            cout<<setw(24)<<TC_Common::tm2str(tPatchInfo.iModifyTime,"%Y-%m-%d %H:%M:%S"); 
            cout<<setw(24)<<tPatchInfo.sVersion;
            cout<<setw(24)<<tPatchInfo.sResult << endl;
            if(iRet == 0 && (iCount == -1 || i< iCount) && tPatchInfo.bPatching == true)
            {    
                sleep(1);
                i++;
                continue;
            }
            break;
        }
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::loadServer(const vector<string>& args)
{
    if(args.size() != 1)
    {
        invalidCommand("Usage: server load APP.SERVER_NODE");
        return;
    }

    try
    {
        string sResult;
        cout<< "##����server���(0=�ɹ�) " <<endl;
        vector<string> vServerArgs = serverIdParse(args[0]);
        cout<< _admin->loadServer(vServerArgs[0], vServerArgs[1], vServerArgs[2], sResult)<<endl;
        cout<< sResult << endl;
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::getProfileTemplate(const vector<string>& args)
{

    if(args.size() != 1)
    {
        invalidCommand("Usage: profile get   PROFILE");
        return;
    }

    string sResultDesc, sProfileTemplate;
    try
    {
        _admin->getProfileTemplate(args[0], sProfileTemplate, sResultDesc);
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }

    cout<<sResultDesc << endl << endl;
    cout<<sProfileTemplate << endl;
}

void Parser::shutdown()
{
    try
    {
        invalidCommand("'shutdown registry' not implement");
        //_admin->shutdown();
    }
    catch(const TafException& ex)
    {
        cout<<ex.what()<<endl;
    }
}

void Parser::showBanner()
{
    cout << "TAF " << TAF_VERSION << " by WSRD Tencent." << endl;
}

void Parser::getInput(char* buf, int& result, int maxSize)
{
    if(!_commands.empty())
    {
        if(_commands == ";")
        {
            result = 0;
        }
        else
        {
            result = min(maxSize, static_cast<int>(_commands.length()));
            strncpy(buf, _commands.c_str(), result);
            _commands.erase(0, result);
            if(_commands.empty())
            {
                _commands = ";";
            }
        }
    }
    else if(isatty(fileno(yyin)))
    {

#ifdef HAVE_READLINE

        const char* prompt = parser->getPrompt();
        char* line = readline(const_cast<char*>(prompt));
        if(!line)
        {
            result = 0;
        }
        else
        {
            if(*line)
            {
                add_history(line);
            }

            result = strlen(line) + 1;
            if(result > maxSize)
            {
                free(line);
                error("input line too long");
                result = 0;
            }
            else
            {
                strcpy(buf, line);
                strcat(buf, "\n");
                free(line);
            }
        }

#else

        cout << parser->getPrompt() << flush;

        string line;
        while(true)
        {
            char c = static_cast<char>(getc(yyin));
            if(c == EOF)
            {
                if(line.size())
                {
                    line += '\n';
                }
                break;
            }

            line += c;

            if(c == '\n')
            {
                break;
            }
        }

        result = (int) line.length();
        if(result > maxSize)
        {
            error("input line too long");
            buf[0] = EOF;
            result = 1;
        }
        else
        {
            strcpy(buf, line.c_str());
        }

#endif

    }
    else
    {
        if(((result = (int) fread(buf, 1, maxSize, yyin)) == 0) && ferror(yyin))
        {
            error("input in flex scanner failed");
            buf[0] = EOF;
            result = 1;
        }
    }
}

void Parser::nextLine()
{
    _currentLine++;
}

void Parser::continueLine()
{
    _continue = true;
}

const char* Parser::getPrompt()
{
    assert(_commands.empty() && isatty(fileno(yyin)));

    if(_continue)
    {
        _continue = false;
        return "(cont) ";
    }
    else
    {
        return "taf>> ";
    }
}

void Parser::scanPosition(const char* s)
{
    string line(s);
    string::size_type idx;

    idx = line.find("line");
    if(idx != string::npos)
    {
        line.erase(0, idx + 4);
    }

    idx = line.find_first_not_of(" \t\r#");
    if(idx != string::npos)
    {
        line.erase(0, idx);
    }

    _currentLine = atoi(line.c_str()) - 1;

    idx = line.find_first_of(" \t\r");
    if(idx != string::npos)
    {
        line.erase(0, idx);
    }

    idx = line.find_first_not_of(" \t\r\"");
    if(idx != string::npos)
    {
        line.erase(0, idx);

        idx = line.find_first_of(" \t\r\"");
        if(idx != string::npos)
        {
            _currentFile = line.substr(0, idx);
            line.erase(0, idx + 1);
        }
        else
        {
            _currentFile = line;
        }
    }
}

void Parser::invalidCommand(const char* s)
{
    error(string(s) + "\n(`help' for more info)");
}

void Parser::invalidCommand(const string& s)
{
    cout << "##�������" << endl;
    invalidCommand(s.c_str());
}

void Parser::error(const char* s)
{
    if(_commands.empty() && !isatty(fileno(yyin)))
    {
        cerr << _currentFile << ':' << _currentLine << ": " << s << endl;
    }
    else
    {
        cerr << "error: " << s << endl;
    }
    _errors++;
}

void Parser::error(const string& s)
{
    error(s.c_str());
}

void Parser::warning(const char* s)
{
    if(_commands.empty() && !isatty(fileno(yyin)))
    {
        cerr << _currentFile << ':' << _currentLine << ": warning: " << s << endl;
    }
    else
    {
        cerr << "warning: " << s << endl;
    }
}

void Parser::warning(const string& s)
{
    warning(s.c_str());
}

int Parser::parse(FILE* file, bool debug)
{
    yydebug = debug ? 1 : 0;

    assert(!parser);
    parser = this;

    _errors = 0;
    _commands.empty();
    yyin = file;
    assert(yyin);

    _currentFile = "";
    _currentLine = 0;
    _continue = false;
    nextLine();

    int status = yyparse();
    if(_errors)
    {
        status = EXIT_FAILURE;
    }

    parser = 0;
    return status;
}

vector<string> Parser::serverIdParse(const string & sServerId)
{
    //�����ֽ�
    //[0] [1]    [2]
    //APP.SERVER_NODE

    string::size_type uMark1 = sServerId.find(".");
    string::size_type uMark2 = sServerId.find("_");

    if(uMark1 == string::npos || uMark2 == string::npos || uMark1 - uMark2 <= 1)
    {
        throw TafException("server id ��ʽ����ӦΪ: APP.SERVER_NODE");
    }

    vector<string> args;
    args.push_back(sServerId.substr(0, uMark1));
    args.push_back(sServerId.substr(uMark1+1, uMark2-uMark1-1));
    args.push_back(sServerId.substr(uMark2+1));

    return args;
}





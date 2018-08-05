#include "servant/ServantProxy.h"
#include "servant/ServantHandle.h"
#include "servant/StatReport.h"
#include "servant/Application.h"
#include "servant/BaseF.h"
#include "log/taf_logger.h"

namespace taf
{

////////////////////////////////////////////////////////////////////////////

int32_t ThreadPrivateData::INVALID_HASH_CODE = -1;

int32_t ThreadPrivateData::INVALID_GRID_CODE = -1;

////////////////////////////////////////////////////////////////////////////

pthread_key_t ServantProxyThreadData::g_key = 0; 

TC_ThreadMutex ServantProxyThreadData::mutex;

void ThreadPrivateData::set(const string &name,const string &value)
{
    TC_LockT<TC_ThreadMutex> lock(_mutex);

    _dataMap[name] = value;
}

bool ThreadPrivateData::get(const string &name,string &value) const
{
    TC_LockT<TC_ThreadMutex> lock(_mutex);

    map<string,string>::const_iterator it = _dataMap.find(name);
    if(it == _dataMap.end())
    {
        return false;
    }
    value = it->second;
    return true;
}

void ServantProxyThreadData::destructor(void* p)
{
    ServantProxyThreadData* td = (ServantProxyThreadData*)p;

    if (td)
    {
        delete td;

        td = NULL;
    }
}

ServantProxyThreadData* ServantProxyThreadData::getData()
{
    if(ServantProxyThreadData::g_key == 0)
    {
        TC_LockT<TC_ThreadMutex> lock(ServantProxyThreadData::mutex);

        //g_key Ϊ0��ʾδ��ʼ������˿��ܻ��˷ѵ���һ���߳�����key
        if(ServantProxyThreadData::g_key == 0)
        {
            int ret = ::pthread_key_create(&ServantProxyThreadData::g_key, ServantProxyThreadData::destructor);

            if (ret != 0)
            {
                LOG->error() << "[TAF][ServantProxyThreadData pthread_key_create fail:" << errno << ":" << strerror(errno) << "]" << endl;

                return NULL;
            }
        }
    }
    ServantProxyThreadData* td = (ServantProxyThreadData*)TC_ThreadPool::getThreadData(ServantProxyThreadData::g_key);

    if (td == NULL)
    {
        TC_LockT<TC_ThreadMutex> lock(ServantProxyThreadData::mutex);

        if(td == NULL)
        {
            td              = new ServantProxyThreadData();
            td->_dataPtr    = new ThreadPrivateData();

            TC_ThreadPool::setThreadData(ServantProxyThreadData::g_key, td);
        }
    }
    return td;
}

ServantProxyThreadData::ServantProxyThreadData() 
{
}

const ThreadPrivateDataPtr & ServantProxyThreadData::data()
{
    return _dataPtr;
}

void ServantProxyThreadData::data(const ThreadPrivateDataPtr & dataPtr)
{
    _dataPtr = dataPtr;
}

void ServantProxyThreadData::setPrivatePointer(void *p)
{
    _p =  p;
}

void* ServantProxyThreadData::getPrivatePointer()
{
    return _p;
}

///////////////////////////////////////////////////////////////////////////////
// 
string ServantProxy::STATUS_GRID_KEY   = "STATUS_GRID_KEY";
string ServantProxy::STATUS_DYED_KEY   = "STATUS_DYED_KEY";
string ServantProxy::STATUS_GRID_CODE  = "STATUS_GRID_CODE";
string ServantProxy::STATUS_SAMPLE_KEY = "STATUS_SAMPLE_KEY";
string ServantProxy::STATUS_RESULT_CODE = "STATUS_RESULT_CODE";
string ServantProxy::STATUS_RESULT_DESC = "STATUS_RESULT_DESC";

ServantProxy::ServantProxy()
: BaseProxy(NULL)
, _objectProxy(NULL)
, _router(NULL)
, _timeout(DEFAULT_TIMEOUT)
{
}

ServantProxy::ServantProxy(Communicator* comm, ObjectProxy* op)
: BaseProxy(comm)
, _objectProxy(op)
, _router(NULL)
, _timeout(DEFAULT_TIMEOUT)
{
}

ServantProxy::~ServantProxy()
{
}

void ServantProxy::taf_max_trans_num(int num)
{
    if (_objectProxy)
    {
        _objectProxy->setAdapterMaxTransNum(num);
    }
}

int ServantProxy::taf_max_trans_num() const
{
    if (_objectProxy)
    {
        return _objectProxy->getAdapterMaxTransNum();
    }
    return 0;
}

string ServantProxy::taf_name() const
{
    if (_objectProxy)
    {
        return _objectProxy->name();
    }
    return "NULL";
}

TC_Endpoint ServantProxy::taf_invoke_endpoint()
{
    std::string sValue;
    ServantProxyThreadData* td = ServantProxyThreadData::getData();
    if (td)
    {
        td->data()->get(string("SERVER_HOST_PORT"), sValue);

        return TC_Endpoint(sValue);
    }

    return TC_Endpoint();
}

vector<TC_Endpoint> ServantProxy::getEndpoint()
{
    if (_objectProxy)
    {
        return _objectProxy->getEndpoint();
    }
    return vector<TC_Endpoint>();
}

vector<TC_Endpoint> ServantProxy::getEndpoint4All()
{
    if (_objectProxy)
    {
        return _objectProxy->getEndpoint4All();
    }
    return vector<TC_Endpoint>();
}

vector<TC_Endpoint> ServantProxy::taf_endpoints(const std::string & sStation)
{
    if (_objectProxy)
    {
        vector<TC_Endpoint> vecActive;
        vector<TC_Endpoint> vecInactive;
        _objectProxy->getEndpoint4All(sStation, vecActive, vecInactive);

        return vecActive;
    }
    return vector<TC_Endpoint>();
}

void ServantProxy::taf_endpoints(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive)
{
    if (_objectProxy)
    {
        _objectProxy->getEndpoint4All(sStation, vecActive, vecInactive);
    }
}

void ServantProxy::taf_timeout(int msecond)
{
    _timeout = msecond;

    if (_timeout > _objectProxy->timeout())
    {
        _timeout = _objectProxy->timeout();
    }
}

int ServantProxy::taf_timeout() const
{
    return _timeout;
}

void ServantProxy::taf_set_protocol(const ProxyProtocol& protocol)
{
    _objectProxy->setProxyProtocol(protocol);
}

CheckTimeoutInfo& ServantProxy::taf_check_timeout()
{
    return _objectProxy->checkTimeoutInfo();
}

void ServantProxy::taf_ping()
{
    vector<char> v;

    map<string, string> m;

    map<string, string> s;

    ResponsePacket rsp;

    taf_invoke(taf::JCENORMAL, "taf_ping", v, m, s, rsp);
}

void ServantProxy::taf_clear_hash()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    if (td)
    {
        td->data()->_hashCode = ThreadPrivateData::INVALID_HASH_CODE;
    }
}

ServantProxy* ServantProxy::taf_hash(int64_t key)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    if (td)
    {
        td->data()->_hashCode = key;
    }
    return this;
}

void ServantProxy::taf_set_router(const ServantProxyRouterPtr& router)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    _router = router;
}

void ServantProxy::taf_del_router()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    if (_router)
    {
        _router = NULL;
    }
}

uint32_t ServantProxy::taf_gen_requestid()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    return _objectProxy->getTimeoutQueue()->generateId();
}

void ServantProxy::taf_set_push_callback(const ServantProxyCallbackPtr& cb)
{
    _objectProxy->setPushCallbacks(cb);
}

int ServantProxy::invoke(ReqMessagePtr& req)
{    
    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    if(td && req->callback)
    {
        //��ǰ˽�����ݱ����ڻص�������
        req->callback->_dataPtr = td->data();
    }

    //�������ҪHASH����Ϣ
    if (td && td->data()->_hashCode != ThreadPrivateData::INVALID_HASH_CODE)
    {
        req->hashCode = td->data()->_hashCode;

        SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEHASH);

        LOG->info() << "[TAF][invoke, " << _objectProxy->name() << ", hash:" << td->data()->_hashCode << "]" << endl;
    }

    //�������ҪȾɫ����Ϣ
    if (td && td->data()->_bDyeing)
    {
        SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEDYED);

        LOG->info() << "[TAF][" << __FILE__ << "|" << __LINE__ << "]set dyeing" << endl;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    map<string, string>::iterator keyIt = req->request.status.find(ServantProxy::STATUS_GRID_KEY);

    //�������Ҫ�Ҷ�·�ɵĽӿ�
    if (keyIt != req->request.status.end())
    {
        string routeKey = keyIt->second;

        //���ҵ��������·����������ѡʹ��·����
        int32_t grid = ThreadPrivateData::INVALID_GRID_CODE;

        if (_router)
        {
            grid = _router->getGridByKey(routeKey);
        }

        //û��·��������·������getGridByKey������Ч״̬(-1)
        if (grid == ThreadPrivateData::INVALID_GRID_CODE)
        {
            //���֮ǰ�ĵ�����·��״̬(��server�������߳�������)
            //����ͬһ���û�(key��ͬ),�򴫵ݻҶ�״̬
            if (td && td->data()->_routeKey == routeKey && td->data()->_gridCode != ThreadPrivateData::INVALID_GRID_CODE)
            {
                grid = td->data()->_gridCode;
            }
        }

        if (grid != ThreadPrivateData::INVALID_GRID_CODE)
        {
            req->request.status[ServantProxy::STATUS_GRID_CODE] = TC_Common::tostr(grid);

            SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID);

            LOG->info() << "[TAF][invoke, " << _objectProxy->name() << ", grid:" << routeKey << "->" << grid << "]" << endl;
        }

        if(td && td->data()->_dyeingKey != "") 
        {
            LOG->info() << "[TAF][dyeing message, " << td->data()->_dyeingKey << "]" << endl;

            req->request.status[ServantProxy::STATUS_DYED_KEY] = td->data()->_dyeingKey;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////
    if (req->type == ReqMessage::SYNC_CALL)
    {
        req->monitor = new ReqMonitor;
    }

    //gettimeofday(&req->begtime, NULL);
    TC_TimeProvider::getInstance()->getNow(&req->begtime);

    int ret = _objectProxy->invoke(req);

    string servantFunc = string("servant:") + req->request.sServantName + ",func:" + req->request.sFuncName;

    //�����ڿͻ��˾�ʧ����(�׳��쳣)
    if (ret < 0)
    {
        //��û�н���TimeoutQueue�������㳬ʱ(proxy==NULL��������Message.cpp����ʱ��Ϊ��ʱ��Ϣ����)
        req->proxy = NULL;

        ostringstream os;

        if(ret == -2)
        {
            os << "[invoke fail,no valid servant," << servantFunc << "]";
        }
        else if(ret == -1)
        {
            os << "[invoke connect fail," << servantFunc << "]";

            if(req->adapter) os << "adapter:" << req->endpoint.desc();
        }
        else
        {
            os << "[invoke unknown fail," << servantFunc << "]";
        }

        //�ϱ��쳣
        reportToStat(req, _objectProxy->name(), StatReport::STAT_EXCE);

        throw TafException(os.str());
    }

    if (req->type == ReqMessage::SYNC_CALL)
    {
        TC_ThreadLock::Lock lock(*(req->monitor));

        //�����û�з����ҵȴ���ʱ
        if (req->response.iVersion == -1 && !req->monitor->timedWait(_timeout))
        {
            //�ϱ���ʱͳ��
            reportToStat(req, _objectProxy->name(), StatReport::STAT_TIMEOUT);

            ostringstream os;

            os << "[invoke timeout(" << _timeout << ")," << servantFunc;
                
            if(req->adapter) os << ",adapter:" << req->adapter->endpoint().desc();

            os << ",reqid:" << req->request.iRequestId << "]";

            throw TafSyncCallTimeoutException(os.str());
        }

        //�ϱ��ɹ�ͳ��
        if (req->response.iRet == JCESERVERSUCCESS)
        {
            if (td)
            {
                ostringstream os; 
                os << (req->endpoint.type() == EndpointInfo::TCP?"tcp":(req->endpoint.type() == EndpointInfo::UDP?"udp":""));
                os << " -h " << req->endpoint.host();
                os << " -p " << req->endpoint.port();
                os << " -g " << req->endpoint.grid();
                os << " -t " << "0";

                td->data()->set("SERVER_HOST_PORT", os.str());
            }

            reportToStat(req, _objectProxy->name(), StatReport::STAT_SUCC);
            return true;
        }

        //�ϱ��쳣ͳ��
        reportToStat(req, _objectProxy->name(), StatReport::STAT_EXCE);

        //���ݷ���˷���ֵ�׳��쳣
        TafException::throwException(req->response.iRet, servantFunc + ",exception info:" + req->response.sResultDesc);
    }
    return false;
}

//ͬ�����÷��أ����ѵȴ���ҵ���߳�
int ServantProxy::finished(ReqMessagePtr& req)
{
    if (req->monitor)
    {
        TC_ThreadLock::Lock sync(*(req->monitor));

        req->monitor->notify();

        return 0;
    }
    return -1;
}

//////////////////////////////////////////////////////////////////
void ServantProxy::taf_invoke_async(char cPacketType, 
                                    const string &sFuncName, 
                                    const vector<char>& buf, 
                                    const map<string, string>& context, 
                                    const map<string, string>& status, 
                                    const ServantProxyCallbackPtr& callback)
{
    try
    {
        ReqMessagePtr req = new ReqMessage;

        req->proxy                  = this;
        req->fromRpc                = false;
        req->request.iRequestId     = 0;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = (callback ? cPacketType : JCEONEWAY);
        req->request.sServantName   = _objectProxy->name();
        req->request.sFuncName      = sFuncName;
        req->request.sBuffer        = buf;
        req->request.context        = context;
        req->request.status         = status;
        req->request.iTimeout       = _timeout;
        req->type                   = (callback ? ReqMessage::ASYNC_CALL : ReqMessage::ONE_WAY);
        req->callback               = callback;
        req->response.iVersion      = -1; //ʹ�ø��ֶ��ж��Ƿ���response����
        req->response.iRet          = JCESERVERUNKNOWNERR;

        this->invoke(req);
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::_invokeAsync exception.");
    }
}

void ServantProxy::taf_invoke(char cPacketType, 
                              const string& sFuncName, 
                              const vector<char>& buf, 
                              const map<string, string>& context, 
                              const map<string, string>& status, 
                              ResponsePacket& rsp)
{
    ReqMessagePtr req = new ReqMessage;

    try
    {
        req->proxy                  = this;
        req->fromRpc                = false;
        req->request.iRequestId     = 0;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = cPacketType;
        req->request.sServantName   = _objectProxy->name();
        req->request.sFuncName      = sFuncName;
        req->request.sBuffer        = buf;
        req->request.context        = context;
        req->request.status         = status;
        req->request.iTimeout       = _timeout;
        req->type                   = ReqMessage::SYNC_CALL;
        req->response.iVersion      = -1; //ʹ�ø��ֶ��ж��Ƿ���response����
        req->response.iRet          = JCESERVERUNKNOWNERR;

        this->invoke(req);

        rsp = req->response;
    }
    catch (TafSyncCallTimeoutException &e)
    {
        if(req->adapter)
        {
            req->adapter->finishInvoke(true);
        }
        throw e;
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::_invoke exception.");
    }

    if(req->adapter)
    {
        req->adapter->finishInvoke(false);
    }
}

void ServantProxy::rpc_call(uint32_t requestId, const string& sFuncName, 
                            const char* buff, uint32_t len, 
                            ResponsePacket& rsp)
{
    ReqMessagePtr req = new ReqMessage;

    try
    {
        req->proxy                  = this;
        req->fromRpc                = true;
        req->request.iRequestId     = requestId;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = JCENORMAL;
        req->request.sServantName   = _objectProxy->name();
        req->request.sFuncName      = sFuncName;
        req->request.iTimeout       = _timeout;
        req->type                   = ReqMessage::SYNC_CALL;
        req->response.iVersion      = -1; //ʹ�ø��ֶ��ж��Ƿ���response����
        req->response.iRet          = JCESERVERUNKNOWNERR;

        req->request.sBuffer.resize(len);
        ::memcpy((taf::Char*)&req->request.sBuffer[0], buff, len);

        this->invoke(req);

        rsp = req->response;
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::_invoke exception.");
    }

    if(req->adapter)
    {
        req->adapter->finishInvoke(false);
    }
}

void ServantProxy::rpc_call_async(uint32_t requestId, const string& sFuncName, 
                                  const char* buff, uint32_t len, 
                                  const ServantProxyCallbackPtr& callback)
{
    ReqMessagePtr req = new ReqMessage;

    try
    {
        req->proxy                  = this;
        req->fromRpc                = true;
        req->request.iRequestId     = requestId;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = (callback ? JCENORMAL : JCEONEWAY);
        req->request.sServantName   = _objectProxy->name();
        req->request.sFuncName      = sFuncName;
        req->request.iTimeout       = _timeout;
        req->type                   = (callback ? ReqMessage::ASYNC_CALL : ReqMessage::ONE_WAY);
        req->callback               = callback;
        req->response.iVersion      = -1; //ʹ�ø��ֶ��ж��Ƿ���response����
        req->response.iRet          = JCESERVERUNKNOWNERR;

        req->request.sBuffer.resize(len);
        ::memcpy((taf::Char*)&req->request.sBuffer[0], buff, len);

        this->invoke(req);
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::_invokeAsync exception.");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
}

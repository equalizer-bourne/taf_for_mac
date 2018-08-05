#include "servant/AdapterProxy.h"
#include "servant/StatReport.h"
#include "servant/Application.h"
#include "log/taf_logger.h"

namespace taf
{
AdapterProxy::AdapterProxy(Communicator* comm, 
                           const EndpointInfo &ep, 
                           ObjectProxy* op, 
                           FDReactor* rt, 
                           TransceiverHandle* transHandle)
: BaseProxy(comm)
, _endpoint(ep)
, _objectProxy(op)
, _reactor(rt)
, _transHandle(transHandle)
, _timeoutQueue(NULL)
, _checkTransInterval(10)
, _lastCheckTransTime(0)
, _currentGridGroup(0)
, _connectTimeout(1000)
, _timeoutInvoke(0)
, _totalInvoke(0)
, _frequenceFailInvoke(0)
, _lastFinishInvokeTime(TC_TimeProvider::getInstance()->getNow())
, _lastRetryTime(0)
, _activeStatus(true)
{
   _currentGridGroup = ep.grid();

   _connectTimeout = TC_Common::strto<int32_t>(_comm->getProperty("connect-timeout", "1000"));

   if (_connectTimeout < 1000)
   {
       _connectTimeout = 1000;
   }

   _timeoutQueue = new TC_TimeoutQueue<ReqMessagePtr>(_objectProxy->timeout()); 

   _comm->getTimeoutHandle()->registerHandle(_timeoutQueue);
}

AdapterProxy::~AdapterProxy()
{
    _trans.clear();

    if (_timeoutQueue != NULL && _comm->getTimeoutHandle())
    {
        _comm->getTimeoutHandle()->unregisterHandle(_timeoutQueue);
    }

    delete _timeoutQueue;

    _timeoutQueue = NULL;
}

const EndpointInfo& AdapterProxy::endpoint()
{
    return _endpoint;
}

void AdapterProxy::setActive(bool value)
{
    _activeStatus = value;
}

TransceiverPtr AdapterProxy::doReconnect()
{
    try
    {
        TransceiverPtr t = Connector::connect(_objectProxy, _endpoint, _connectTimeout);

        LOG->info() << "[TAF][doReconnect," << _objectProxy->name() << ",connect succ:" << _endpoint.desc() << ",fd:" << t->fd() << "]" << endl;

        _transHandle->registerProxy(t->fd(), t, this);

        _reactor->registerHandle(t->fd(), EPOLLIN|EPOLLOUT, _transHandle);

        _trans.push_back(t);

        return t;
    }
    catch (TafException& e)
    {
        LOG->error() << "[TAF][doReconnect," << _objectProxy->name() << ",connect fail:" << _endpoint.desc() << ",ex:" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][doReconnect," << _objectProxy->name() << ",connect fail,ep:" << _endpoint.desc() << "]" << endl;
    }
    return NULL;
}

/**
 * Adapter�Լ��������·��״̬����Ϣ
 */
bool AdapterProxy::popRequest(ReqMessagePtr& req)
{
    if (_timeoutQueue)
    {
        ReqMessagePtr ptr = _timeoutQueue->pop();

        if (ptr)
        {
            req = ptr;

            return true;
        }
    }
    return false;
}

/**
 * ��Transceiver��Ӧ��EpollOut����ã���ȡ�����͵���Ϣ
 */
bool AdapterProxy::sendRequest(TransceiverPtr& trans)
{
    int len =  0;

    ReqMessagePtr req;

    //Adapter�Լ��������״̬����Ϣ����
    while (this->popRequest(req) || _objectProxy->popRequest(req))
    {       
        req->endpoint = _endpoint;

        //�����������ݵ�adapter
        req->adapter  = this;

        string s = "";

        _objectProxy->getProxyProtocol().requestFunc(req->request, s);

        //ͬ�����ã��������һ������
        //�����ӳ����쳣ʱ�����Լ�ʱ���ѵȴ���ҵ���߳�
        if (req->type == ReqMessage::SYNC_CALL)
        {
            trans->setLastReq(req);
        }

        trans->writeToSendBuffer(s);

        LOG->info() << "[TAF][sendRequest, " << _objectProxy->name() << ", " << _endpoint.desc() << ", id:" << req->request.iRequestId << ", len:" << s.length() << "]" << endl;
        
        len += s.length();
        //�ϲ�һ�η��͵İ� ���ϲ���8k ����첽ʱ�ͻ���Ч��
        if (trans->getEndpointType() == EndpointInfo::UDP || len > 8192)
        {
            break;
        }
    }
    return len>0;
}

int AdapterProxy::invoke(ReqMessagePtr& req)
{
    //ѡ��һ������
    TransceiverPtr trans = selectTransceiver();

    if (!trans)
    {
        LOG->error() << "[TAF][invoke," << _objectProxy->name() << ",select trans and retry fail,ep:" << _endpoint.desc() << "]" << endl;

        return -1;
    }
    
    if (req->fromRpc == false)
    {
        req->request.iRequestId = _objectProxy->getTimeoutQueue()->generateId();
    }

    //1.���û��·��״̬�������ObjectProxy������
    //2.�����HASH����·��״̬�������Adapter�Լ��Ķ�����
    //3.ע�⣬ʹ�õĶ���ObjectProxy��TimeoutQueue������id
    if (IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEHASH)
        || IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        _timeoutQueue->push(req, req->request.iRequestId);
    }
    else
    {
        _objectProxy->getTimeoutQueue()->push(req, req->request.iRequestId);
    }

    //��Ȼ����ط�ѡ�������ӣ�����ʵ���ϲ�һ����������ӷ��͵�����
   // _transHandle->handle(trans->fd(), TransceiverHandle::W);
    _transHandle->handleWrite(trans->fd());

    //����ͳ��
    sampleToStat(req, _objectProxy->name(), _endpoint.host());

    return 0;
}

bool AdapterProxy::dispatchPushCallback(ReqMessagePtr& req)
{
    ServantProxyCallback* cb = _objectProxy->getOneRandomPushCallback();

    if (cb)
    {
        req->adapter = this;

        cb->onDispatch(req);

        return true;
    }
    return false;
}

int AdapterProxy::finished(ReqMessagePtr& req)
{
    //����Ҷ�·��ѡȡ�ķ����״̬��ƥ�䣬�Է����Ϊ׼
    //reset,��Adapter������group��ת�Ƶ�����˷��ص�group��
    //���޸ı���Adapter::Endpoint��grid����
    if (req->response.iRet == JCESERVERRESETGRID)
    {
        const map<string, string>& respStatus = req->response.status;

        map<string, string>::const_iterator sit = respStatus.find(ServantProxy::STATUS_GRID_CODE);

        if (sit != respStatus.end())
        {
            int32_t newGrid = TC_Common::strto<int32_t>(sit->second);

            LOG->info() << "[TAF][finished," << _objectProxy->name() << ",server reset grid msg:" << _currentGridGroup << "->" << newGrid << "]" << endl;

            if (_objectProxy->resetAdapterGrid(_currentGridGroup, newGrid, this))
            {
                _currentGridGroup = newGrid;
            }
        }
    }

    //��ȡ����ʱ��id�Ͷ�Ӧ��request��
    uint32_t id = req->response.iRequestId;

    ReqMessagePtr ptr = NULL;

    //1.�����·��״̬�����AdapterProxy�Լ��Ķ����в���request
    //2.���û��·��״̬�����ObjectProxy�����в���request
    if (IS_MSG_TYPE(req->response.iMessageType, taf::JCEMESSAGETYPEHASH)
        || IS_MSG_TYPE(req->response.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        if (!_timeoutQueue)
        {
            LOG->error() << "[TAF][finished," << _objectProxy->name() << ",adapter proxy's timeoutQueue is NULL]" << endl;

            return -1;
        }
        ptr = _timeoutQueue->get(id);
    }
    else
    {
        ptr = _objectProxy->getTimeoutQueue()->get(id);

        //����ǵ�����Э��������Ӧ���ᶪʧHASH״̬
        //����Ҫ���Դ�AdpaterProxy�Ķ�������getһ��
        if (!ptr && _timeoutQueue)
        {
            ptr = _timeoutQueue->get(id);
        }
    }

    if (!ptr)
    {
        if (!dispatchPushCallback(req))
        {
            LOG->error() << "[TAF][finished," << _objectProxy->name() << ",get req-ptr from timeoutqueue ret NULL,may be timeout,id:" << id << "]" << endl;
        }
        return -1;
    }

    ptr->response = req->response;

    //ͬ�����ã�����ServantProxy�߳�
    if (ptr->type == ReqMessage::SYNC_CALL)
    {
        return ptr->proxy->finished(ptr);
    }

    //�첽�ص�������ص������߳���
    if (ptr->callback)
    {
        _comm->asyncProcThread()->put(ptr);

        return 0;
    }

    LOG->error() << "[TAF][finished," << _objectProxy->name() << ",adapter proxy finish fail,id:" << id << ",ret:" << req->response.iRet << "]" << endl;

    return -1;
}

void AdapterProxy::refreshTransceiver()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    //ˢ�����е�����״̬��ɾ����Ч������
    //��TranscieverHandle�յ������쳣ʱ����
    vector<TransceiverPtr>::iterator it = _trans.begin();

    while (it != _trans.end())
    {
        (*it)->isValid() ? ++it : _trans.erase(it);
    }
}

TransceiverPtr AdapterProxy::selectTransceiver()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    //���ѡȡһ������, ���ĳ�����Ӳ����ã����vector��ɾ��
    if (!_trans.empty())
    {
        uint32_t rnum = rand() % _trans.size();

        vector<TransceiverPtr>::iterator it = _trans.begin() + rnum;

        while (it != _trans.end())
        {
            if ((*it)->isValid()) { return *it; }

            _trans.erase(it);
        }

        it = _trans.begin();

        while (rnum-- >= 0 && it != _trans.end())
        {
            if ((*it)->isValid()) { return *it; }

            _trans.erase(it);
        }
    }
    return doReconnect();
}

void AdapterProxy::finishInvoke(bool bTimeout)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    time_t now = TC_TimeProvider::getInstance()->getNow();

    _frequenceFailInvoke = (bTimeout? (_frequenceFailInvoke + 1) : 0);
        
    //�����쳣״̬�����Գɹ�,�ָ�����״̬
    if (!_activeStatus && !bTimeout)
    {
        _activeStatus = true;

        _lastFinishInvokeTime = now;

        _frequenceFailInvoke = 0;

        _totalInvoke = 1;

        _timeoutInvoke = 0;

        //�������� ���ε�
        //TAF_NOTIFY_ERROR(_endpoint.desc() + ": enabled");

        LOG->info() << "[TAF][finishInvoke, " << _objectProxy->name() << ", " << _endpoint.desc() << ", retry ok]" << endl;

        return;
    }

    //�����쳣״̬������ʧ��
    if (!_activeStatus && (now - _lastFinishInvokeTime >= _lastRetryTime))
    {
        LOG->info() << "[TAF][finishInvoke, " << _objectProxy->name() << ", " << _endpoint.desc() << ", retry fail]" << endl;

        return;
    }

    ++_totalInvoke;

    if (bTimeout) 
    { 
        ++_timeoutInvoke; 
    }

    CheckTimeoutInfo& info = _objectProxy->checkTimeoutInfo();

    //�ж��Ƿ���뵽��һ������ʱ��Ƭ  
    uint32_t interval = uint32_t(now - _lastFinishInvokeTime);
    if (interval >= info.checkTimeoutInterval || (_frequenceFailInvoke >= info.frequenceFailInvoke && interval >10)) //���ٴ���1��
    {
        _lastFinishInvokeTime = now;

        //�����Ƿ�ʱ��������������
        if (_timeoutInvoke >= info.minTimeoutInvoke && ((1.0*_timeoutInvoke/_totalInvoke) >= info.radio))
        {
            _activeStatus = false;

            LOG->error() << "[TAF][finishInvoke, " 
                         << _objectProxy->name() << "," << _endpoint.desc() 
                         << ",disable,freqtimeout:" << _frequenceFailInvoke 
                         << ",timeout:"<< _timeoutInvoke 
                         << ",total:" << _totalInvoke << "] " << endl;

            //�������� ���ε�
            //TAF_NOTIFY_ERROR(_endpoint.desc() + ": disabled because of too many timeout.");

            resetInvoke();
        }
        else
        {
            _frequenceFailInvoke = 0;

            _totalInvoke = 0;

            _timeoutInvoke = 0;
        }
    }
}

bool AdapterProxy::checkActive()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    time_t now = TC_TimeProvider::getInstance()->getNow();

    LOG->info() << "[TAF][checkActive," 
                << _objectProxy->name() << "," << _endpoint.desc() << "," 
                << (_activeStatus ? "enable" : "disable") 
                << ",freqtimeout:" << _frequenceFailInvoke 
                << ",timeout:" << _timeoutInvoke 
                << ",total:" << _totalInvoke << "]" << endl;

    //ʧЧ��û�е��´�����ʱ��, ֱ�ӷ��ز�����
    if(!_activeStatus && (now - _lastRetryTime < (int)_objectProxy->checkTimeoutInfo().tryTimeInterval))
    {
        return false;
    }

    _lastRetryTime = now;

    //����Ƿ���Ҫ�����µ�����
    if (_trans.size() < (size_t)_objectProxy->getAdapterMaxTransNum() 
        && (_lastCheckTransTime + _checkTransInterval < now))
    {
        _lastCheckTransTime = now;

        doReconnect();
    }

    return (_trans.size() > 0);
}

void AdapterProxy::resetInvoke()
{
    _lastFinishInvokeTime = TC_TimeProvider::getInstance()->getNow();

    _lastRetryTime = TC_TimeProvider::getInstance()->getNow();

    _frequenceFailInvoke = 0;

    _totalInvoke = 0;

    _timeoutInvoke = 0;

    //����ȫ�����
    vector<TransceiverPtr>::iterator it = _trans.begin();

    while (it != _trans.end())
    {
         (*it)->doClose();

         _trans.erase(it);
    }
}
//////////////////////////////////////////////////////////////////////////////////
}

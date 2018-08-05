#include "servant/ObjectProxy.h"
#include "servant/QueryEndpoint.h"
#include "servant/Global.h"
#include "servant/AppCache.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"
#include "log/taf_logger.h"

namespace taf
{
///////////////////////////////////////////////////////////////////////////////////
AdapterProxyGroup::AdapterProxyGroup() 
: lastRoundPosition(0) 
{
}

AdapterPrx AdapterProxyGroup::getHashProxy(int64_t hashCode)
{
    if (!adapterProxys.empty())
    {
        return adapterProxys[((uint32_t)hashCode % adapterProxys.size())];
    }
    return NULL;
}

AdapterPrx AdapterProxyGroup::getRandomProxy()
{
    if (!adapterProxys.empty())
    {
        return adapterProxys[((uint32_t)rand() % adapterProxys.size())];
    }
    return NULL;
}

AdapterPrx AdapterProxyGroup::getNextValidProxy()
{
    int32_t all = (int32_t)adapterProxys.size();

    while (all-- > 0)
    {
        ++lastRoundPosition;

        lastRoundPosition %= adapterProxys.size();

        if (adapterProxys[lastRoundPosition]->checkActive() == true)
        {
            return adapterProxys[lastRoundPosition];
        }
    }
    return NULL;
}
///////////////////////////////////////////////////////////////////////////////////
ObjectProxy::ObjectProxy(Communicator* comm, const string& name)
: BaseProxy(comm)
, _name(name)
, _timeout(5*1000)
, _maxTransNum(1)
, _isDirectProxy(false)
, _serverHasGrid(false)
, _lastRefreshEndpointTime(0)
, _refreshEndpointInterval(60*1000)
, _lastCacheEndpointTime(0)
, _cacheEndpointInterval(30*60*1000)
, _roundStartIndex(0)
, _timeoutQueue(NULL)
, _queryEndpoint(NULL)
, _lastRefreshEndpoint4AllTime(0)
{
    _proxyProtocol.requestFunc = ProxyProtocol::tafRequest;

    _proxyProtocol.responseFunc = ProxyProtocol::tafResponse;
}

ObjectProxy::~ObjectProxy()
{
    if (_timeoutQueue != NULL && _comm->getTimeoutHandle())
    {
        _comm->getTimeoutHandle()->unregisterHandle(_timeoutQueue);
    }
    delete _timeoutQueue;

    _timeoutQueue = NULL;
/*
    if( _queryEndpoint != NULL)
    {
        delete _queryEndpoint;
    }
    _queryEndpoint = NULL;
    */
}

void ObjectProxy::initialize()
{
    int size = TC_Common::strto<int>(_comm->getProperty("timeout-queue-size", "50000"));

    _timeoutQueue = new TC_TimeoutQueue<ReqMessagePtr>(_timeout,size); //Ĭ��hash_map size����Ϊ5W

    _comm->getTimeoutHandle()->registerHandle(_timeoutQueue);

    string::size_type n = _name.find_first_of('@');
    
    string endpoints;
    
    string name = _name;

    //[ֱ������]ָ�������IP�Ͷ˿��б�
    if (n != string::npos)
    {
        _name = name.substr(0, n);

        _isDirectProxy = true;

        endpoints = name.substr(n + 1);
    }
    //[�������]ͨ��registry��ѯ����˵�IP�Ͷ˿��б�
    //[�������] ��һ��ʹ��cache
    else
    {          
        _locator = _comm->getProperty("locator");
        
        if (_locator.find_first_not_of('@') == string::npos)
        {
            LOG->error() << "[Locator is not valid:" << _locator << "]" << endl;

            throw TafRegistryException("locator is not valid:" + _locator);
        }
        QueryFPrx prx = _comm->stringToProxy<QueryFPrx>(_locator);

        _queryEndpoint = new taf::QueryEndpoint(prx);

        endpoints = AppCache::getInstance()->get(_name,_locator);        
    }
        
    vector<string> eps = TC_Common::sepstr<string>(endpoints, ":", false);
    
    for (uint32_t i = 0; i < eps.size(); ++i)
    {
        try
        {
            TC_Endpoint ep(eps[i]);

            EndpointInfo::EType type = (ep.isTcp() ? EndpointInfo::TCP : EndpointInfo::UDP);

            EndpointInfo epi(ep.getHost(), ep.getPort(), type, ep.getGrid());

            _activeEndpoints.insert(epi);

            AdapterPrx ap = new AdapterProxy(_comm, epi, this, _comm->fdReactor(), _comm->transceiverHandle());

            _adapterGroups[epi.grid()].adapterProxys.push_back(ap);
        }
        catch (...)
        {
            LOG->error() << "[endpoints parse error:" << name << ":" << eps[i] << "]" << endl;
        }
    }
    _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);
}

int ObjectProxy::loadLocator()
{
    if(_isDirectProxy)
    {
        //ֱ������
        return 0;
    }

    string locator = _comm->getProperty("locator");

    if (locator.find_first_not_of('@') == string::npos)
    {
        LOG->error() << "[Locator is not valid:" << locator << "]" << endl;

        return -1;
    }

    QueryFPrx prx = _comm->stringToProxy<QueryFPrx>(locator);

    ((QueryEndpoint*)_queryEndpoint)->setLocatorPrx(prx);

    return 0;
}

ServantProxyCallback* ObjectProxy::getOneRandomPushCallback()
{
    if (!_pushCallbacks.empty())
    {
        return _pushCallbacks[((uint32_t)rand() % _pushCallbacks.size())].get();
    }
    return NULL;
}

void ObjectProxy::setPushCallbacks(const ServantProxyCallbackPtr& cb)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    _pushCallbacks.push_back(cb);
}

const string& ObjectProxy::name() const
{
    return _name;
}

int ObjectProxy::timeout() const
{
    return _timeout;
}

void ObjectProxy::setAdapterMaxTransNum(int num)
{
    _maxTransNum = num < 1 ? 1 : num;
}

int ObjectProxy::getAdapterMaxTransNum() const
{
    return _maxTransNum;
}

void ObjectProxy::setProxyProtocol(const ProxyProtocol& protocol)
{
    _proxyProtocol = protocol;
}

ProxyProtocol& ObjectProxy::getProxyProtocol()
{
    return _proxyProtocol;
}

CheckTimeoutInfo& ObjectProxy::checkTimeoutInfo()
{
    return _checkTimeoutInfo;
}

void ObjectProxy::timeout(int msec)
{
    //�������첽��ʱʱ�䲻��С��1��
    if (msec >= 1000)
    {
        _timeout = msec;
    }
}

vector<TC_Endpoint> ObjectProxy::getEndpoint()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    refreshEndpointInfos();

    vector<TC_Endpoint>  v;

    for(set<EndpointInfo>::iterator it = _activeEndpoints.begin(); it != _activeEndpoints.end(); ++it)
    {
       TC_Endpoint ep(it->host(),it->port(),_timeout,it->type() == EndpointInfo::TCP?1:0,it->grid());

       v.push_back(ep);
    }
    return v;
}


vector<TC_Endpoint> ObjectProxy::getEndpoint4All()
{
    //ֱ�� ֱ�ӷ��س�ʼ��ʱ����б�
    if (_isDirectProxy)
    {
        return getEndpoint();
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

     //����Ǽ�����ӣ�ͨ��registry��ѯ�����б�
    if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now) ||
        (_activeEndpoints4All.empty() && (_lastRefreshEndpoint4AllTime + 2 < now))) //2s��������
    {
        _lastRefreshEndpoint4AllTime = now;

        set<EndpointInfo>  activeEndpoints = ((QueryEndpoint*)_queryEndpoint)->findObjectById(_name);
        if (!activeEndpoints.empty())
        {
            _activeEndpoints4All = activeEndpoints;
        } 
    }
    vector<TC_Endpoint>  v;  
    for(set<EndpointInfo>::iterator it = _activeEndpoints4All.begin(); it != _activeEndpoints4All.end(); ++it)
    {
       TC_Endpoint ep(it->host(),it->port(),_timeout,it->type() == EndpointInfo::TCP?1:0,it->grid());

       v.push_back(ep);
    }
    return v;
}

void ObjectProxy::getEndpoint4All(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive)
{
    vecActive.clear();
    vecInactive.clear();
    //ֱ�� ֱ�ӷ��س�ʼ��ʱ����б�
    if (_isDirectProxy)
    {
        vecActive = getEndpoint();
        return ;
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);
    std::map<std::string, std::pair<std::vector<TC_Endpoint>, std::vector<TC_Endpoint> > >::iterator itEndpoint = _mapEndpoints.find(sStation);
    if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now) || (itEndpoint == _mapEndpoints.end() && (_lastRefreshEndpoint4AllTime + 2 < now)))
    {
        std::set<EndpointInfo> setActive, setInactive;
        ((QueryEndpoint*)_queryEndpoint)->findObjectByStation(_name, sStation, setActive, setInactive);

        for(std::set<EndpointInfo>::iterator it = setActive.begin(); it != setActive.end(); ++it)
        {
            TC_Endpoint ep(it->host(), it->port(), _timeout, it->type() == EndpointInfo::TCP?1:0, it->grid());

            vecActive.push_back(ep);
        }

        for(std::set<EndpointInfo>::iterator it = setInactive.begin(); it != setInactive.end(); ++it)
        {
            TC_Endpoint ep(it->host(), it->port(), _timeout, it->type() == EndpointInfo::TCP?1:0, it->grid());

            vecInactive.push_back(ep);
        }

        _mapEndpoints[sStation].first = vecActive;
        _mapEndpoints[sStation].second= vecInactive;

        return ;
    }

    vecActive   = _mapEndpoints[sStation].first;
    vecInactive = _mapEndpoints[sStation].second;
}

int ObjectProxy::refreshEndpointInterval() const
{
    return _refreshEndpointInterval;
}

void ObjectProxy::refreshEndpointInterval(int msecond)
{
    //������ˢ�·�����б��ʱ�䲻��С��1��
    _refreshEndpointInterval = msecond > 1000 ? msecond : _refreshEndpointInterval;
}

void ObjectProxy::cacheEndpointInterval(int msecond)
{
    //������cache������б��ʱ�䲻��С��60�� Ĭ��30����
    _cacheEndpointInterval = msecond > 1000*60 ? msecond : _cacheEndpointInterval;
}

TC_TimeoutQueue<ReqMessagePtr>* ObjectProxy::getTimeoutQueue()
{
    return _timeoutQueue;
}

int ObjectProxy::invoke(ReqMessagePtr& req)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    //ѡ��һ��Զ�̷����Adapter������
    AdapterPrx adp = selectAdapterProxy(req);

    if (!adp)
    {
        LOG->error() << "[TAF][invoke, " << _name << ", select adapter proxy ret NULL]" << endl;

        return -2;
    }

    //ע������ѡ����adapter,���Ƕ��ڷ�hash�ͷǻҶȵ�����,����һ�������adapter���ͳ�ȥ��!ע��Ŷ
    return adp->invoke(req);
}

//��Ӧ��Ϣ��AdapterProxy���Ѿ�ȫ��������
int ObjectProxy::finished(ReqMessagePtr& req)
{
    return 0;
}

//�ɺ�����AdapterProxyȡ��һ����Ϣ����
bool ObjectProxy::popRequest(ReqMessagePtr& req)
{
    //��FIFO�����л�ȡһ����Ϣ���з���
    ReqMessagePtr ptr = _timeoutQueue->pop();

    if (ptr)
    {
        req = ptr;

        return true;
    }
    return false;
}

//��ʱˢ�·����б�
void ObjectProxy::refreshEndpointInfos()
{
    try
    {
        //�����ֱ���ģ����ó�ʼ��ʱ��ķ����б�
        //��Ϊ����ɾ����Ч��AdapterProxy�����Բ���ˢ��
        if (_isDirectProxy)
        {
            return;
        }

        time_t now = TC_TimeProvider::getInstance()->getNow();

        //����Ǽ�����ӣ�ͨ��registry��ʱ��ѯ�����б�
        if ((_lastRefreshEndpointTime + _refreshEndpointInterval/1000 < now) ||
            (_adapterGroups.empty() && (_lastRefreshEndpointTime + 2 < now))) //2s��������
        {
            _lastRefreshEndpointTime = now;

            //��ʼ��ʱʹ��ͬ�����ã��Ժ����첽ˢ��
            bool sync = (_activeEndpoints.empty() || (_lastCacheEndpointTime == 0));

            ((QueryEndpoint*)_queryEndpoint)->findObjectById4All(_name, sync);
        }

        //�������ж��Ƿ��Ѿ���ˢ��
        if (!((QueryEndpoint*)_queryEndpoint)->hasRefreshed())
        {
            return;
        }

        set<EndpointInfo> activeEps;

        set<EndpointInfo> inactiveEps;

        set<int32_t> allGridCodes;

        //�첽���û�û��callback����
        if (!((QueryEndpoint*)_queryEndpoint)->hasNewEndpoints(activeEps, inactiveEps, allGridCodes))
        {
            LOG->info() << "[TAF][refreshEndpointInfos,`findObjectById4All` hasNewEndpoints false:" << _name << "]" << endl;

            return;
        }

        //���registry����Active�����б�Ϊ�գ���������
        if (activeEps.empty())
        {
            LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` ret activeEps is empty:" << _name << "]" << endl;

            return;
        }

        set<EndpointInfo> del; //��Ҫɾ���ķ����ַ

        set<EndpointInfo> add; //��Ҫ���ӵķ����ַ

        set_difference(_activeEndpoints.begin(), _activeEndpoints.end(), activeEps.begin(), activeEps.end(), inserter(del,del.begin()));

        set_difference(activeEps.begin(), activeEps.end(), _activeEndpoints.begin(), _activeEndpoints.end(), inserter(add,add.begin()));

        //��������ֻ��һ��״̬�������·������
        _serverHasGrid = (allGridCodes.size() > 1 ? true : false);

        //û����Ҫ���µķ���ڵ�
        if (del.empty() && add.empty())
        {
            return;
        }

        LOG->info() << "[TAF][refreshEndpointInfos," << _name << ":`findObjectById4All` add:" << add.size() << ",del:" << del.size() << ",isGrid:" << _serverHasGrid << "]" << endl;

        //ɾ��������Ч�ķ���ڵ�
        for (set<EndpointInfo>::iterator it = del.begin(); it != del.end(); ++it)
        {
            map<int32_t, AdapterProxyGroup>::iterator mit = _adapterGroups.find(it->grid());

            if (mit == _adapterGroups.end())
            {
                continue;
            }
            vector<AdapterPrx>::iterator vit = mit->second.adapterProxys.begin();

            while (vit != mit->second.adapterProxys.end())
            {
                if ((*it) == (*vit)->endpoint())
                {
                    mit->second.adapterProxys.erase(vit);

                    continue;
                }
                ++vit;
            }
        }

        //��ӱ��ػ�û�еĽڵ�
        for (set<EndpointInfo>::iterator it = add.begin(); it != add.end(); ++it)
        {
            AdapterPrx ap = new AdapterProxy(_comm, *it, this, _comm->fdReactor(), _comm->transceiverHandle());

            _adapterGroups[it->grid()].adapterProxys.push_back(ap);
        }

        //�����µ��б����������´�ˢ��ʱʹ��
        _activeEndpoints.clear();

        for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
        {
            for (uint32_t i = 0; i < it->second.adapterProxys.size(); ++i)
            {
                _activeEndpoints.insert(it->second.adapterProxys[i]->endpoint());
            }
        }     
    }
    catch (exception& e)
    {
        LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` exception:" << _name << ":" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` exception:" << _name << "]" << endl;
    }
}

//��ָ����һ��adapter��ѡȡһ����Ч��
AdapterPrx ObjectProxy::selectFromGroup(const ReqMessagePtr& req, AdapterProxyGroup& group)
{
    AdapterPrx prx = NULL;

    //�����hash������ʹ��hash����
    if (req->hashCode != ThreadPrivateData::INVALID_HASH_CODE)
    {
        prx = group.getHashProxy(req->hashCode);

        if (prx && prx->checkActive() == true)
        {
            return prx;
        }
    }
    //û��hash����hashʧ�ܣ�����ѭѡ��һ����Ч��
    prx = group.getNextValidProxy();

    if (!prx)
    {
        //û����Ч�ķ��������ѡȡһ��(����ʱ�᳢������)
        prx = group.getRandomProxy();
    }
    return prx;
}
/******************************************************************
 * �ӿ��õķ����б�ѡ��һ������ڵ�
 */
AdapterPrx ObjectProxy::selectAdapterProxy(const ReqMessagePtr& req)
{
    refreshEndpointInfos();

    //û�п��õķ����б�ֱ�ӷ���NULL��ҵ����յ��쳣
    if (_adapterGroups.empty())
    {
        LOG->error() << "[TAF][selectAdapterProxy," << _name << ",adapter proxy groups is empty!]" << endl;

        return NULL;
    }
    
    time_t now = TC_TimeProvider::getInstance()->getNow();

    //��ֱ������Ҫ��ʱ���»�������б�
    if (!_isDirectProxy && _lastCacheEndpointTime + _cacheEndpointInterval/1000 < now)
    {
        _lastCacheEndpointTime = now;

        string sEndpoints = "";

        for (set<EndpointInfo>::iterator it = _activeEndpoints.begin(); it != _activeEndpoints.end(); ++it)
        {
            bool isTcp = (it->type() == EndpointInfo::TCP ? true : false);

            TC_Endpoint ep(it->host(), it->port(), _timeout, isTcp, it->grid());

            if (sEndpoints != "") sEndpoints += ":";

            sEndpoints += ep.toString();
        }

        AppCache::getInstance()->set(_name,sEndpoints,_locator);

        LOG->info() << "[TAF][setEndPointToCache,obj:" << _name << ",endpoint:" << sEndpoints << "]" << endl;
    }

    int32_t gridCode = 0;

    bool isValidGrid = false;

    string gridKey = "";

    const map<string, string>& status = req->request.status;

    //����ǻҶ�·����Ϣ���������Ч��
    if (IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        map<string, string>::const_iterator keyIt = status.find(ServantProxy::STATUS_GRID_KEY);

        map<string, string>::const_iterator codeIt = status.find(ServantProxy::STATUS_GRID_CODE);

        if (keyIt != status.end() && codeIt != status.end())
        {
            gridCode = TC_Common::strto<int32_t>(codeIt->second);

            if (gridCode != ThreadPrivateData::INVALID_GRID_CODE)
            {
                isValidGrid = true;
            }
        }
    }

    //��Ч��·����Ϣ���ҷ�����ж���״̬
    if (isValidGrid == true && _serverHasGrid == true)
    {
        map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.find(gridCode);

        if (it != _adapterGroups.end())
        {
            return selectFromGroup(req, it->second);
        }

        LOG->error() << "[TAF][selectAdapterProxy," << _name << ",grid router fail,gridKey:" << gridKey << "->gridCode:" << gridCode << "]" << endl;

        return NULL;
    }

    //��·����Ϣ������ֻ��һ��״̬(�п�������reset��Ӧ)
    return selectFromGroup(req, _adapterGroups.begin()->second);
}

/*************************************************************************************
 * �Ҷ�·�ɷ��͵�����ˣ�����˼�����״̬��ƥ�䣬�򷵻�reset��Ϣ����ȷ��״̬
 * �ͻ��˽��÷����adapterת�Ƶ���ȷ��group�����ǲ���ı䱾��adapter��endpoint����
 * ע�⣺�������˼��״̬��ƥ�䣬���Ƿ����״̬Ϊ0���򲻻᷵��reset
 */
bool ObjectProxy::resetAdapterGrid(int32_t gridFrom, int32_t gridTo, AdapterProxy* adapter)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    LOG->info() << "[TAF][resetAdapterGrid," << _name << ",reset adapter grid:" << gridFrom << "->" << gridTo <<":" << adapter->endpoint().desc() << "]" << endl;

    map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.find(gridFrom);

    if (gridFrom == gridTo || it == _adapterGroups.end())
    {
        return false;
    }

    vector<AdapterPrx>& vap = it->second.adapterProxys;

    for (vector<AdapterPrx>::iterator vit = vap.begin(); vit != vap.end(); ++vit)
    {
        if ((*vit)->endpoint() == adapter->endpoint())
        {
            //������ȷ��group
            _adapterGroups[gridTo].adapterProxys.push_back(*vit);

            //�����е�group��ɾ��
            vap.erase(vit);

            _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);

            return true;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////
}

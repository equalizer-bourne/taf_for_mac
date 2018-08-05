#ifndef __TAF_OBJECT_PROXY_H_
#define __TAF_OBJECT_PROXY_H_

#include "servant/Communicator.h"
#include "servant/Message.h"
#include "servant/AdapterProxy.h"
#include "servant/EndpointInfo.h"
#include "servant/EndpointF.h"
#include "servant/BaseProxy.h"
#include "servant/Global.h"

namespace taf
{
class QueryEndpointImp;

/**
 * ���ڱ������˵�һ���б�
 * (������˻Ҷ�״̬���з���)
 */
struct AdapterProxyGroup
{
    AdapterProxyGroup();

    vector<AdapterPrx> adapterProxys;

    int32_t lastRoundPosition;

    AdapterPrx getHashProxy(int64_t hashCode);

    AdapterPrx getRandomProxy();

    AdapterPrx getNextValidProxy();
};
///////////////////////////////////////////////////////////////////
/**
 * ÿ��objectname�ڽ��̿ռ�Ψһ��һ��objectproxy
 * �����շ�����Ϣ����
 */
class ObjectProxy : public BaseProxy
{
public:
    /**
     * ���캯��
     * @param comm
     * @param name
     */
    ObjectProxy(Communicator* comm, const string& name);

    /**
     * ��������
     */
    ~ObjectProxy();

public:
    /**
     * ��������
     * @param req
     * @return int
     */
    int invoke(ReqMessagePtr& req);

    /**
     * �������÷���
     * @param req
     * @return int
     */
    int finished(ReqMessagePtr& req);

public:
    /**
     * ��ʼ����Ϣ���е�
     */
    void initialize();

    /**
     * �Ӷ����л�ȡһ������
     * @param req
     * @return bool
     */
    bool popRequest(ReqMessagePtr& req);

public:
    /**
     * ��ȡobject����
     * @return const string&
     */
    const string& name() const;

    /**
     * ��ȡ��Ϣ���г�ʱ����ʱ�����Ϣ������
     * @return int
     */
    int timeout() const;

    /**
     * ������Ϣ���г�ʱ����ʱ�����Ϣ������
     * @param msec
     */
    void timeout(int msec);

    /**
     * ���õ���̨server��������
     * @param num
     */
    void setAdapterMaxTransNum(int num);

    /**
     * ���õ���̨server��������
     * @return int
     */
    int getAdapterMaxTransNum() const;

    /**
     * ��ʱ���Ի�ȡ������ 
     * @return CheckTimeoutInfo& 
     */
    CheckTimeoutInfo& checkTimeoutInfo();

    /**
     * ����Э�������
     * @return UserProtocol& 
     */
    void setProxyProtocol(const ProxyProtocol& protocol);

    /**
     * ��ȡЭ�������
     * @return ProxyProtocol& 
     */
    ProxyProtocol& getProxyProtocol();

    /**
     * ��ȡ���÷����б� ������÷���,ֻ����ͬ����ķ����ip 
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint();

    /**
     * ��ȡ���з����б�  ��������IDC
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All();

	/**
	 * ��ȡ���з����б� ����ָ�������� 
	 *  @return void
	 */
	void getEndpoint4All(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive);

    /**
     * ��ȡˢ�·����б�ļ��
     * @return int
     */
    int refreshEndpointInterval() const;

    /**
     * ����ˢ�·����б�ļ��
     * @param msecond
     */
    void refreshEndpointInterval(int msecond);
    
     /**
     * ���û�������б�ļ��
     * @param msecond
     */
    void cacheEndpointInterval(int msecond);

    /**
     * ���س�ʱ���ж���
     */
    TC_TimeoutQueue<ReqMessagePtr>* getTimeoutQueue();

    /**
     * ����AdapterProxy�ĻҶ�״̬
     * @param gridFrom
     * @param gridTo
     * @param adapter
     * @return bool
     */
    bool resetAdapterGrid(int32_t gridFrom, int32_t gridTo, AdapterProxy* adapter);

    /**
     * ����locator
     * @param
     * @return AdapterPrx
     */
    int loadLocator();

    /**
     * ����PUSH����Ϣ��callback���� 
     * @param cb 
     */
    void setPushCallbacks(const ServantProxyCallbackPtr& cb);

    /**
     * ��ȡPUSH����Ϣ��callback���� 
     */
    ServantProxyCallback* getOneRandomPushCallback();
  
protected:
    /**
     * ��ʱˢ�·����б���registry��
     */
    void refreshEndpointInfos();

    /**
     * ѡȡһ��AdapterProxy
     * @param req
     * @return AdapterPrx
     */
    AdapterPrx selectAdapterProxy(const ReqMessagePtr& req);

    /**
     * ��ѯѡȡ
     * @param adapters
     * @return AdapterPrx
     */
    AdapterPrx selectFromGroup(const ReqMessagePtr& req, AdapterProxyGroup& group);

private:
    string _name;
    
    string _locator;

    int32_t _timeout;

    int32_t _maxTransNum;

    string _queryFObjectName;

    bool _isDirectProxy;

    bool _serverHasGrid;

    time_t _lastRefreshEndpointTime;

    int32_t _refreshEndpointInterval;
    
    time_t _lastCacheEndpointTime;
    
    int32_t _cacheEndpointInterval;
     
    int32_t _roundStartIndex;

    TC_TimeoutQueue<ReqMessagePtr>* _timeoutQueue;

    ServantProxyCallback* _queryEndpoint;

    set<EndpointInfo> _activeEndpoints; //���ڵ�

    map<int32_t, AdapterProxyGroup> _adapterGroups;

    CheckTimeoutInfo _checkTimeoutInfo;

    ProxyProtocol _proxyProtocol;

    vector<ServantProxyCallbackPtr> _pushCallbacks;

private:
    int32_t _lastRefreshEndpoint4AllTime;
 
    set<EndpointInfo> _activeEndpoints4All; //���д��ڵ� ��������ͬIDC��

	std::map<std::string, std::pair<std::vector<TC_Endpoint>, std::vector<TC_Endpoint> > > _mapEndpoints;//���治ͬ�����صĽڵ�
};
///////////////////////////////////////////////////////////////////////////////////
}
#endif

#ifndef __TAF_COMMUNICATOR_H_
#define __TAF_COMMUNICATOR_H_

#include "util/tc_thread.h"
#include "util/tc_config.h"
#include "util/tc_singleton.h"
#include "util/tc_timeout_queue.h"
#include "servant/Global.h"
#include "servant/ServantProxy.h"
#include "servant/ServantProxyFactory.h"
#include "servant/ObjectProxyFactory.h"
#include "servant/TransceiverHandle.h"
#include "servant/AsyncProcThread.h"
#include "servant/FDReactor.h"
#include "servant/TMReactor.h"
//#include "servant/TMEventHandle.h"
#include "servant/TimeoutHandle.h"

#define CONFIG_ROOT_PATH "/taf/application/client"

namespace taf
{
////////////////////////////////////////////////////////////////////////
/**
 * �ͻ�������
 */
struct ClientConfig
{
    /**
     * �ͻ���IP��ַ
     */
    static string LocalIp;
    /**
     * �ͻ���ģ������
     */
    static string ModuleName;
};
////////////////////////////////////////////////////////////////////////
/**
 * ͨ����,���ڴ�����ά���ͻ���proxy
 */
class Communicator : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     */
    Communicator();

    /**
     * �������ù���
     * @param conf
     * @param path
     */
    Communicator(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * ����
     * ����ʱ�Զ���������߳�
     */
    ~Communicator();

    /**
     * ���ɴ���
     * @param T
     * @param objectName
     * @return T
     */
    template<class T> T stringToProxy(const string& objectName)
    {
        T prx = NULL;

        stringToProxy<T>(objectName, prx);

        return prx;
    }

    /**
     * ���ɴ���
     * @param T
     * @param objectName
     * @param proxy
     */
    template<class T> void stringToProxy(const string& objectName, T& proxy)
    {
        ServantPrx prx = getServantProxy(objectName);

        proxy = (typename T::element_type*)(prx.get());
    }

public:
    /**
     * ��ȡ����
     * @param name
     * @param dft, ȱʡֵ
     * @return string
     */
    string getProperty(const string& name, const string& dft = "");

    /**
     * ��������
     * @param properties
     */
    void setProperty(const map<string, string>& properties);

    /**
     * ����ĳһ������
     * @param name
     * @param value
     */
    void setProperty(const string& name, const string& value);

    /**
     * ��������
     * @param conf
     * @param path
     */
    void setProperty(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * �ϱ�ͳ��
     * @return StatReport*
     */
    StatReport* getStatReport();

    /**
     * ���¼�������
     */
    int reloadProperty(string & sResult);
    
    /**
     * ��ȡobj��Ӧ����ip port�б�  ������÷���,ֻ����ͬ����ķ����ip 
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint(const string & objName);

    /**
     * ��ȡobj��Ӧ����ip port�б� ��������IDC��
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All(const string & objName);
    
protected:
    /**
     * ��ʼ��
     */
    void initialize();

    /**
     * ����
     */
    void terminate();

    /**
     * �Ƿ�������
     * @return bool
     */
    bool isTerminating();

    /**
     * ��Property��ʼ���ͻ�������
     */
    void initClientConfig();

    /**
     * ��ȡ����������
     * @return ObjectProxyFactoryPtr
     */
    ObjectProxyFactory* objectProxyFactory();

    /**
     * ��ȡ�������������
     * @return ServantProxyFactoryPtr
     */
    ServantProxyFactory* servantProxyFactory();

    /**
     * ���䴦����
     * @return TransceiverHandlePtr
     */
    TransceiverHandle* transceiverHandle();

    /**
     * fd��Ӧ��
     * @return FDReactorPtr
     */
    FDReactor* fdReactor();

    /**
     * ��ȡ�첽�����߳�
     * @return AsyncProcThreadPtr
     */
    AsyncProcThread* asyncProcThread();

    /**
     * ��ȡͨ�ö���
     * @param objectName
     * @return ServantPrx
     */
    ServantPrx getServantProxy(const string& objectName);

    /**
     * ��ȡ��ʱ������
     * @return TimeoutHandle<TC_TimeoutQueue<ReqMessagePtr>>*
     */
    TimeoutHandle<TC_TimeoutQueue<ReqMessagePtr> >* getTimeoutHandle();

    /**
     * ����ڲ���Ҫֱ�ӷ���ͨ��������
     */
    friend class ReqMessage;

    friend class BaseProxy;

    friend class AdapterProxy;

    friend class ServantProxy;

    friend class ObjectProxy;

    friend class ServantProxyFactory;

    friend class ObjectProxyFactory;

protected:
    bool _initialized;

    bool _terminating;

    map<string, string> _properties;

    ObjectProxyFactoryPtr _objectProxyFactory;

    ServantProxyFactoryPtr _servantProxyFactory;

    TransceiverHandlePtr _transceiverHandle;

    TimeoutHandle<TC_TimeoutQueue<ReqMessagePtr> >* _timeoutHandle;

    StatReportPtr _statReportPtr;

    FDReactorPtr _fdReactor;

    TMReactorPtr _tmReactor;

    AsyncProcThreadPtr _asyncProcThread;
};
////////////////////////////////////////////////////////////////////////
}
#endif

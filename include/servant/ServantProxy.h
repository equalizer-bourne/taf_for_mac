#ifndef _TAF_SERVANT_PROXY_H_
#define _TAF_SERVANT_PROXY_H_

#include "servant/Message.h"
#include "servant/BaseProxy.h"
#include "servant/JceCurrent.h"
#include "util/tc_monitor.h"
#include "util/tc_pack.h"
#include "util/tc_autoptr.h"

namespace taf
{

//////////////////////////////////////////////////////////////////////////
//tat������Ϣ(���ڵ�����ʱ�����)
struct SampleKey
{
    bool _root;   //�Ƿ���ڵ� �ڸ��ڵ����Ψһid

    string _unid; //Ψһid  

    int _depth;   //���

    int _width;   //���

    int _parentWidth;  //���ڵ���ֵ
    
    SampleKey() : _root(true), _unid(""), _depth(0), _width(0), _parentWidth(0) {}
};

//////////////////////////////////////////////////////////////////////////
//�̵߳�˽������(����TAFͬ�������첽����, ���������)
struct ThreadPrivateData : virtual public TC_HandleBase
{
    static int INVALID_HASH_CODE;

    static int INVALID_GRID_CODE;

    int64_t   _hashCode;  //·��ʱ��hashֵ

    int32_t   _gridCode;  //�Ҷ�״ֵ̬

    string    _routeKey;  //�Ҷ�·�ɵ�keyֵ

    string    _dyeingKey; //Ⱦɫ��keyֵ
    
    SampleKey _sampleKey; //������Key

	bool	  _bDyeing;	  //��ʶ��ǰ�߳��Ƿ���ҪȾɫ

    ThreadPrivateData() : _hashCode(INVALID_HASH_CODE), _gridCode(INVALID_GRID_CODE),_bDyeing(false) {}

    /**
     * ��������
     * @param data
     */
    void set(const string &name,const string &value);

    /**
     * ��ȡ����
     * @param data
     */
    bool get(const string &name,string &value) const;

private:
	TC_ThreadMutex _mutex;

    map<string,string> _dataMap; 
};

typedef TC_AutoPtr<ThreadPrivateData> ThreadPrivateDataPtr;

//////////////////////////////////////////////////////////////////////////
// �첽�ص�����Ļ���
class ServantProxyCallback : virtual public TC_HandleBase
{
public:
    /**
     * ��������
     */
    virtual ~ServantProxyCallback() {}

    /**
     * ��ȡ����
     * @return const string& 
     */
    virtual const string& getType() { return _type; }

    /**
     * ��������
     * @return const string& 
     */
    virtual void setType(const string& type) { _type = type; }

public:
    /**
     * �첽�ص�����ʵ�ָ÷���������ҵ���߼�����
     * @param msg
     * @return int
     */
    virtual int onDispatch(ReqMessagePtr msg) = 0;

    /**
     * �߳�˽������
     */
    ThreadPrivateDataPtr _dataPtr;   

    /**
     * ͬһ��·���cb��ʱ�������������class����
     */
    string _type;
};

//////////////////////////////////////////////////////////////////////////
//�û����̵߳��첽�ص�����(�����첽ģ��ͬ��)
//////////////////////////////////////////////////////////////////////////
//�߳�˽������
struct ServantProxyThreadData : public TC_ThreadPool::ThreadData
{
    static TC_ThreadMutex mutex;  //ȫ�ֵĻ�����

    static pthread_key_t g_key;   //˽���߳�����key

    /**
     * ������Դ�ͷ�
     * @param p
     */
    static void destructor(void* p);

    /**
     * ��ȡ�߳����ݣ�û�еĻ����Զ�����
     * @return ServantProxyThreadData*
     */
    static ServantProxyThreadData* getData();

    /**
     * �û����߳�ʱ�õ�
     * @return ServantProxyThreadData*
     */
    static ServantProxyThreadData* makeData();

    /**
     * ���캯��
     */
    ServantProxyThreadData();

    /**
     * ��ȡ�߳�˽������
     * @return ThreadPrivateData&
     */
    const ThreadPrivateDataPtr & data();

    /**
     * ��������
     * @param data
     */
    void data(const ThreadPrivateDataPtr & dataPtr);

    /**
     * ҵ����߳�˽������
     * @param p
     */
    void setPrivatePointer(void *p);

    /**
     * ��ȡҵ����߳�˽������
     * @return void*
     */
    void* getPrivatePointer();

protected:
    void                    *_p;                        //ҵ���߳�˽������

    ThreadPrivateDataPtr    _dataPtr;                   //�û�˽������    
};
//////////////////////////////////////////////////////////////////////////
/**
 * ServantProxy��·��������ҵ��ʵ�ֲ�ע�ᵽServantProxy
 * �������л��߼����£�
 * -���������ݻҶ�״̬����
 * -������ݵ�ǰ���û��Ҷ�״̬�ӷ�����ѡ����Ӧ�ķ������������
 * -���ĳ̨��������ʱ����һ��ֵ�� ����̨����������ΪʧЧ״̬���ر��������ӣ� ����ʱ����
 * -���峬ʱ���ж��߼���μ�taf_check_timeout˵��
 */
struct ServantProxyRouter : public TC_HandleBase
{
    /**
     * ��������
     */
    virtual ~ServantProxyRouter() {}

    /**
     * ����key��ȡ�û��ĻҶ�״̬����ҵ��ʵ�֣�
     * ����INVALID_GRID_CODE����Ϊ�޻Ҷ�״̬
     * @param key
     * @return int
     */
    virtual int getGridByKey(const string& key) = 0;
};

//////////////////////////////////////////////////////////////////////////
/**
 * 1:Զ�̶���ı��ش��� 
 * 2:ͬ��servant��һ��ͨ���������ֻ��һ��ʵ��
 * 3:��ֹ���û���Jce�ж���ĺ�������ͻ���ӿ���taf_��ͷ
 */
class ServantProxy : public BaseProxy
{
public:
    /**
     * ͨ��status��������ʱ�õ���ȱʡ�ַ���
     */
    static string STATUS_DYED_KEY;  //��ҪȾɫ���û�ID

    static string STATUS_GRID_KEY;  //��Ҫ·�ɵ��û�ID

    static string STATUS_GRID_CODE; //Ŀ�����ĻҶ�ֵ
    
    static string STATUS_SAMPLE_KEY; //stat ��������Ϣ

    static string STATUS_RESULT_CODE; //�������룬 wupʹ��

    static string STATUS_RESULT_DESC; //����������,wupʹ��

    /**
     * ȱʡ��ͬ�����ó�ʱʱ��
     * ��ʱ�󲻱�֤��Ϣ���ᱻ����˴���
     */
    enum { DEFAULT_TIMEOUT = 3000, };

    /**
     * ���캯��
     * @param op
     */
    ServantProxy(Communicator* comm, ObjectProxy* op);

    /**
     * ��������
     */
    virtual ~ServantProxy();

public:
    /**
     * ���캯��
     */
    ServantProxy();

    /**
     * ���Ͳ�����Ϣ��������
     */
    void taf_ping();

    /**
     * ����ͬ�����ó�ʱʱ�䣬�Ը�proxy�����з�������Ч
     * @param msecond
     */
    void taf_timeout(int msecond);

    /**
     * ��ȡͬ�����ó�ʱʱ�䣬�Ը�proxy�����з�������Ч
     * @return int
     */
    int taf_timeout() const;

    /**
     * ���õ�����Server��������
     * @param num
     */
    void taf_max_trans_num(int num);

    /**
     * ��ȡ������Server��������
     * @return int
     */
    int taf_max_trans_num() const;

    /**
     * ��ȡ������Object����
     * @return string
     */
    string taf_name() const;
  
	/**
	 * ��ȡ���һ�ε��õ�IP��ַ�Ͷ˿� 
	 * @return string 
	 */
	static TC_Endpoint taf_invoke_endpoint();

     /**
     * ��ȡObject���÷����б� ������÷���,ֻ����ͬ����ķ����ip 
     * @return vector<TC_Endpoint>
     * ��ôû�� taf ��ͷ-_-!!!!!!!
     */
    vector<TC_Endpoint> getEndpoint();

    /**
     * ��ȡObject���÷����б� ��������IDC
     * @return vector<TC_Endpoint>
     * ��ôû�� taf ��ͷ-_-!!!!!!!
     */
    vector<TC_Endpoint> getEndpoint4All();

	/**
	 * ��ȡObject���÷����б� ����ָ�������� 
	 *  @return vector<TC_Endpoint>
	 */
	vector<TC_Endpoint> taf_endpoints(const std::string & sStation);

	/**
	 * ��ȡObject���÷����б� ����ָ�������� 
	 *  @return void
	 */
	void taf_endpoints(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive);


    /**
     * �����û��Զ���Э��
     * @param protocol 
     */
    void taf_set_protocol(const ProxyProtocol& protocol);

    /**
     * ��ȡ�����ó�ʱ������
     */
    CheckTimeoutInfo& taf_check_timeout();

    /**
     * hash������Ϊ��֤һ��ʱ����ͬһ��key����Ϣ����
     * ����ͬ�ķ���ˣ����ڷ����б�̬�仯������
     * ���ϸ�֤
     * @param key
     * @return ServantProxy*
     */
    virtual ServantProxy* taf_hash(int64_t key);

    /**
     * �����ǰ��Hash����
     * @param key
     * @return ServantProxy*
     */
    virtual void taf_clear_hash();

    /**
     * ���ö���·����
     * @param router
     */
    virtual void taf_set_router(const ServantProxyRouterPtr& router);

    /**
     * �����ǰ�Ķ���·����
     */
    virtual void taf_del_router();

    /**
     * ��proxy����һ����object�ϵ����к�
     * @return uint32_t 
     */
    virtual uint32_t taf_gen_requestid();

    /**
     * ����PUSH����Ϣ����Ӧcallback
     * @param cb 
     */
    virtual void taf_set_push_callback(const ServantProxyCallbackPtr& cb);

    /**
     * TAFЭ��ͬ����������
     */
    virtual void taf_invoke(char cPacketType, 
                            const string& sFuncName, 
                            const vector<char> &buf, 
                            const map<string, string>& context, 
                            const map<string, string>& status, 
                            ResponsePacket& rep);

    /**
     * TAFЭ���첽��������
     */
    virtual void taf_invoke_async(char cPacketType,
                                  const string& sFuncName, 
                                  const vector<char> &buf, 
                                  const map<string, string>& context, 
                                  const map<string, string>& status, 
                                  const ServantProxyCallbackPtr& callback);

    /**
     * ��ͨЭ��ͬ��Զ�̵���
     */
    virtual void rpc_call(uint32_t requestId, const string& sFuncName, 
                          const char* buff, uint32_t len, ResponsePacket& rsp);

    /**
     * ��ͨЭ���첽����
     */
    virtual void rpc_call_async(uint32_t requestId, const string& sFuncName, 
                                const char* buff, uint32_t len, 
                                const ServantProxyCallbackPtr& callback);

private:
    /**
     * Զ�̷�������
     * @param req
     * @return int
     */
    virtual int invoke(ReqMessagePtr& req);

    /**
     * Զ�̷������÷���
     * @param req
     * @return int
     */
    virtual int finished(ReqMessagePtr& req);

private:
    friend class ObjectProxy;
    friend class AdapterProxy;

    /**
     * ������ObjectPrx
     */
    ObjectProxy* _objectProxy;

    /**
     * ҵ�����õ�·����
     */
    ServantProxyRouterPtr _router;

    /**
     * ��ʱ(����)
     */
    int _timeout;
};
//////////////////////////////////////////////////////////////////////////
}
#include "servant/ObjectProxy.h"
//////////////////////////////////////////////////////////////////////////
#endif

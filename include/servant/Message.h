#ifndef __TAF_MESSAGE_H_
#define __TAF_MESSAGE_H_

#include "servant/Global.h"
#include "servant/EndpointInfo.h"
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"

namespace taf
{
/**
 * ����ͬ������ʱ����������
 */
struct ReqMonitor : public TC_ThreadLock, public TC_HandleBase {};

typedef TC_AutoPtr<ReqMonitor> ReqMonitorPtr;

#define IS_MSG_TYPE(m, t) ((m & t) != 0)
#define SET_MSG_TYPE(m, t) do { (m |= t); } while (0);
#define CLR_MSG_TYPE(m, t) do { (m &=~t); } while (0);

/**
 * Proxy�˵�������Ϣ�ṹ
 */
struct ReqMessage : public TC_HandleBase//, public TC_ThreadPool::ThreadData
{
    //��������
    enum CallType
    {
        SYNC_CALL = 1, //ͬ��
        ASYNC_CALL,    //�첽
        ONE_WAY,       //����
    };
    /**
     * ���캯��
     */
    ReqMessage();
    /**
     * ��������
     */
    ~ReqMessage();
public:
    /**
     * �ж���Ϣ����
     * 
     * @return bool
     */
    bool is_message_type(uint32_t msg, uint32_t type);
    /**
     * 
     * @param msg
     * @param type
     */
    void set_message_type(uint32_t& msg, uint32_t type);
    /**
     * 
     * @param msg
     * @param type
     */
    void clr_message_type(uint32_t& msg, uint32_t type);

public:
    ReqMessage::CallType        type;       //��������
    ServantPrx::element_type*   proxy;      //���ö˵�proxy����
    ReqMonitorPtr               monitor;    //����ͬ����monitor
    AdapterPrx                  adapter;    //��̨���������͵�
    RequestPacket               request;    //������Ϣ��
    ResponsePacket              response;   //��Ӧ��Ϣ��
    ServantProxyCallbackPtr     callback;   //�첽����ʱ�Ļص�����
    EndpointInfo                endpoint;   //�����󵽵ķ����ַ
    timeval                     begtime;    //����ʱ��
    timeval                     endtime;    //���ʱ��
    int64_t                     hashCode;   //hashֵ���û���֤һ���û��������͵�ͬһ��server(���ϸ�֤)
    bool                        fromRpc;    //�Ƿ��ǵ�����Э���rcp_call��ȱʡΪfalse
};
////////////////////////////////////////////////////////////////////////////////////////////////////
}
#include "servant/ServantProxy.h"

#endif

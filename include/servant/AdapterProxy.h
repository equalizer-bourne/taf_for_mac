#ifndef __TAF_ADAPTER_PROXY_H_
#define __TAF_ADAPTER_PROXY_H_

#include "util/tc_timeout_queue.h"
#include "servant/TransceiverHandle.h"
#include "servant/EndpointInfo.h"
#include "servant/Connector.h"
#include "servant/ObjectProxy.h"
#include "servant/TimeoutHandle.h"
#include "servant/FDReactor.h"
#include "servant/Transceiver.h"
#include "servant/BaseProxy.h"
#include "servant/Global.h"

namespace taf
{
/**
 * ÿ��Adapter��Ӧһ��Endpoint��Ҳ����һ������˿�
 */
class AdapterProxy : public BaseProxy
{
public:
    /**
     * ���캯��
     * @param ep
     * @param op
     * @param rt
     * @param transHandle
     */
    AdapterProxy(Communicator* comm, const EndpointInfo &ep, ObjectProxy* op, FDReactor* rt, TransceiverHandle* transHandle);

    /**
     * ��������
     */
    ~AdapterProxy();

public:
    /**
     * ����server�˶��󷽷�
     * @param req
     * @return int
     */
	int invoke(ReqMessagePtr& req);

    /**
     * server�˷�������
     * @param req
     * @return int
     */
    int finished(ReqMessagePtr& req);

public:
    /**
     * �˿��Ƿ���Ч,������ȫ��ʧЧʱ����false
     * @return bool
     */
    bool checkActive();

    /**
     * ���÷������Ч״̬
     * @param value
     */
    void setActive(bool value);

    /**
     * ����������Ϣ
     */
    void resetInvoke();

    /**
     * ���ӵ�����
     * @param bTimeout, �Ƿ��ǳ�ʱ��
     */
    void finishInvoke(bool bTimeout);

    /**
     * ��ȡ�˿���Ϣ
     * @return const EndpointInfo&
     */
    const EndpointInfo& endpoint();

    /**
     * �Ӷ�����ȡһ������
     * @param trans
     * @return bool
     */
	bool sendRequest(TransceiverPtr& trans);

    /**
     * ��ʱ���trans
     */
    void refreshTransceiver();

    /**
     * ����û���ҵ�request��response(������push��Ϣ)
     * @param req 
     * @return bool 
     */
    bool dispatchPushCallback(ReqMessagePtr& req);

protected:
    /**
     * �Ӷ����л�ȡһ������
     * @param req
     * @return bool
     */
	bool popRequest(ReqMessagePtr& req);

    /**
     * ѡȡһ������
     * @param trans
     * @return TransceiverPtr
     */
    TransceiverPtr selectTransceiver();

    /**
     * ����
     * @return TransceiverPtr
     */
    TransceiverPtr doReconnect();

private:
    EndpointInfo _endpoint;

    ObjectProxy* _objectProxy;

	FDReactor* _reactor;

    TransceiverHandle* _transHandle;

    TC_TimeoutQueue<ReqMessagePtr>* _timeoutQueue;

	vector<TransceiverPtr> _trans;

    int32_t _checkTransInterval;

    time_t _lastCheckTransTime;

    int32_t _currentGridGroup;

    int32_t _connectTimeout;

    uint32_t _timeoutInvoke;

    uint32_t _totalInvoke;

    uint32_t _frequenceFailInvoke;

    time_t _lastFinishInvokeTime;

    time_t _lastRetryTime;

    bool _activeStatus;
};
////////////////////////////////////////////////////////////////////
}
#endif

#ifndef __TAF_TRANSCEIVER_HANDLE_H_
#define __TAF_TRANSCEIVER_HANDLE_H_

#include "servant/AdapterProxy.h"
#include "servant/Transceiver.h"
#include "servant/Global.h"
#include "util/tc_thread.h"
#include "util/tc_thread_queue.h"

namespace taf
{
class FDReactor;

//////////////////////////////////////////////////////
/**
 * ���������¼�
 */
class TransceiverHandle : public TC_HandleBase, public TC_ThreadMutex//, public TC_Thread
{
public:
    enum { R = 0x01, W = 0x02, };

    /**
     * ����������Ϣ
     * ���ڲ�ʹ��
     */
    struct ProxyInfo
    {
        int event;      //�����¼� or ����¼�

        int fd;

        TransceiverPtr trans;

        AdapterPrx adapter;

        ProxyInfo() : event(-1), fd(-1), trans(NULL), adapter(NULL) {}
    };

public:
    /**
     * ���캯��
     * @param comm
     */
    TransceiverHandle(Communicator* comm);

    /**
     * ��������
     */
    virtual ~TransceiverHandle();

public:

    /**
     * 
     * @param fdReactor
     */
    void setFDReactor(FDReactor *fdReactor);

    /**
     * ע��һ�����Ӹ�����adapter
     * @param fd
     * @param trans
     * @param adapter
     */
    void registerProxy(int fd, TransceiverPtr& trans, const AdapterPrx& adapter);

    /**
     * �¼�����
     * @param fd
     * @param ev
     */
    void handle(int fd, int ev);

    /**
     * ��������Ҫд
     * @param fd
     */
    void handleWrite(int fd);

    /**
     * �����쳣
     * @param fd
     * 
     * @return int
     */
    int handleExcept(int fd);

    /**
     * ��ֹhandle
     */
//    void terminate();

protected:
//    void run();

    /**
     * �����¼�
     * @param pi
     */
    void handleInputImp(TransceiverHandle::ProxyInfo& pi);

    /**
     * ����¼�
     * @param pi
     */
    void handleOutputImp(TransceiverHandle::ProxyInfo& pi);

protected:
//    bool _bTerminate;

    Communicator* _comm;

    map<int, TransceiverHandle::ProxyInfo> _proxyInfo;

    FDReactor       *_fdReactor;

//    TC_ThreadQueue<ProxyInfo> _eventQueue;
};
//////////////////////////////////////////////////////
}
#endif

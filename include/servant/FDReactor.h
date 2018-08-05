#ifndef __TAF_FD_REACTOR_H_
#define __TAF_FD_REACTOR_H_

#include "servant/Global.h"
#include "servant/Transceiver.h"
#include "util/tc_thread.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_epoller.h"
#include <set>

namespace taf
{
class TransceiverHandle;
/**
 * ����FD�¼�������ע���handle
 */
class FDReactor : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{
public:
    /**
     * ���캯��
     */
    FDReactor();

    /**
     * ��������
     */
    virtual ~FDReactor();

public:
    /**
     * ѭ�����������¼�
     */
    virtual void run();

    /**
     * ע��fd��Ӧ�Ĵ���handle
     * @param fd
     * @param event
     * @param handle
     */
    virtual void registerHandle(int fd, uint32_t event, TransceiverHandle* handle);

    /**
     * ȡ����ע���handle
     * @param fd
     * @param event
     * @param handle
     */
    virtual void unregisterHandle(int fd, uint32_t event, TransceiverHandle* handle);

    /**
     * ��ֹ����
     */
    virtual void terminate();

    /**
     * ֪ͨ�¼�����
     * @param fd
     */
    void notify(int fd);

protected:
    void handle(int fd, int events);

protected:
    TC_Socket _shutdown;

    bool _terminate;

    TC_Epoller _ep;

    map<int, TransceiverHandle*> _handles;

    //set<int>    _sfd;
};
/////////////////////////////////////////////////////////////////////////////////////
}

#endif

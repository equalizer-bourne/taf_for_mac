#ifndef __TAF_TM_REACTOR_H_
#define __TAF_TM_REACTOR_H_

#include "servant/Global.h"
#include "servant/TMEventHandle.h"
#include "util/tc_thread.h"
#include "util/tc_monitor.h"
#include "util/tc_epoller.h"

namespace taf
{
/**
 * ��ʱ�¼��Ĵ�����
 */
class TMReactor : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{
public:
    /**
     * ��ʱ��Ϣ
     */
    struct TimerInfo
    {
        uint32_t timeout;       //��ʱ����

        time_t lastHandleTime;  //�ϴδ���ʱ��
    };

public:
    /**
     * ���캯��
     */
    TMReactor();

    /**
     * ��������
     */
    virtual ~TMReactor();

public:
    /**
     * ���̣߳�ѭ�����ʱ��
     */
    virtual void run();

    /**
     * ע���¼�������
     * @param timeout
     * @param handle
     */
    virtual void registerHandle(int timeout, TMEventHandle* handle);

    /**
     * ע���¼�������
     * @param timeout
     * @param handle
     */
    virtual void unregisterHandle(int timeout, TMEventHandle* handle);

    /**
     * ��������
     */
    virtual void terminate();

protected:
    bool _terminate;

    map<TMEventHandle*, TimerInfo> _handles;
};
////////////////////////////////////////////////////
}
#endif

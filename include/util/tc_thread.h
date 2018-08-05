#ifndef	__TC_THREAD_H_
#define __TC_THREAD_H_

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include "util/tc_ex.h"
#include "util/tc_monitor.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �߳���(�޸���ICEԴ��)
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
 * �߳̿����쳣��
 */
struct TC_ThreadThreadControl_Exception : public TC_Exception
{
    TC_ThreadThreadControl_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadThreadControl_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadThreadControl_Exception() throw() {};
};

/**
 * �߳̿�����
 */
class TC_ThreadControl
{
public:

    /**
     * ����, ��ʾ��ǰ���е��߳�
     * join��detach�ڲ����ڸö����ϵ���
     */
    TC_ThreadControl();

    /**
     *
     *
     * @return explicit
     */
    explicit TC_ThreadControl(pthread_t);

    /**
     * �ȴ���ǰ�߳̽���, �����ڵ�ǰ�߳��ϵ���
     */
    void join();

    /**
     * detach, �����ڵ�ǰ�߳��ϵ���
     */
    void detach();

    /**
     * ��ǰ�߳�id
     *
     * @return pthread_t
     */
    pthread_t id() const;

    /**
     * ��Ϣmsʱ��
     * @param millsecond
     */
    static void sleep(long millsecond);

    /**
     * ������ǰ�߳̿���Ȩ
     */
    static void yield();

private:

    pthread_t _thread;
};

/**
 *
 */
class TC_Runable
{
public:
    virtual ~TC_Runable(){};
    virtual void run() = 0;
};

/**
 * �̻߳���
 */
class TC_Thread : public TC_Runable
{
public:

	/**
     * ���캯��
	 */
	TC_Thread();

	/**
     * ��������
	 */
	virtual ~TC_Thread(){};

	/**
     * �߳�����
	 */
	TC_ThreadControl start();

    /**
     * ��ȡ�߳̿�����
     *
     * @return ThreadControl
     */
    TC_ThreadControl getThreadControl() const;

    /**
     * �߳��Ƿ���
     *
     * @return bool
     */
    bool isAlive() const;

	/**
     * ��ȡ�߳�id
	 *
	 * @return pthread_t
	 */
	pthread_t id() { return _tid; }

protected:

	/**
     * ��̬����, �߳����
	 * @param pThread
	 */
	static void threadEntry(TC_Thread *pThread);

	/**
     * ����
	 */
    virtual void run() = 0;

protected:
    /**
     * �Ƿ�������
     */
    bool            _running;

    /**
     * �߳�ID
     */
	pthread_t	    _tid;

    /**
     * �߳���
     */
    TC_ThreadLock   _lock;
};

}
#endif


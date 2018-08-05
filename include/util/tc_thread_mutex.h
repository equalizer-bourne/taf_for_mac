#ifndef __TC_THREAD_MUTEX_H
#define __TC_THREAD_MUTEX_H

#include "util/tc_lock.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �߳���������(�޸���ICEԴ��)
// Author : j@syswin.com              
/////////////////////////////////////////////////
class TC_ThreadCond;

/**
 * �̻߳������
 */
struct TC_ThreadMutex_Exception : public TC_Lock_Exception
{
    TC_ThreadMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_ThreadMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_ThreadMutex_Exception() throw() {};
};

/**
* �߳���
*/
class TC_ThreadMutex
{
public:

    TC_ThreadMutex();
    virtual ~TC_ThreadMutex();

    /**
     * ����
     */
    void lock() const;

    /**
     * ������
     * 
     * @return bool
     */
    bool tryLock() const;

    /**
     * ����
     */
    void unlock() const;

    /**
     * ���������unlock�Ƿ�����, ��TC_Monitorʹ�õ�
     * ��Զ����true
     * 
     * @return bool
     */
    bool willUnlock() const { return true;}

protected:

    // noncopyable
    TC_ThreadMutex(const TC_ThreadMutex&);
    void operator=(const TC_ThreadMutex&);

	/**
     * ����
	 */
    int count() const;

    /**
     * ����
	 */
    void count(int c) const;

    friend class TC_ThreadCond;

protected:
    mutable pthread_mutex_t _mutex;

};

/**
* �߳�����
* �����߳̿�ʵ��
**/
class TC_ThreadRecMutex
{
public:

    /**
    * ���캯��
    */
    TC_ThreadRecMutex();

    /**
    * ��������
    */
    virtual ~TC_ThreadRecMutex();

    /**
    * ��, ����pthread_mutex_lock
    * return : ����pthread_mutex_lock�ķ���ֵ
    */
    int lock() const;

    /**
    * ����, pthread_mutex_unlock
    * return : ����pthread_mutex_lock�ķ���ֵ
    */
    int unlock() const;

    /**
    * ������, ʧ���׳��쳣
    * return : true, �ɹ���; false �����߳��Ѿ�����
    */
    bool tryLock() const;

    /**
     * ���������unlock�Ƿ�����, ��TC_Monitorʹ�õ�
     * 
     * @return bool
     */
    bool willUnlock() const;
protected:

	/**
     * ��Ԫ��
     */
    friend class TC_ThreadCond;

	/**
     * ����
	 */
    int count() const;

    /**
     * ����
	 */
    void count(int c) const;

private:
    /**
    ������
    */
    mutable pthread_mutex_t _mutex;
	mutable int _count;
};

}

#endif


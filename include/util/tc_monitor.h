#ifndef _TC_MONITOR_H
#define _TC_MONITOR_H

#include "util/tc_thread_mutex.h"
#include "util/tc_thread_cond.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: ���������(�޸���ICEԴ��)
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
 * �����
 */
template <class T, class P>
class TC_Monitor
{
public:

    /**
     * ���������ƶ���
     */
    typedef TC_LockT<TC_Monitor<T, P> > Lock;
    typedef TC_TryLockT<TC_Monitor<T, P> > TryLock;

    /**
     * ���캯��
     */
    TC_Monitor() : _nnotify(0)
    {
    }

    /**
     * ����
     */
    virtual ~TC_Monitor()
    {
    }

    /**
     * ��
     */
    void lock() const
    {
        _mutex.lock();
        _nnotify = 0;
    }

    /**
     * ����, ���������Ĵ���֪ͨ
     */
    void unlock() const
    {
//        int nnotify = _nnotify;
        notifyImpl(_nnotify);
        _mutex.unlock();
//        notifyImpl(nnotify);
    }

    /**
     * ������
     *
     * @return bool
     */
    bool tryLock() const
    {
        bool result = _mutex.tryLock();
        if(result)
        {
            _nnotify = 0;
        }
        return result;
    }

    /**
     * �ȴ�
     */
    void wait() const
    {
        notifyImpl(_nnotify);

        try
        {
            _cond.wait(_mutex);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
    }

    /**
     * ��ʱ��
     * @param millsecond
     *
     * @return bool, false:��ʱ��, ture:���¼�����
     */
    bool timedWait(int millsecond) const
    {
        notifyImpl(_nnotify);

        bool rc;

        try
        {
            rc = _cond.timedWait(_mutex, millsecond);
        }
        catch(...)
        {
            _nnotify = 0;
            throw;
        }

        _nnotify = 0;
        return rc;
    }

    /**
     * ֪ͨĳһ���߳�����
     * ���øú���֮ǰ�������, �ڽ�����ʱ�������֪ͨ
     */
    void notify()
    {
        if(_nnotify != -1)
        {
            ++_nnotify;
        }
    }

    /**
     * ֪ͨ���е��߳�����
     * �ú�������ǰ֮�������, �ڽ�����ʱ�������֪ͨ
     *
     */
    void notifyAll()
    {
        _nnotify = -1;
    }

protected:

    /**
     * ֪ͨʵ��
     * @param nnotify
     */
    void notifyImpl(int nnotify) const
    {
        if(nnotify != 0)
        {
            if(nnotify == -1)
            {
                _cond.broadcast();
                return;
            }
            else
            {
                while(nnotify > 0)
                {
                    _cond.signal();
                    --nnotify;
                }
            }
        }
    }

private:

    // noncopyable
    TC_Monitor(const TC_Monitor&);
    void operator=(const TC_Monitor&);

protected:

    mutable int     _nnotify;
    mutable P       _cond;
    T               _mutex;
};

/**
 * ��ͨ�߳���
 */
typedef TC_Monitor<TC_ThreadMutex, TC_ThreadCond> TC_ThreadLock;

/**
 * ѭ����(һ���߳̿��ԼӶ����)
 */
typedef TC_Monitor<TC_ThreadRecMutex, TC_ThreadCond> TC_ThreadRecLock;

}
#endif


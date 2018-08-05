#ifndef _TC_LOCK_H
#define _TC_LOCK_H

#include <string>
#include <stdexcept>
#include <cerrno>
#include "util/tc_ex.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: ����
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* ���쳣
*/
struct TC_Lock_Exception : public TC_Exception
{
    TC_Lock_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Lock_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Lock_Exception() throw() {};
};

/**
 * ��ģ����
 */
template <typename T>
class TC_LockT
{
public:

    /**
     * ���캯��
     * @param mutex
     */
    TC_LockT(const T& mutex) : _mutex(mutex)
    {
        _mutex.lock();
        _acquired = true;
    }

    /**
     * ����
     */
    virtual ~TC_LockT()
    {
        if (_acquired)
        {
            _mutex.unlock();
        }
    }

    /**
     * ����, ����Ѿ�����,���׳��쳣
     */
    void acquire() const
    {
        if (_acquired)
        {
            throw TC_Lock_Exception("thread has locked!");
        }
        _mutex.lock();
        _acquired = true;
    }

    /**
     * ��������
     *
     * @return bool
     */
    bool tryAcquire() const
    {
        _acquired = _mutex.tryLock();
        return _acquired;
    }

    /**
     * �ͷ���, ���û���Ϲ���, ���׳��쳣
     */
    void release() const
    {
        if (!_acquired)
        {
            throw TC_Lock_Exception("thread hasn't been locked!");
        }
        _mutex.unlock();
        _acquired = false;
    }

    /**
     * �Ƿ��Ѿ�����
     *
     * @return bool
     */
    bool acquired() const
    {
        return _acquired;
    }

protected:

    // TC_TryLockT's contructor
    TC_LockT(const T& mutex, bool) : _mutex(mutex)
    {
        _acquired = _mutex.tryLock();
    }

private:

    // Not implemented; prevents accidental use.
    TC_LockT(const TC_LockT&);
    TC_LockT& operator=(const TC_LockT&);

protected:
    /**
     * ������
     */
    const T&        _mutex;

    /**
     * �Ƿ��Ѿ�����
     */
    mutable bool _acquired;
};

/**
 * ��������
 */
template <typename T>
class TC_TryLockT : public TC_LockT<T>
{
public:

    TC_TryLockT(const T& mutex) : TC_LockT<T>(mutex, true)
    {
    }
};

/**
 * ����, �����κ�������
 */
class TC_EmptyMutex
{
public:
    /**
    * д��
    * @return int, 0 ��ȷ
    */
    int lock()  const   {return 0;}

    /**
    * ��д��
    */
    int unlock() const  {return 0;}

    /**
    * ���Խ���
    * @return int, 0 ��ȷ
    */
    bool trylock() const {return true;}
};

};
#endif


#ifndef _JMEM_POLICY_H
#define _JMEM_POLICY_H

#include "util/tc_thread_mutex.h"
#include "util/tc_sem_mutex.h"
#include "util/tc_shm.h"
#include "util/tc_mmap.h"

namespace taf
{
//////////////////////////////////////////////////////////////////////
// �洢����: �ڴ�, �����ڴ�, mmap(�ļ�)

/**
 * �ڴ�洢
 */
template<typename T, typename LockPolicy>
class MemStorePolicy : public LockPolicy
{
public:
    /**
    * ��ʼ��
    * @param pAddr: ָ����пռ��ָ��
    * @param iSize: �ռ��ָ��
    */
    void create(void *pAddr, size_t iSize)
    {
        _t.create(pAddr,iSize);
    }

    /**
    * �����϶���
    * @param pAddr: ָ����пռ��ָ��
    * @param iSize: �ռ��ָ��
    */
    void connect(void *pAddr, size_t iSize)
    {
        _t.connect(pAddr,iSize);
    }

protected:
    T   _t;
};

/**
 * �����ڴ�洢
 */
template<typename T, typename LockPolicy>
class ShmStorePolicy : public LockPolicy
{
public:
    /**
     * ��ʼ������洢
     * @param iShmKey
     * @param iSize
     */
    void initStore(key_t iShmKey, size_t iSize)
    {
        _shm.init(iSize, iShmKey);
        if(_shm.iscreate())
        {
            _t.create(_shm.getPointer(), iSize);
        }
        else
        {
            _t.connect(_shm.getPointer(), iSize);
        }
    }

    /**
     * �ͷŹ����ڴ�
     */
    void release()
    {
        _shm.del();
    }
protected:
    TC_Shm  _shm;
    T       _t;
};

/**
 * �ļ��洢
 */
template<typename T, typename LockPolicy>
class FileStorePolicy : public LockPolicy
{
public:
    /**
     * ��ʼ���ļ�
     * @param file, �ļ�·��
     * @param iSize, �ļ���С
     */
    void initStore(const char *file, size_t iSize)
    {
        _file = file;
        _mmap.mmap(file, iSize);
        if(_mmap.iscreate())
        {
            _t.create(_mmap.getPointer(), iSize);
        }
        else
        {
            _t.connect(_mmap.getPointer(), iSize);
        }
    }

    /**
     * ��չ�ռ�, Ŀǰֻ��hashmap��Ч
     */
    int expand(size_t iSize)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        TC_Mmap m(false);
        m.mmap(_file.c_str(), iSize);

        int ret = _t.append(m.getPointer(), iSize);

        if(ret == 0)
        {
            _mmap.munmap();
            _mmap = m;
            _mmap.setOwner(true);
        }
        else
        {
            m.munmap();
        }

        return ret;
    }

protected:
    string  _file;
    TC_Mmap _mmap;
    T       _t;
};

//////////////////////////////////////////////////////////////////////
// ������: ����, �߳���, ������

/**
 * ����
 */
class EmptyLockPolicy
{
public:
    typedef TC_EmptyMutex Mutex;
    Mutex &mutex()     { return _mutex; }

protected:
    Mutex _mutex;
};

/**
 * �߳�������
 */
class ThreadLockPolicy
{
public:
    typedef TC_ThreadMutex Mutex;
    Mutex &mutex()     { return _mutex; }

protected:
    Mutex _mutex;
};

/**
 * ����������
 */
class SemLockPolicy
{
public:
    typedef TC_SemMutex Mutex;
    void initLock(key_t iSemKey)    { return _mutex.init(iSemKey); }
    Mutex &mutex()                  { return _mutex; }

protected:
    Mutex _mutex;
};

}

#endif

#ifndef _JMEM_QUEUE_H
#define _JMEM_QUEUE_H

#include "util/tc_mem_queue.h"
#include "jmem/jmem_policy.h"
#include "jce/Jce.h"

namespace taf
{

/**
 * ����JceЭ����ڴ�ѭ������
 * �����������׳�JceDecodeException��JceEncodeException
 * ���Զ������Ժʹ洢���Խ������, ����:
 * �����ź�����, �ļ��洢�Ķ���:
 * JceQueue<Test::QueueElement, SemLockPolicy, FileStorePolicy>
 * �����ź�����, �����ڴ�洢�Ķ���
 * JceQueue<Test::QueueElement, SemLockPolicy, ShmStorePolicy>
 * �����߳���, �ڴ�洢�Ķ���
 * JceQueue<Test::QueueElement, ThreadLockPolicy, MemStorePolicy>
 * 
 * ʹ����, ��ͬ�����, ��ʼ����������ȫһ��
 * ��ʼ��������:
 * SemLockPolicy::initLock(key_t)
 * ShmStorePolicy::initStore(key_t, size_t)
 * FileStorePolicy::initStore(const char *file, size_t)
 * ��, ����μ�jmem_policy.h
 */

template<typename T,
         typename LockPolicy,
         template<class,class> class StorePolicy>
class JceQueue : public StorePolicy<TC_MemQueue, LockPolicy>
{
public:
    /**
     * ����һ��Ԫ��
     * @param t
     *
     * @return bool,true:�ɹ�, false:��Ԫ��
     */
    bool pop_front(T &t)
    {
        string s;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            if(!this->_t.pop_front(s))
            {
                return false;
            }
        }
        taf::JceInputStream<BufferReader> is;
        is.setBuffer(s.c_str(), s.length());
        t.readFrom(is);

        return true;
    }

    /**
     * ���뵽����
     * @param t
     *
     * @return bool, ture:�ɹ�, false:������
     */
    bool push_back(const T &t)
    {
        taf::JceOutputStream<BufferWriter> os;
        t.writeTo(os);

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.push_back(os.getBuffer(), os.getLength());
    }

    /**
     * �Ƿ�����
     * @param t
     * 
     * @return bool
     */
    bool isFull(const T &t) 
    { 
        taf::JceOutputStream<BufferWriter> os;
        t.writeTo(os);

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isFull(os.getLength()); 
    }

    /**
    * �����Ƿ���
    * @param : iSize, �������ݿ鳤��
    * @return bool , true:��, false: ����
    */
    bool isFull(size_t iSize) 
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isFull(iSize); 
    }

    /**
    * �����Ƿ��
    * @return bool , true: ��, false: ����
    */
    bool isEmpty()
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isEmpty();
    }

    /**
    * ������Ԫ�ظ���, ������������²���֤һ����ȷ
    * @return size_t, Ԫ�ظ���
    */
    size_t elementCount()
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.elementCount();
    }


    /**
    * ���г���(�ֽ�), С���ܴ洢������(�ܴ洢�����Ȱ����˿��ƿ�)
    * @return size_t : ���г���
    */
    size_t queueSize()
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.queueSize();
    }

    /**
    * �����ڴ泤��
    * @return size_t : �����ڴ泤��
    */
    size_t memSize() const 
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.memSize();
    };
};

}

#endif

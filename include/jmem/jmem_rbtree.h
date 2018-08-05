#ifndef _JMEM_RBTREE_H
#define _JMEM_RBTREE_H

#include "util/tc_rbtree.h"
#include "util/tc_autoptr.h"
#include "jmem/jmem_policy.h"
#include "jce/Jce.h"

namespace taf
{

/************************************************************************
 ����˵������:
 ����JceЭ����ڴ�rbtree
 �����������׳�JceDecodeException��JceEncodeException
 ���Զ������Ժʹ洢���Խ������, ����:
 �����ź�����, �ļ��洢��rbtree:
 JceRBtree<Test::QueueElement, SemLockPolicy, FileStorePolicy>
 �����ź�����, �����ڴ�洢��rbtree
 JceRBTree<Test::QueueElement, SemLockPolicy, ShmStorePolicy>
 �����߳���, �ڴ�洢��rbtree
 JceRBTree<Test::QueueElement, ThreadLockPolicy, MemStorePolicy>
 
 ʹ����, ��ͬ�����, ��ʼ����������ȫһ��
 ��ʼ��������:
 SemLockPolicy::initLock(key_t)
 ShmStorePolicy::initStore(key_t, size_t)
 FileStorePolicy::initStore(const char *file, size_t)
 ��, ����μ�jmem_policy.h
 
 ***********************************************************************

 ��������˵��:
 > �ڴ����ݺ����, �������Getʱ���˳����̭����;
 > ֧�ֻ�д/dump���ļ�/���߱���;
 > ֧�ֲ�ͬ��С�ڴ�������, �ṩ�ڴ��ʹ����;
 > ֧�ֻ��յ�ָ�����б��ʵĿռ�;
 > ֧�ֽ�����Key�Ĳ���, ��������value, ֻ��Key, ������stl::set;
 > ֧�ּ��ַ�ʽ�ı���, ͨ������ʱ��Ҫ��map����; 
 > ֧���Զ��������������, ���Էǳ�����ʵ����صĽӿ�;
 > ֧���Զ������, Key��Value�Ľṹ��ͨ��jce2cpp����;
 > jceЭ��֧���Զ���չ�ֶ�, ��˸�hashmap֧���Զ���չ�ֶ�(Key��Value��������ͨ��jce�����);
 > map֧��ֻ��ģʽ, ֻ��ģʽ��set/erase/del���޸����ݵĲ�������ʹ��, get/��д/���߱�������ʹ��
 > ֧���Զ���̭, setʱ, �ڴ������Զ���̭, �ڷ��Զ���̭ʱ, �ڴ���ֱ�ӷ���RT_READONLY
 > ����mmap�ļ�, ֧���Զ���չ�ļ�, ���ڴ治����, �����Զ���չ�ļ���С(ע��hash����������, ��˿�ʼ����Ҫ����hash������), ���Ҳ��ܿ�JRBTree���󣨼�����rbtree�������ͬһ���ļ���֪ͨһ��rbtree��չ�Ժ�����һ�����󲢲�֪����չ�ˣ�

 ***********************************************************************

 rbtree��˵��:
 rbtree��һ�����������¼�������:
 > Setʱ����: �κ�Set���������޸ĸ�����, Set�����ݱ�����Ϊ������, ���ƶ���Set����ͷ��;
 > Getʱ����: �κ�Get���������޸ĸ�����, ��������ֻ��, ע��Set��ͬʱҲ���޸�Get��
 > Dirtyʱ����: dirty����Set����һ����, ���ڻ�д������
 > Backup��:��������Get����һ����, ����������ʱ, ˳��Get����β����ͷ����ʼ����;
 
 ***********************************************************************

 ��ز���˵��:
 > ��������mapֻ��, ������д��������RT_READONLY, ��ʱGet�������޸�����
 > ��������֪���Զ���̭, Ĭ�����Զ���̭��.������Զ���̭,��setʱ,���ڴ�ռ䷵��:RT_NO_MEMORY
 > ���Ը���key�Ƚϵ��㷨, ����setLessFunctor����
 > ���Խ�ĳ����������Ϊ�ɾ�, ��ʱ�Ƴ���Dirty����ָDirtyβ������һ��Ԫ��;
 > ���Խ�ĳ����������Ϊ��, ��ʱ���ƶ���Set����ͷ��;
 > ÿ�����ݶ���һ���ϴλ�дʱ��(SyncTime), ���������д����, ����һ��ʱ���ڻ��д;
 > ����dump���ļ����ߴ��ļ���load, ���ʱ����map����
 > ���Ե���erase������̭����ֱ���ڴ�����ʵ�һ������
 > ���Ե���sync�������ݻ�д, ��֤һ��ʱ����û�л�д�����ݻ��д, map��дʱ��ͨ��setSyncTime����, Ĭ��10����
 > ����setToDoFunctor���ò�����, �����ǲ������������:
  
 ***********************************************************************
 
 ToDoFunctor�ĺ���˵��:
 > ͨ���̳�ToDoFunctor, ʵ����غ����Ϳ�����, ����ʵ�����¹���:Get����, ��̭����, ɾ������, ��д����, ��������
 > ToDoFunctor::erase, ������map.eraseʱ, �ú����ᱻ����
 > ToDoFunctor::del, ������map.delʱ, �ú����ᱻ����, ע��ɾ��ʱ���ݿ��ܶ�����cache��;
 > ToDoFunctor::sync, ������map.syncʱ, �ᴥ��ÿ����Ҫ��д�����ݸú�����������һ��, �ڸú����д����д����;
 > ToDoFunctor::backup, ������map.backupʱ, �ᴥ����Ҫ���ݵ����ݸú����ᱻ����һ��, �ڸú����д���������;
 > ToDoFunctor::get, ������map.getʱ, ���map��������, ��ú���������, �ú�����db�л�ȡ����, ������RT_OK, ���db�������򷵻�RT_NO_DATA;
 > ToDoFunctor���нӿڱ�����ʱ, �������map����, ��˿��Բ���map

 ***********************************************************************

 map����Ҫ����˵��:
 > set, �������ݵ�map��, �����set����
        �������, �ҿ����Զ���̭, �����Get����̭����, ��ʱToDoFunctor��sync�ᱻ����
        �������, �ҿ��Բ����Զ���̭, �򷵻�RT_NO_MEMORY

 > get, ��map��ȡ����, ���������, ��ֱ�Ӵ�map��ȡ���ݲ�����RT_OK;
        ���û������, �����ToDoFunctor::get����, ��ʱget������Ҫ����RT_OK, ͬʱ�����õ�map��, ����������;
        ���û������, ��ToDoFunctor::get����Ҳ������, ����Ҫ����RT_NO_DATA, ��ʱֻ���Key���õ�map��, ������RT_ONLY_KEY;
        �����������, �������get����, ���ٵ���ToDoFunctor::get, ֱ�ӷ���RT_ONLY_KEY;

 > del, ɾ������, ����cache�Ƿ�������, ToDoFunctor::del���ᱻ����;
        ���ֻ��Key, �������Ҳ�ᱻɾ��;

 > erase, ��̭����, ֻ��cache��������, ToDoFunctor::erase�Żᱻ����
          ���ֻ��Key, �������Ҳ�ᱻ��̭, ����ToDoFunctor::erase���ᱻ����;

 > erase(int radio), ������̭����, ֱ�����п���ʵ���radio;
                     ToDoFunctor::erase�ᱻ����;
                     ֻ��Key�ļ�¼Ҳ�ᱻ��̭, ����ToDoFunctor::erase���ᱻ����;

 > sync: ��д����, ��ʱû�л�д������������Ҫ��д, ��д��Ϻ�, ���ݻ��Զ�����Ϊ�ɾ�����;
         ���Զ���̻߳����ͬʱ��д;
         ToDoFunctor::sync�ᱻ����;
         ֻ��Key�ļ�¼, ToDoFunctor::sync���ᱻ����;

 > backup: ��������, ˳��˳��Get����β����ͷ����ʼ����;
           ToDoFunctor::backup�ᱻ����;
           ֻ��Key�ļ�¼, ToDoFunctor::backup���ᱻ����;
           ���ڱ����α�ֻ��һ��, ��˶������ͬʱ���ݵ�ʱ�����ݿ��ܻ�ÿ��������һ����
           ������ݳ��򱸷ݵ�һ��down��, ���´���������ʱ������ϴεı��ݽ���, ���ǽ�backup(true)���ñ���

 ***********************************************************************

 ����ֵ˵��: 
 > ע�⺯������int�ķ���ֵ, �����ر�˵��, ��μ�TC_RBTree::RT_
 
 ***********************************************************************

 ����˵��:
 > ������lock_iterator��map�������¼��ֱ���, �ڱ�����������ʵ��map����������
 > beginSetTime(): ����Setʱ��˳�����
 > rbeginSetTime(): ����Setʱ��˳�����
 > beginGetTime(): ����Getʱ��˳�����
 > rbeginGetTime(): ����Getʱ���������
 > beginDirty(): ��ʱ�����������������(���setClean, ��Ҳ��������������)
 > ��ʵ��д�������������������Ӽ�
 > ע��:lock_iteratorһ����ȡ, �Ͷ�map������, ֱ��lock_iterator����Ϊֹ
 
 lock_iterator��nolock_iterator������:
 > map�ĺ����������lock_iterator��,���Զ���map������,ֱ��lock_iterator�����������Զ�����
 > map�ĺ����������nolock_iterator, �����map����, ֻ����nolock_iterator����++����get��ʱ��ż���
 > nolock_iterator�������ǿ��Ա���mapʱ,�����map�Ӵ������,���Ǳ�����Ч�ʻ��һЩ
 > nolock_iterator����get����ʱ,����ж�get����ֵ,��Ϊ������ָ��������п���ʧЧ
*/

template<typename K,
         typename V,
         typename LockPolicy,
         template<class, class> class StorePolicy>
class JceRBTree : public StorePolicy<TC_RBTree, LockPolicy>
{
public:

    /**
    * �������ݲ�������
    * ��ȡ,����,ɾ��,��̭ʱ������ʹ�øò�����
    */
    class ToDoFunctor
    {
    public:
        /**
         * ���ݼ�¼
         */
        struct DataRecord
        {
            K       _key;
            V       _value;
            bool    _dirty;
            time_t  _iSyncTime;

            DataRecord() : _dirty(true), _iSyncTime(0)
            {
            }
        };

        /**
         * ����
         */
        virtual ~ToDoFunctor(){};

        /**
         * ��̭����
         * @param stDataRecord: ����̭������
         */
        virtual void erase(const DataRecord &stDataRecord){};

        /**
         * ɾ������
         * @param bExists: �Ƿ��������
         * @param stDataRecord: ����, bExists==trueʱ��Ч, ����ֻ��key��Ч
         */
        virtual void del(bool bExists, const DataRecord &stDataRecord){};

        /**
         * ��д����
         * @param stDataRecord: ����
         */
        virtual void sync(const DataRecord &stDataRecord){};

        /**
         * ��������
         * @param stDataRecord: ����
         */
        virtual void backup(const DataRecord &stDataRecord){};

        /**
         * ��ȡ����, Ĭ�Ϸ���RT_NO_GET
         * stDataRecord��_key��Ч, ����������Ҫ����
         * @param stDataRecord: ��Ҫ��ȡ������
         *
         * @return int, ��ȡ������, ����:TC_RBTree::RT_OK
         *              û������,����:TC_RBTree::RT_NO_DATA,
         *              ϵͳĬ��GET,����:TC_RBTree::RT_NO_GET
         *              ����,�򷵻�:TC_RBTree::RT_LOAD_DATA_ERR
         */
        virtual int get(DataRecord &stDataRecord)
        {
            return TC_RBTree::RT_NO_GET;
        }
    };

    ///////////////////////////////////////////////////////////////////
    /**
     * �Զ���, ���ڵ�����
     */
    class JhmAutoLock : public TC_HandleBase
    {
    public:
        /**
         * ����
         * @param mutex
         */
        JhmAutoLock(typename LockPolicy::Mutex &mutex) : _lock(mutex)
        {
        }

    protected:
        //��ʵ��
        JhmAutoLock(const JhmAutoLock &al);
        JhmAutoLock &operator=(const JhmAutoLock &al);

    protected:
        /**
         * ��
         */
        TC_LockT<typename LockPolicy::Mutex>   _lock;
    };

    typedef TC_AutoPtr<JhmAutoLock> JhmAutoLockPtr;

    ///////////////////////////////////////////////////////////////////
    /**
     * ������
     */
    class JhmLockItem
    {
    public:

        /**
         * ���캯��
         * @param item
         */
        JhmLockItem(const TC_RBTree::RBTreeLockItem &item)
        : _item(item)
        {
        }

        /**
         * ��������
         * @param it
         */
        JhmLockItem(const JhmLockItem &item)
        : _item(item._item)
        {
        }

        /**
         * 
         */
        JhmLockItem()
        {
        }
        /**
         * ����
         * @param it
         *
         * @return JhmLockItem&
         */
        JhmLockItem& operator=(const JhmLockItem &item)
        {
            if(this != &item)
            {
                _item     = item._item;
            }

            return (*this);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator==(const JhmLockItem& item)
        {
            return (_item == item._item);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator!=(const JhmLockItem& item)
        {
            return !((*this) == item);
        }

        /**
         * �Ƿ���������
         *
         * @return bool
         */
        bool isDirty()          { return _item.isDirty(); }

        /**
         * �Ƿ�ֻ��Key
         *
         * @return bool
         */
        bool isOnlyKey()        { return _item.isOnlyKey(); }

        /**
         * ����дʱ��
         *
         * @return time_t
         */
        time_t getSyncTime()    { return _item.getSyncTime(); }

    	/**
    	 * ��ȡֵ
         * @return int
         *          TC_RBTree::RT_OK:���ݻ�ȡOK
         *          ����ֵ, �쳣
    	 */
    	int get(K& k)
        {
            string sk;
            int ret = _item.get(sk);
            if(ret != TC_RBTree::RT_OK)
            {
                return ret;
            }

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sk.c_str(), sk.length());
            k.readFrom(is);

            return ret;
        }

    	/**
    	 * ��ȡֵ
         * @return int
         *          TC_RBTree::RT_OK:���ݻ�ȡOK
         *          TC_RBTree::RT_ONLY_KEY: key��Ч, v��ЧΪ��
         *          ����ֵ, �쳣
    	 */
    	int get(K& k, V& v)
        {
            string sk;
            string sv;
            int ret = _item.get(sk, sv);
            if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
            {
                return ret;
            }

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sk.c_str(), sk.length());
            k.readFrom(is);

            if(ret != TC_RBTree::RT_ONLY_KEY)
            {
                is.setBuffer(sv.c_str(), sv.length());
                v.readFrom(is);
            }

            return ret;
        }

    protected:
        TC_RBTree::RBTreeLockItem _item;
    };

    ///////////////////////////////////////////////////////////////////
    /**
     * ������
     */
    struct JhmLockIterator
    {
    public:

        /**
         * ����
         * @param it
         * @param lock
         */
        JhmLockIterator(const TC_RBTree::lock_iterator it, const JhmAutoLockPtr &lock)
        : _it(it), _item(it._iItem), _lock(lock)
        {
        }

        /**
         * ��������
         * @param it
         */
        JhmLockIterator(const JhmLockIterator &it)
        : _it(it._it), _item(it._item), _lock(it._lock)
        {
        }

        /**
         * ����
         */
        JhmLockIterator()
        {
        }

        /**
         * ����
         * @param it
         *
         * @return JhmLockIterator&
         */
        JhmLockIterator& operator=(const JhmLockIterator &it)
        {
            if(this != &it)
            {
                _it     = it._it;
                _item   = it._item;
                _lock   = it._lock;
            }

            return (*this);
        }

        /**
         *
         * @param it
         *
         * @return bool
         */
        bool operator==(const JhmLockIterator& it)
        {
            return (_it == it._it && _item == it._item);
        }

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const JhmLockIterator& it)
        {
            return !((*this) == it);
        }

    	/**
    	 * ǰ��++
    	 *
    	 * @return JhmLockIterator&
    	 */
    	JhmLockIterator& operator++()
        {
            ++_it;
            _item = JhmLockItem(_it._iItem);
            return (*this);
        }

    	/**
         * ����++
         *
         * @return JhmLockIterator&
    	 */
    	JhmLockIterator operator++(int)
        {
            JhmLockIterator jit(_it, _lock);
            ++_it;
            _item = JhmLockItem(_it._iItem);
            return jit;
        }

    	/**
         * ��ȡ������
    	 *
    	 * @return JhmLockItem&
    	 */
    	JhmLockItem& operator*()     { return _item; }

    	/**
         * ��ȡ������
    	 *
    	 * @return JhmLockItem*
    	 */
    	JhmLockItem* operator->()   { return &_item; }

    protected:

        /**
         * ������
         */
        TC_RBTree::lock_iterator  _it;

        /**
         * ������
         */
        JhmLockItem                     _item;

        /**
         * ��
         */
        JhmAutoLockPtr              _lock;
    };

    typedef JhmLockIterator lock_iterator ;

    ///////////////////////////////////////////////////////////////////
    /**
     * ��, ���ڷ���������
     * 
     */
    class JhmLock : public TC_HandleBase
    {
    public:
        /**
         * ����
         * @param mutex
         */
        JhmLock(typename LockPolicy::Mutex &mutex) : _mutex(mutex)
        {
        }

        /**
         * ��ȡ��
         * 
         * @return typename LockPolicy::Mutex
         */
        typename LockPolicy::Mutex& mutex()
        {
            return _mutex;
        }
    protected:
        //��ʵ��
        JhmLock(const JhmLock &al);
        JhmLock &operator=(const JhmLock &al);

    protected:
        /**
         * ��
         */
        typename LockPolicy::Mutex &_mutex;
    };

    typedef TC_AutoPtr<JhmLock> JhmLockPtr;

    ///////////////////////////////////////////////////////////////////
    /**
     * ������
     */
    class JhmItem
    {
    public:

        /**
         * ���캯��
         * @param item
         */
        JhmItem(const TC_RBTree::RBTreeItem &item, const JhmLockPtr &lock)
        : _item(item), _lock(lock)
        {
        }

        /**
         * ��������
         * @param it
         */
        JhmItem(const JhmItem &item)
        : _item(item._item), _lock(item._lock)
        {
        }

        /**
         * ����
         * @param it
         *
         * @return JhmItem&
         */
        JhmItem& operator=(const JhmItem &item)
        {
            if(this != &item)
            {
                _item     = item._item;
                _lock     = item._lock;
            }

            return (*this);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator==(const JhmItem& item)
        {
            return (_item == item._item);
        }

        /**
         *
         * @param item
         *
         * @return bool
         */
        bool operator!=(const JhmItem& item)
        {
            return !((*this) == item);
        }

    	/**
         * ��ȡ��ǰ����
         * @param
         * 
         * @return int
         *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
         *          TC_RBTree::RT_ONLY_KEY:ֻ��Key��Ч
         *          TC_RBTree::RT_OK: �ɹ�(key��value����Ч)
         *          ��������ֵ: ����
         */
    	int get(K &k, V &v)
        {
            int ret;
            TC_RBTree::BlockData stData;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                ret = _item.get(stData);
            }

            if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
            {
                return ret;
            }

            try
            {
                taf::JceInputStream<BufferReader> is;
                is.setBuffer(stData._key.c_str(), stData._key.length());
                k.readFrom(is);

                if(ret == TC_RBTree::RT_OK)
                {
                    is.setBuffer(stData._value.c_str(), stData._value.length());
                    v.readFrom(is);
                }
            }
            catch(exception &ex)
            {
                return TC_RBTree::RT_DECODE_ERR;
            }

            return ret;
        }

    protected:
        TC_RBTree::RBTreeItem _item;
        JhmLockPtr            _lock;
    };


    ///////////////////////////////////////////////////////////////////
    /**
     * ������
     */
    struct JhmIterator
    {
    public:

        /**
         * ����
         * @param it
         * @param lock
         */
        JhmIterator(const TC_RBTree::nolock_iterator &it, const JhmLockPtr &lock)
        : _it(it), _item(it._iItem, lock), _lock(lock)
        {
        }

        /**
         * ��������
         * @param it
         */
        JhmIterator(const JhmIterator &it)
        : _it(it._it), _item(it._item), _lock(it._lock)
        {
        }

        /**
         * ����
         * @param it
         *
         * @return JhmIterator&
         */
        JhmIterator& operator=(const JhmIterator &it)
        {
            if(this != &it)
            {
                _it     = it._it;
                _item   = it._item;
            }

            return (*this);
        }

        /**
         *
         * @param it
         *
         * @return bool
         */
        bool operator==(const JhmIterator& it)
        {
            return (_it == it._it && _item == it._item);
        }

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const JhmIterator& it)
        {
            return !((*this) == it);
        }

    	/**
    	 * ǰ��++
    	 *
    	 * @return JhmIterator&
    	 */
    	JhmIterator& operator++()
        {
            TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
            ++_it;
            _item = JhmItem(_it._iItem, _lock);
            return (*this);
        }

    	/**
         * ����++
         *
         * @return JhmIterator&
    	 */
    	JhmIterator operator++(int)
        {
            TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
            JhmIterator jit(_it, _lock);
            ++_it;
            _item = JhmItem(_it._iItem, _lock);
            return jit;
        }

    	/**
         * ��ȡ������
    	 *
    	 * @return JhmItem&
    	 */
    	JhmItem& operator*()     { return _item; }

    	/**
         * ��ȡ������
    	 *
    	 * @return JhmItem*
    	 */
    	JhmItem* operator->()   { return &_item; }

    protected:

        /**
         * ������
         */
        TC_RBTree::nolock_iterator  _it;

        /**
         * ������
         */
        JhmItem               _item;

        /**
         * ��
         */
        JhmLockPtr            _lock;
    };

    typedef JhmIterator nolock_iterator ;

    /**
     * Ĭ�ϵıȽ�
     */
    struct RBTreeLess
    {
        bool operator()(const string &k1, const string &k2)
        {
            K tk1;
            K tk2;

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(k1.c_str(), k1.length());
            tk1.readFrom(is);

            is.setBuffer(k2.c_str(), k2.length());
            tk2.readFrom(is);

            return tk1 < tk2;
        }
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    /**
     * ���캯��
     */
    JceRBTree()
    {
        _todo_of = NULL;

        this->_t.setLessFunctor(RBTreeLess());
    }

    /**
     * ��ʼ�����ݿ�ƽ����С
     * ��ʾ�ڴ�����ʱ�򣬻����n����С�飬 n������С��*�������ӣ�, n������С��*��������*�������ӣ�..., ֱ��n������
     * n��hashmap�Լ����������
     * ���ַ������ͨ���������ݿ��¼�䳤�Ƚ϶��ʹ�ã� ���ڽ�Լ�ڴ棬������ݼ�¼�������Ǳ䳤�ģ� ����С��=���죬��������=1�Ϳ�����
     * @param iMinDataSize: ��С���ݿ��С
     * @param iMaxDataSize: ������ݿ��С
     * @param fFactor: �������� >= 1.0
     */
    void initDataBlockSize(size_t iMinDataSize, size_t iMaxDataSize, float fFactor)
    {
        this->_t.initDataBlockSize(iMinDataSize, iMaxDataSize, fFactor);
    }

    /**
     * ����less��ʽ
     * @param lessf
     */
    void setLessFunctor(TC_RBTree::less_functor lessf)     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setLessFunctor(lessf); 
    }

    /**
     * ��ȡless��ʽ
     * 
     * @return TC_RBTree::less_functor&
     */
    TC_RBTree::less_functor &getLessFunctor()              { return this->_t.getLessFunctor(); }

    /**
     * ������̭������
     * @param erase_of
     */
    void setToDoFunctor(ToDoFunctor *todo_of)               { this->_todo_of = todo_of; }

    /**
     * ��ȡÿ�ִ�С�ڴ���ͷ����Ϣ
     *
     * @return vector<TC_MemChunk::tagChunkHead>: ��ͬ��С�ڴ��ͷ����Ϣ
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail()      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getBlockDetail(); 
    }

    /**
     * ����block��chunk�ĸ���
     *
     * @return size_t
     */
    size_t allBlockChunkCount()                             
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.allBlockChunkCount(); 
    }

    /**
     * ÿ��block��chunk�ĸ���(��ͬ��С�ڴ��ĸ�����ͬ)
     *
     * @return size_t
     */
    vector<size_t> singleBlockChunkCount()                  
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.singleBlockChunkCount(); 
    }

    /**
     * Ԫ�صĸ���
     *
     * @return size_t
     */
    size_t size()                                           
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.size(); 
    }

    /**
     * ������Ԫ�ظ���
     *
     * @return size_t
     */
    size_t dirtyCount()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.dirtyCount();
    }

	/**
     * Only����Ԫ�ظ���
     *
     * @return size_t
     */
    size_t onlyKeyCount()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.onlyKeyCount();
    }

    /**
     * ����ÿ����̭����
     * @param n
     */
    void setEraseCount(size_t n)                            
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setEraseCount(n); 
    }

    /**
     * ��ȡÿ����̭����
     *
     * @return size_t
     */
    size_t getEraseCount()                                  
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getEraseCount(); 
    }

    /**
     * ����ֻ��
     * @param bReadOnly
     */
    void setReadOnly(bool bReadOnly)                        
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setReadOnly(bReadOnly); 
    }

    /**
     * �Ƿ�ֻ��
     *
     * @return bool
     */
    bool isReadOnly()                                       
    {   
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isReadOnly(); 
    }

    /**
     * �����Ƿ�����Զ���̭
     * @param bAutoErase
     */
    void setAutoErase(bool bAutoErase)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setAutoErase(bAutoErase); 
    }

    /**
     * �Ƿ�����Զ���̭
     *
     * @return bool
     */
    bool isAutoErase()                                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.isAutoErase(); 
    }

    /**
     * ������̭��ʽ
     * TC_RBTree::ERASEBYGET
     * TC_RBTree::ERASEBYSET
     * @param cEraseMode
     */
    void setEraseMode(char cEraseMode)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setEraseMode(cEraseMode); 
    }

    /**
     * ��ȡ��̭��ʽ
     *
     * @return bool
     */
    char getEraseMode()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getEraseMode(); 
    }

    /**
     * ͷ����Ϣ
     * 
     * @return TC_RBTree::tagMapHead
     */
    TC_RBTree::tagMapHead& getMapHead()                    { return this->_t.getMapHead(); }

    /**
     * ���û�дʱ��(��)
     * @param iSyncTime
     */
    void setSyncTime(time_t iSyncTime)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setSyncTime(iSyncTime); 
    }

    /**
     * ��ȡ��дʱ��
     *
     * @return time_t
     */
    time_t getSyncTime()                                    
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getSyncTime(); 
    }

    /**
     * dump���ļ�
     * @param sFile
     * @param bDoClear: �Ƿ����
     * @return int
     *          TC_RBTree::RT_DUMP_FILE_ERR: dump���ļ�����
     *          TC_RBTree::RT_OK: dump���ļ��ɹ�
     */
    int dump2file(const string &sFile, bool bDoClear = false)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        int ret = this->_t.dump2file(sFile);
        if(ret != TC_RBTree::RT_OK)
        {
            return ret;
        }

        if(bDoClear)
           	this->_t.clear();

        return ret;
    }

    /**
     * ���ļ�load
     * @param sFile
     *
     * @return int
     *          TC_RBTree::RT_LOAL_FILE_ERR: load����
     *          TC_RBTree::RT_VERSION_MISMATCH_ERR: �汾��һ��
     *          TC_RBTree::RT_OK: load�ɹ�
     */
    int load5file(const string &sFile)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.load5file(sFile);
    }

    /**
     * ���hash map
     * ����map�е����ݶ������
     */
    void clear()
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.clear();
    }

    /**
     * �������״̬
     * @param k
     *
     * @return int
     *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key
     *          TC_RBTree::RT_DIRTY_DATA: ��������
     *          TC_RBTree::RT_OK: �Ǹɾ�����
     *          ��������ֵ: ����
     */
    int checkDirty(const K &k)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkDirty(sk);
    }

    /**
     * ����Ϊ�ɾ�����i, �޸�SET/GETʱ����, �ᵼ�����ݲ���д
     * @param k
     *
     * @return int
     *          TC_RBTree::RT_READONLY: ֻ��
     *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key
     *          TC_RBTree::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int setClean(const K& k)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setClean(sk);
    }

    /**
     * ����Ϊ������, �޸�SET/GETʱ����, �ᵼ�����ݻ�д
     * @param k
     * @return int
     *          TC_RBTree::RT_READONLY: ֻ��
     *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key
     *          TC_RBTree::RT_OK: ���������ݳɹ�
     *          ��������ֵ: ����
     */
    int setDirty(const K& k)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setDirty(sk);
    }

    /**
     * ��ȡ����, �޸�GETʱ����
     * (���û�����Զ���Get����,û������ʱ����:RT_NO_DATA)
     * @param k
     * @param v
     * @param iSyncTime:�����ϴλ�д��ʱ��, û�л�д��Ϊ0
     *
     * @return int:
     *          TC_RBTree::RT_NO_DATA: û������
     *          TC_RBTree::RT_READONLY: ֻ��ģʽ
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key
     *          TC_RBTree::RT_OK:��ȡ���ݳɹ�
     *          TC_RBTree::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const K& k, V &v, time_t &iSyncTime)
    {
        iSyncTime   = 0;
        int ret = TC_RBTree::RT_OK;

        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        string sv;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(sk, sv, iSyncTime);
        }

        //��ȡ��������, ���
        if(ret == TC_RBTree::RT_OK)
        {
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            v.readFrom(is);

            return ret;
        }

        if(ret != TC_RBTree::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //ֻ��ģʽ
        if(isReadOnly())
        {
            return TC_RBTree::RT_READONLY;
        }

    	//��ȡ����
        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._key   = k;
    	ret = _todo_of->get(stDataRecord);
    	if(ret == TC_RBTree::RT_OK)
    	{
            v = stDataRecord._value;
    		return this->set(stDataRecord._key, stDataRecord._value, stDataRecord._dirty);
    	}
        else if(ret == TC_RBTree::RT_NO_GET)
        {
            return TC_RBTree::RT_NO_DATA;
        }
        else if(ret == TC_RBTree::RT_NO_DATA)
        {
            ret = this->set(stDataRecord._key);
            if(ret == TC_RBTree::RT_OK)
            {
                return TC_RBTree::RT_ONLY_KEY;
            }
            return ret;
        }

    	return TC_RBTree::RT_LOAD_DATA_ERR;
    }

    /**
     * ��ȡ����, �޸�GETʱ����
     * @param k
     * @param v
     *
     * @return int:
     *          TC_RBTree::RT_NO_DATA: û������
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key
     *          TC_RBTree::RT_OK:��ȡ���ݳɹ�
     *          TC_RBTree::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const K& k, V &v)
    {
        time_t iSyncTime;
        return get(k, v, iSyncTime);
    }

    /**
     * ��������, �޸�ʱ����, �ڴ治��ʱ���Զ���̭�ϵ�����
     * @param k: �ؼ���
     * @param v: ֵ
     * @param bDirty: �Ƿ���������
     * @return int:
     *          TC_RBTree::RT_READONLY: mapֻ��
     *          TC_RBTree::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_RBTree::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const K& k, const V& v, bool bDirty = true)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        taf::JceOutputStream<BufferWriter> osv;
        v.writeTo(osv);
        string sv(osv.getBuffer(), osv.getLength());

        int ret = TC_RBTree::RT_OK;
        vector<TC_RBTree::BlockData> vtData;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(sk, sv, bDirty, vtData);
        }

        //������̭����
        if(_todo_of)
        {
            for(size_t i = 0; i < vtData.size(); i++)
            {
                K tk;
                V tv;
                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._key.c_str(), vtData[i]._key.length());
                    tk.readFrom(is);

                    is.setBuffer(vtData[i]._value.c_str(), vtData[i]._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._key       = tk;
                    stDataRecord._value     = tv;
                    stDataRecord._dirty     = vtData[i]._dirty;
                    stDataRecord._iSyncTime = vtData[i]._synct;

                    _todo_of->sync(stDataRecord);
                }
                catch(exception &ex)
                {
                }
            }
        }
        return ret;
    }

    /**
     * ������Key, �ڴ治��ʱ���Զ���̭�ϵ�����
     * @param k: �ؼ���
     * @return int:
     *          TC_RBTree::RT_READONLY: mapֻ��
     *          TC_RBTree::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_RBTree::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const K& k)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        int ret = TC_RBTree::RT_OK;
        vector<TC_RBTree::BlockData> vtData;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(sk, vtData);
        }

        //������̭����
        if(_todo_of)
        {
            for(size_t i = 0; i < vtData.size(); i++)
            {
                K tk;
                V tv;

                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._key.c_str(), vtData[i]._key.length());
                    tk.readFrom(is);

                    is.setBuffer(vtData[i]._value.c_str(), vtData[i]._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._key       = tk;
                    stDataRecord._value     = tv;
                    stDataRecord._dirty     = vtData[i]._dirty;
                    stDataRecord._iSyncTime = vtData[i]._synct;

                    _todo_of->sync(stDataRecord);
                }
                catch(exception &ex)
                {
                }
            }
        }
        return ret;
    }

    /**
     * ɾ������
     * ����cache�Ƿ�������,todo��del��������
     *
     * @param k, �ؼ���
     * @return int:
     *          TC_RBTree::RT_READONLY: mapֻ��
     *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_RBTree::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int del(const K& k)
    {
        int ret = TC_RBTree::RT_OK;

        TC_RBTree::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY && ret != TC_RBTree::RT_NO_DATA)
        {
            return ret;
        }

        if(_todo_of)
        {
            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = k;

            if(ret == TC_RBTree::RT_OK)
            {
                V v;
                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._value.c_str(), data._value.length());
                v.readFrom(is);

                stDataRecord._value     = v;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;
            }

            _todo_of->del((ret == TC_RBTree::RT_OK), stDataRecord);
        }
        return ret;
    }

    /**
     * ɾ������
     * cache������,todo��erase������
     *
     * @param k, �ؼ���
     * @return int:
     *          TC_RBTree::RT_READONLY: mapֻ��
     *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_RBTree::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int erase(const K& k)
    {
        int ret = TC_RBTree::RT_OK;

        TC_RBTree::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_RBTree::RT_OK)
        {
            return ret;
        }

        if(_todo_of)
        {
            V v;
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(data._value.c_str(), data._value.length());
            v.readFrom(is);

            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = k;
            stDataRecord._value     = v;
            stDataRecord._dirty     = data._dirty;
            stDataRecord._iSyncTime = data._synct;

            _todo_of->erase(stDataRecord);
        }
        return ret;
    }

    /**
     * ǿ��ɾ������,������todo��erase������
     *
     * @param k, �ؼ���
     * @return int:
     *          TC_RBTree::RT_READONLY: mapֻ��
     *          TC_RBTree::RT_NO_DATA: û�е�ǰ����
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_RBTree::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int eraseByForce(const K& k)
    {
        int ret = TC_RBTree::RT_OK;

        TC_RBTree::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_RBTree::RT_OK)
        {
            return ret;
        }

        return ret;
    }

    /**
     * ��̭����, ����Getʱ����̭
     * ֱ��: Ԫ�ظ���/chunks * 100 < radio��bCheckDirty Ϊtrueʱ����������������̭����
     * @param radio: �����ڴ�chunksʹ�ñ��� 0< radio < 100
     * @return int:
     *          TC_RBTree::RT_READONLY: mapֻ��
     *          TC_RBTree::RT_OK:��̭���
     */
    int erase(int radio, bool bCheckDirty = false)
    {
        while(true)
        {
            int ret;
            TC_RBTree::BlockData data;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.erase(radio, data, bCheckDirty);
                if(ret == TC_RBTree::RT_OK || ret == TC_RBTree::RT_READONLY)
                {
                    return ret;
                }

                if(ret != TC_RBTree::RT_ERASE_OK)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                K tk;
                V tv;

                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                is.setBuffer(data._value.c_str(), data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = tk;
                stDataRecord._value     = tv;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;

                _todo_of->erase(stDataRecord);
            }
        }
        return TC_RBTree::RT_OK;
    }

    /**
     * ��д������¼, �����¼������, �����κδ���
     * @param k
     * 
     * @return int
     *          TC_RBTree::RT_NO_DATA: û������
     *          TC_RBTree::RT_ONLY_KEY:ֻ��Key
     *          TC_RBTree::RT_OK:��ȡ���ݳɹ�
     *          TC_RBTree::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int sync(const K& k)
    {
        V v;
        time_t iSyncTime;
        int ret = get(k, v, iSyncTime);

        if(ret == TC_RBTree::RT_OK)
        {
            bool bDirty = (checkDirty(k) == TC_RBTree::RT_DIRTY_DATA);

            if(_todo_of)
            {
                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = k;
                stDataRecord._value     = v;
                stDataRecord._dirty     = bDirty;
                stDataRecord._iSyncTime = iSyncTime;

                _todo_of->sync(stDataRecord);
            }
        }

        return ret;
    }

    /**
     * ����������һ��ʱ��û�л�д������ȫ����д
     * ���ݻ�дʱ���뵱ǰʱ�䳬��_pHead->_iSyncTime(setSyncTime)����Ҫ��д
     * 
     * mapֻ��ʱ��Ȼ���Ի�д
     * 
     * @param iNowTime: ��д��ʲôʱ��, ͨ���ǵ�ǰʱ��
     * @return int:
     *      TC_RBTree::RT_OK: ��д�����
     */
    int sync(time_t iNowTime)
    {
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.sync();
        }

        while(true)
        {
            TC_RBTree::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.sync(iNowTime, data);
                if(ret == TC_RBTree::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_RBTree::RT_NEED_SYNC)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                K tk;
                V tv;

                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                is.setBuffer(data._value.c_str(), data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = tk;
                stDataRecord._value     = tv;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;

                _todo_of->sync(stDataRecord);
            }
        }

        return TC_RBTree::RT_OK;
    }

    /**
    *��������βָ�븳����дβָ��
    */
	void sync()
	{
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.sync();
	}

	/**
     * ����������һ��ʱ��û�л�д�����ݻ�д,ֻ��дһ�������ݣ�Ŀ�������int sync(time_t iNowTime)
     * ����������ҵ�����ÿ�λ�д��������ʹ��ʱӦ���ȵ���void sync()
     * 
     * ���ݻ�дʱ���뵱ǰʱ�䳬��_pHead->_iSyncTime(setSyncTime)����Ҫ��д

     * mapֻ��ʱ��Ȼ���Ի�д
     * 
     * @param iNowTime: ��д��ʲôʱ��, ͨ���ǵ�ǰʱ��
     * @return int:
     *      TC_RBTree::RT_OK: ��д�����
     * 
     * ʾ����
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_RBTree::RT_OK )
	 *				break;
	 *		}
     */
    int syncOnce(time_t iNowTime)
    {
		TC_RBTree::BlockData data;

		int ret;
		{
			TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
			ret = this->_t.sync(iNowTime, data);
			if(ret == TC_RBTree::RT_OK)
			{
				return ret;
			}

			if(ret != TC_RBTree::RT_NEED_SYNC)
			{
				return ret;
			}
		}

		if(_todo_of)
		{
			K tk;
			V tv;

			taf::JceInputStream<BufferReader> is;
			is.setBuffer(data._key.c_str(), data._key.length());
			tk.readFrom(is);

			is.setBuffer(data._value.c_str(), data._value.length());
			tv.readFrom(is);

			typename ToDoFunctor::DataRecord stDataRecord;
			stDataRecord._key       = tk;
			stDataRecord._value     = tv;
			stDataRecord._dirty     = data._dirty;
			stDataRecord._iSyncTime = data._synct;

			_todo_of->sync(stDataRecord);
		}

        return ret;
    }
    /**
     * ��������
     * mapֻ��ʱ��Ȼ���Ա���
     * ���Զ���߳�/���̱�������,ͬʱ����ʱbForceFromBegin����ΪfalseЧ�ʸ���
     *
     * @param bForceFromBegin: �Ƿ�ǿ����ͷ��ʼ����, ͨ��Ϊfalse
     * @return int:
     *      TC_RBTree::RT_OK: ����OK��
     */
    int backup(bool bForceFromBegin = false)
    {
        {
            //��ʼ׼������
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.backup(bForceFromBegin);
        }

        while(true)
        {
            TC_RBTree::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.backup(data);
                if(ret == TC_RBTree::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_RBTree::RT_NEED_BACKUP)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                K tk;
                V tv;

                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._key.c_str(), data._key.length());
                tk.readFrom(is);

                is.setBuffer(data._value.c_str(), data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = tk;
                stDataRecord._value     = tv;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;

                _todo_of->backup(stDataRecord);
            }
        }

        return TC_RBTree::RT_OK;
    }

    /**
     * ����
     *
     * @return string
     */
    string desc() { return this->_t.desc(); }

    /////////////////////////////////////////////////////////////////////////////////////////
    // �����Ǳ���map����, ����Ҫ��map�Ӵ������, ���Ǳ���Ч����һ��Ӱ��
    // (ֻ��get�Լ�������++��ʱ�����)
    // ��ȡ�ĵ����������ݲ���֤ʵʱ��Ч,�����Ѿ���ɾ����,��ȡ����ʱ��Ҫ�ж����ݵĺϷ���
    // �õ�����get����ʱ, ����ؼ�鷵��ֵ
    /**
     * β��
     *
     * @return nolock_iterator
     */
    nolock_iterator nolock_end()
    {
        JhmLockPtr jlock;
        return JhmIterator(this->_t.nolock_end(), jlock);
    }

    /**
     * ˳��
     *
     * @return nolock_iterator
     */
    nolock_iterator nolock_begin()
    {
        JhmLockPtr jlock(new JhmLock(this->mutex()));
        return JhmIterator(this->_t.nolock_begin(), jlock);
    }

    /**
     * ����
     *
     * @return nolock_iterator
     */
    nolock_iterator nolock_rbegin()
    {
        JhmLockPtr jlock(new JhmLock(this->mutex()));
        return JhmIterator(this->_t.nolock_rbegin(), jlock);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    // �����Ǳ���map����, ��Ҫ��map�Ӵ������(��������������Ч��Χ��ȫ��������)
    // ��ȡ�������Լ�����������ʵʱ��Ч

    /**
     * β��
     *
     * @return lock_iterator
     */
    lock_iterator end()
    {
        JhmAutoLockPtr jlock;
        return JhmLockIterator(this->_t.end(), jlock);
    }

    /**
     * ˳��
     *
     * @return lock_iterator
     */
    lock_iterator begin()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.begin(), jlock);
    }

    /**
     * ����
     *
     * @return lock_iterator
     */
    lock_iterator rbegin()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbegin(), jlock);
    }

    /**
     * ����(++˳��)
     *
     * @return lock_iterator
     */
    lock_iterator find(const K &k)
    {
        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(sk), jlock);
    }

    /**
     * ����(++����)
     *
     * @return lock_iterator
     */
    lock_iterator rfind(const K &k)
    {
        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rfind(sk), jlock);
    }

    /**
     * ���ز��ҹؼ��ֵ��½�
     * map���Ѿ�������1,2,3,4�Ļ������lower_bound(2)�Ļ������ص�2����upper-bound(2)�Ļ������صľ���3
     * @param k
     * 
     * @return lock_iterator
     */
    lock_iterator lower_bound(const K &k)
    {
        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.lower_bound(sk), jlock);
    }

    /**
     * ���ز��ҹؼ��ֵ��Ͻ�
     * map���Ѿ�������1,2,3,4�Ļ������lower_bound(2)�Ļ������ص�2����upper-bound(2)�Ļ������صľ���3
     * @param k
     * 
     * @return lock_iterator
     */
    lock_iterator upper_bound(const K &k)
    {
        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.upper_bound(sk), jlock);
    }

    /**
     * ˳��
     *
     * @return lock_iterator
     */
    pair<lock_iterator, lock_iterator> equal_range(const K &k1, const K &k2)
    {
        taf::JceOutputStream<BufferWriter> os;
        k1.writeTo(os);
        string sk1(os.getBuffer(), os.getLength());

        os.reset();
        k2.writeTo(os);
        string sk2(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        pair<TC_RBTree::lock_iterator, TC_RBTree::lock_iterator> pit = this->_t.equal_range(sk1, sk2);

        pair<lock_iterator, lock_iterator> p;
        JhmLockIterator it1(pit.first, jlock);
        JhmLockIterator it2(pit.second, jlock);

        p.first  = it1;
        p.second = it2;

        return p;
    }

    /////////////////////////////////////////////////////////////////////////////
    /**
     * ��Setʱ������ĵ�����
     * ���صĵ�����++��ʾ����ʱ��˳��:���Set-->���Set
     *
     * @return lock_iterator
     */
    lock_iterator beginSetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.beginSetTime(), jlock);
    }

    /**
     * Setʱ��������ĵ�����
     *
     * ���صĵ�����++��ʾ����ʱ��˳��:���Set-->���Set
     *
     * @return lock_iterator
     */
    lock_iterator rbeginSetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbeginSetTime(), jlock);
    }

    /**
     * ��Getʱ������ĵ�����
     * ���صĵ�����++��ʾ����ʱ��˳��:���Get-->���Get
     *
     * @return lock_iterator
     */
    lock_iterator beginGetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.beginGetTime(), jlock);
    }

    /**
     * Getʱ��������ĵ�����
     *
     * ���صĵ�����++��ʾ����ʱ��˳��:���Get-->���Get
     *
     * @return lock_iterator
     */
    lock_iterator rbeginGetTime()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbeginGetTime(), jlock);
    }

    /**
     * ��ȡ������β��������(�ʱ��û��Set��������)
     *
     * ���صĵ�����++��ʾ����ʱ��˳��:���Set-->���Set
     * ���ܴ��ڸɾ�����
     *
     * @return lock_iterator
     */
    lock_iterator beginDirty()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.beginDirty(), jlock);
    }

protected:

    /**
     * ɾ�����ݵĺ�������
     */
    ToDoFunctor                 *_todo_of;
};

}

#endif

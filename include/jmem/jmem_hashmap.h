#ifndef _JMEM_HASHMAP_H
#define _JMEM_HASHMAP_H

#include "util/tc_hashmap.h"
#include "util/tc_autoptr.h"
#include "jmem/jmem_policy.h"
#include "jce/Jce.h"

namespace taf
{

/************************************************************************
 ����˵������:
 ����JceЭ����ڴ�hashmap
 �����������׳�JceDecodeException��JceEncodeException
 ���Զ������Ժʹ洢���Խ������, ����:
 �����ź�����, �ļ��洢��hashmap:
 JceHashMap<Test::QueueElement, SemLockPolicy, FileStorePolicy>
 �����ź�����, �����ڴ�洢��hashmap
 JceHashMap<Test::QueueElement, SemLockPolicy, ShmStorePolicy>
 �����߳���, �ڴ�洢��hashmap
 JceHashMap<Test::QueueElement, ThreadLockPolicy, MemStorePolicy>
 
 ʹ����, ��ͬ�����, ��ʼ����������ȫһ��
 ��ʼ��������:
 SemLockPolicy::initLock(key_t)
 ShmStorePolicy::initStore(key_t, size_t)
 FileStorePolicy::initStore(const char *file, size_t)
 ��, ����μ�jmem_policy.h
 
 ***********************************************************************

 ��������˵��:
 > �ڴ����ݵ�map, �������Getʱ���˳����̭����;
 > ֧�ֻ�д/dump���ļ�/���߱���;
 > ֧�ֲ�ͬ��С�ڴ�������, �ṩ�ڴ��ʹ����;
 > ֧�ֻ��յ�ָ�����б��ʵĿռ�;
 > ֧�ֽ�����Key�Ĳ���, ��������value, ֻ��Key, ������stl::set;
 > ֧���Զ���hash�㷨;
 > hash�����Ը����ڴ���������, ���Ż�������, ���hash��ɢ����;
 > ֧�ּ��ַ�ʽ�ı���, ͨ������ʱ��Ҫ��map����; 
 > ����hash��ʽ�ı���, ����ʱ���Բ���Ҫ��map����, �Ƽ�ʹ��;
 > ֧���Զ��������������, ���Էǳ�����ʵ����صĽӿ�;
 > ֧���Զ������, Key��Value�Ľṹ��ͨ��jce2cpp����;
 > jceЭ��֧���Զ���չ�ֶ�, ��˸�hashmap֧���Զ���չ�ֶ�(Key��Value��������ͨ��jce�����);
 > map֧��ֻ��ģʽ, ֻ��ģʽ��set/erase/del���޸����ݵĲ�������ʹ��, get/��д/���߱�������ʹ��
 > ֧���Զ���̭, setʱ, �ڴ������Զ���̭, �ڷ��Զ���̭ʱ, �ڴ���ֱ�ӷ���RT_READONLY
 > ����mmap�ļ�, ֧���Զ���չ�ļ�, ���ڴ治����, �����Զ���չ�ļ���С(ע��hash����������, ��˿�ʼ����Ҫ����hash������), ���Ҳ��ܿ�JHashmap���󣨼�����hashmap�������ͬһ���ļ���֪ͨһ��hashmap��չ�Ժ�����һ�����󲢲�֪����չ�ˣ�

 ***********************************************************************

 hashmap��˵��:
 hashmap��һ�����������¼�������:
 > Setʱ����: �κ�Set���������޸ĸ�����, Set�����ݱ�����Ϊ������, ���ƶ���Set����ͷ��;
 > Getʱ����: �κ�Get���������޸ĸ�����, ��������ֻ��, ע��Set��ͬʱҲ���޸�Get��
 > Dirtyʱ����: dirty����Set����һ����, ���ڻ�д������
 > Backup��:��������Get����һ����, ����������ʱ, ˳��Get����β����ͷ����ʼ����;
 
 ***********************************************************************

 ��ز���˵��:
 > ��������mapֻ��, ������д��������RT_READONLY, ��ʱGet�������޸�����
 > ��������֪���Զ���̭, Ĭ�����Զ���̭��.������Զ���̭,��setʱ,���ڴ�ռ䷵��:RT_NO_MEMORY
 > ���Ը���hash���㷨, ����setHashFunctor����
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
 > ע�⺯������int�ķ���ֵ, �����ر�˵��, ��μ�TC_HashMap::RT_
 
 ***********************************************************************

 ����˵��:
 > ������lock_iterator��map�������¼��ֱ���, �ڱ�����������ʵ��map����������
 > end(): ������β��
 > begin(): ����block�������
 > rbegin():����block�����������
 > beginSetTime(): ����Setʱ��˳�����
 > rbeginSetTime(): ����Setʱ��˳�����
 > beginGetTime(): ����Getʱ��˳�����
 > rbeginGetTime(): ����Getʱ���������
 > beginDirty(): ��ʱ�����������������(���setClean, ��Ҳ��������������)
 > ��ʵ��д�������������������Ӽ�
 > ע��:lock_iteratorһ����ȡ, �Ͷ�map������, ֱ��lock_iterator����Ϊֹ
 >
 > ������hash_iterator��map���б���, ���������ж�mapû�м���, �Ƽ�ʹ��
 > hashBegin(): ��ȡhash����������
 > hashEnd(): hash����β��������
 > ע��:hash_iterator��Ӧ����ʵ��һ��hashͰ��, ÿ�λ�ȡ������ʵ���ȡͰ���������������
*/

template<typename K,
         typename V,
         typename LockPolicy,
         template<class, class> class StorePolicy>
//class JceHashMap : public LockPolicy, public StorePolicy<TC_HashMap>
class JceHashMap : public StorePolicy<TC_HashMap, LockPolicy>
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
         * @return int, ��ȡ������, ����:TC_HashMap::RT_OK
         *              û������,����:TC_HashMap::RT_NO_DATA,
         *              ϵͳĬ��GET,����:TC_HashMap::RT_NO_GET
         *              ����,�򷵻�:TC_HashMap::RT_LOAD_DATA_ERR
         */
        virtual int get(DataRecord &stDataRecord)
        {
            return TC_HashMap::RT_NO_GET;
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
        JhmLockItem(const TC_HashMap::HashMapLockItem &item)
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
         *          TC_HashMap::RT_OK:���ݻ�ȡOK
         *          ����ֵ, �쳣
    	 */
    	int get(K& k)
        {
            string sk;
            int ret = _item.get(sk);
            if(ret != TC_HashMap::RT_OK)
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
         *          TC_HashMap::RT_OK:���ݻ�ȡOK
         *          TC_HashMap::RT_ONLY_KEY: key��Ч, v��ЧΪ��
         *          ����ֵ, �쳣
    	 */
    	int get(K& k, V& v)
        {
            string sk;
            string sv;
            int ret = _item.get(sk, sv);
            if(ret != TC_HashMap::RT_OK && ret != TC_HashMap::RT_ONLY_KEY)
            {
                return ret;
            }

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sk.c_str(), sk.length());
            k.readFrom(is);

            if(ret != TC_HashMap::RT_ONLY_KEY)
            {
                is.setBuffer(sv.c_str(), sv.length());
                v.readFrom(is);
            }

            return ret;
        }

    protected:
        TC_HashMap::HashMapLockItem _item;
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
        JhmLockIterator(const TC_HashMap::lock_iterator it, const JhmAutoLockPtr &lock)
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
        TC_HashMap::lock_iterator  _it;

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
        JhmItem(const TC_HashMap::HashMapItem &item, const JhmLockPtr &lock)
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
         * ��ȡ��ǰhashͰ����������, ע��ֻ��ȡ��key/value������
         * ����ֻ��key������, ����ȡ
         * ���Э�����������Ҳ����ȡ
         * @param
    	 */
    	void get(vector<pair<K, V> > &v)
        {
            vector<TC_HashMap::BlockData> vtData;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                _item.get(vtData);
            }

            for(size_t i = 0; i < vtData.size(); i++)
            {
                pair<K, V> pk;

                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._key.c_str(), vtData[i]._key.length());
                    pk.first.readFrom(is);

                    is.setBuffer(vtData[i]._value.c_str(), vtData[i]._value.length());
                    pk.second.readFrom(is);

                    v.push_back(pk);
                }
                catch(exception &ex)
                {
                }
            }
        }

    protected:
        TC_HashMap::HashMapItem _item;
        JhmLockPtr              _lock;
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
        JhmIterator(const TC_HashMap::hash_iterator &it, const JhmLockPtr &lock)
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
        TC_HashMap::hash_iterator  _it;

        /**
         * ������
         */
        JhmItem               _item;

        /**
         * ��
         */
        JhmLockPtr            _lock;
    };

    typedef JhmIterator hash_iterator ;

    ////////////////////////////////////////////////////////////////////////////
    //
    /**
     * ���캯��
     */
    JceHashMap()
    {
        _todo_of = NULL;
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
     * ����hash����(����chunk���ݿ�/hash���ֵ, Ĭ����2)
     * ����Ҫ���ı�����create֮ǰ����
     *
     * @param fRadio
     */
    void initHashRadio(float fRadio)                       { this->_t.initHashRadio(fRadio);}
    
    /**
     * ����hash��ʽ
     * @param hash_of
     */
    void setHashFunctor(TC_HashMap::hash_functor hashf)     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctor(hashf); 
    }

    /**
     * ��ȡhash��ʽ
     * 
     * @return TC_HashMap::hash_functor&
     */
    TC_HashMap::hash_functor &getHashFunctor()              { return this->_t.getHashFunctor(); }

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
     * ��ȡhashͰ�ĸ���
     *
     * @return size_t
     */
    size_t getHashCount()                                   
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getHashCount(); 
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
     * TC_HashMap::ERASEBYGET
     * TC_HashMap::ERASEBYSET
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
     * @return TC_HashMap::tagMapHead
     */
    TC_HashMap::tagMapHead& getMapHead()                    { return this->_t.getMapHead(); }

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
     *          TC_HashMap::RT_DUMP_FILE_ERR: dump���ļ�����
     *          TC_HashMap::RT_OK: dump���ļ��ɹ�
     */
    int dump2file(const string &sFile, bool bDoClear = false)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        int ret = this->_t.dump2file(sFile);
        if(ret != TC_HashMap::RT_OK)
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
     *          TC_HashMap::RT_LOAL_FILE_ERR: load����
     *          TC_HashMap::RT_VERSION_MISMATCH_ERR: �汾��һ��
     *          TC_HashMap::RT_OK: load�ɹ�
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
     *          TC_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMap::RT_DIRTY_DATA: ��������
     *          TC_HashMap::RT_OK: �Ǹɾ�����
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
     *          TC_HashMap::RT_READONLY: ֻ��
     *          TC_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMap::RT_OK: ���óɹ�
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
     *          TC_HashMap::RT_READONLY: ֻ��
     *          TC_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMap::RT_OK: ���������ݳɹ�
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
     *          TC_HashMap::RT_NO_DATA: û������
     *          TC_HashMap::RT_READONLY: ֻ��ģʽ
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMap::RT_OK:��ȡ���ݳɹ�
     *          TC_HashMap::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const K& k, V &v, time_t &iSyncTime)
    {
        iSyncTime   = 0;
        int ret = TC_HashMap::RT_OK;

        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        string sv;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(sk, sv, iSyncTime);
        }

        //��ȡ��������, ���
        if(ret == TC_HashMap::RT_OK)
        {
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            v.readFrom(is);

            return ret;
        }

        if(ret != TC_HashMap::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //ֻ��ģʽ
        if(isReadOnly())
        {
            return TC_HashMap::RT_READONLY;
        }

    	//��ȡ����
        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._key   = k;
    	ret = _todo_of->get(stDataRecord);
    	if(ret == TC_HashMap::RT_OK)
    	{
            v = stDataRecord._value;
    		return this->set(stDataRecord._key, stDataRecord._value, stDataRecord._dirty);
    	}
        else if(ret == TC_HashMap::RT_NO_GET)
        {
            return TC_HashMap::RT_NO_DATA;
        }
        else if(ret == TC_HashMap::RT_NO_DATA)
        {
            ret = this->set(stDataRecord._key);
            if(ret == TC_HashMap::RT_OK)
            {
                return TC_HashMap::RT_ONLY_KEY;
            }
            return ret;
        }

    	return TC_HashMap::RT_LOAD_DATA_ERR;
    }

    /**
     * ��ȡ����, �޸�GETʱ����
     * @param k
     * @param v
     *
     * @return int:
     *          TC_HashMap::RT_NO_DATA: û������
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMap::RT_OK:��ȡ���ݳɹ�
     *          TC_HashMap::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const K& k, V &v)
    {
        time_t iSyncTime;
        return get(k, v, iSyncTime);
    }

    /**
     * ����key, ��ȡ��ͬhashֵ����������
     * ע��:cƥ����������, map�Ǽ�����, ��Ҫע��
     * @param h
     * @param vv
     * @param c, ƥ��º���: bool operator()(K v);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHash(size_t h, vector<pair<K, V> > &vv, C c)
    {
        int ret = TC_HashMap::RT_OK;

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        this->_t.doUpdate();
        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;

        TC_HashMap::Block block(&this->_t, iAddr);

        while(block.getHead() != 0)
        {
            TC_HashMap::BlockData data;
            ret = block.getBlockData(data);
            if(ret == TC_HashMap::RT_OK)
            {
                K tk;
                V tv;

                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(data._key.c_str(), data._key.length());
                    tk.readFrom(is);

                    if(c(tk))
                    {
                        is.setBuffer(data._value.c_str(), data._value.length());
                        tv.readFrom(is);
                        vv.push_back(make_pair(tk, tv));
                    }
                }
                catch(exception &ex)
                {
                }
            }
            if(!block.nextBlock())
            {
                break;
            }
        }

        return TC_HashMap::RT_OK;
    }

    /**
     * �ָ�����
     * ����block��¼�޷���ȡ�������Զ�ɾ��
     * @param bRepair: �Ƿ��޸�
     * @return ����ɾ���ļ�¼��
     */
    size_t recover(bool bRepair)
    {
        size_t c     = this->_t.getHashCount();
        size_t e     = 0;
        for(size_t i = 0; i < c; i++)
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

            e += this->_t.recover(i, bRepair);
        }

        return e;
    }

    /**
     * ��������, �޸�ʱ����, �ڴ治��ʱ���Զ���̭�ϵ�����
     * @param k: �ؼ���
     * @param v: ֵ
     * @param bDirty: �Ƿ���������
     * @return int:
     *          TC_HashMap::RT_READONLY: mapֻ��
     *          TC_HashMap::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_HashMap::RT_OK: ���óɹ�
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

        int ret = TC_HashMap::RT_OK;
        vector<TC_HashMap::BlockData> vtData;

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
     *          TC_HashMap::RT_READONLY: mapֻ��
     *          TC_HashMap::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_HashMap::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const K& k)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        int ret = TC_HashMap::RT_OK;
        vector<TC_HashMap::BlockData> vtData;

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
     *          TC_HashMap::RT_READONLY: mapֻ��
     *          TC_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int del(const K& k)
    {
        int ret = TC_HashMap::RT_OK;

        TC_HashMap::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMap::RT_OK && ret != TC_HashMap::RT_ONLY_KEY && ret != TC_HashMap::RT_NO_DATA)
        {
            return ret;
        }

        if(_todo_of)
        {
            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = k;

            if(ret == TC_HashMap::RT_OK)
            {
                V v;
                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._value.c_str(), data._value.length());
                v.readFrom(is);

                stDataRecord._value     = v;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;
            }

            _todo_of->del((ret == TC_HashMap::RT_OK), stDataRecord);
        }
        return ret;
    }

    /**
     * ɾ������
     * cache������,todo��erase������
     *
     * @param k, �ؼ���
     * @return int:
     *          TC_HashMap::RT_READONLY: mapֻ��
     *          TC_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int erase(const K& k)
    {
        int ret = TC_HashMap::RT_OK;

        TC_HashMap::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMap::RT_OK)
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
     *          TC_HashMap::RT_READONLY: mapֻ��
     *          TC_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int eraseByForce(const K& k)
    {
        int ret = TC_HashMap::RT_OK;

        TC_HashMap::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMap::RT_OK)
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
     *          TC_HashMap::RT_READONLY: mapֻ��
     *          TC_HashMap::RT_OK:��̭���
     */
    int erase(int radio, bool bCheckDirty = false)
    {
        while(true)
        {
            int ret;
            TC_HashMap::BlockData data;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.erase(radio, data, bCheckDirty);
                if(ret == TC_HashMap::RT_OK || ret == TC_HashMap::RT_READONLY)
                {
                    return ret;
                }

                if(ret != TC_HashMap::RT_ERASE_OK)
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
        return TC_HashMap::RT_OK;
    }

    /**
     * ��д������¼, �����¼������, �����κδ���
     * @param k
     * 
     * @return int
     *          TC_HashMap::RT_NO_DATA: û������
     *          TC_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMap::RT_OK:��ȡ���ݳɹ�
     *          TC_HashMap::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int sync(const K& k)
    {
        V v;
        time_t iSyncTime;
        int ret = get(k, v, iSyncTime);

        if(ret == TC_HashMap::RT_OK)
        {
            bool bDirty = (checkDirty(k) == TC_HashMap::RT_DIRTY_DATA);

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
     *      TC_HashMap::RT_OK: ��д�����
     */
    int sync(time_t iNowTime)
    {
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.sync();
        }

        while(true)
        {
            TC_HashMap::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.sync(iNowTime, data);
                if(ret == TC_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_HashMap::RT_NEED_SYNC)
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

        return TC_HashMap::RT_OK;
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
     *      TC_HashMap::RT_OK: ��д�����
     * 
     * ʾ����
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_HashMap::RT_OK )
	 *				break;
	 *		}
     */
    int syncOnce(time_t iNowTime)
    {
        

		TC_HashMap::BlockData data;

		int ret;
		{
			TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
			ret = this->_t.sync(iNowTime, data);
			if(ret == TC_HashMap::RT_OK)
			{
				return ret;
			}

			if(ret != TC_HashMap::RT_NEED_SYNC)
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
     *      TC_HashMap::RT_OK: ����OK��
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
            TC_HashMap::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.backup(data);
                if(ret == TC_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_HashMap::RT_NEED_BACKUP)
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

        return TC_HashMap::RT_OK;
    }

    /**
     * ����
     *
     * @return string
     */
    string desc() { return this->_t.desc(); }

    ///////////////////////////////////////////////////////////////////////////////
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
     * ����Key��������
     * @param k
     */
    lock_iterator find(const K& k)
    {
        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(sk), jlock);
    }

    /**
     * block����
     *
     * @return lock_iterator
     */
    lock_iterator begin()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.begin(), jlock);
    }

    /**
     * block����
     *
     * @return lock_iterator
     */
    lock_iterator rbegin()
    {
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.rbegin(), jlock);
    }

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

    /////////////////////////////////////////////////////////////////////////////////////////
    // �����Ǳ���map����, ����Ҫ��map����

    /**
     * ����hashͰ����
     * 
     * @return hash_iterator
     */
    hash_iterator hashBegin()
    {
        JhmLockPtr jlock(new JhmLock(this->mutex()));
        return JhmIterator(this->_t.hashBegin(), jlock);
    }

    /**
     * ����
     *
     * @return
     */
    hash_iterator hashEnd() 
    { 
        JhmLockPtr jlock;
        return JhmIterator(this->_t.hashEnd(), jlock); 
    }

protected:

    /**
     * ɾ�����ݵĺ�������
     */
    ToDoFunctor                 *_todo_of;
};

}

#endif

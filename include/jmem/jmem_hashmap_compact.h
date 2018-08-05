#ifndef _JMEM_HASHMAP_COMPACT_H
#define _JMEM_HASHMAP_COMPACT_H

#include "util/tc_hashmap_compact.h"
#include "util/tc_autoptr.h"
#include "jmem/jmem_policy.h"
#include "jce/Jce.h"

namespace taf
{
/************************************************************************
��Ҫ˵��:
1 ʹ�÷�ʽ��JceHashmap��ȫһ��;
2 ��JHashmap���, ��64λos����,ÿ��chunk�ܹ���Լ���30���ֽ�;
3 Ч�ʱ�JHashmap����һ��,���Ժ��Բ���;
4 ÿ��chunk�Ĵ�С���ֻ����64k(��ʵ�����ݿ��С�޹�,ʵ�����ݿ���Զ��chunkƴ����һ��)
************************************************************************/

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
 > ע�⺯������int�ķ���ֵ, �����ر�˵��, ��μ�TC_HashMapCompact::RT_
 
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
//class JceHashMap : public LockPolicy, public StorePolicy<TC_HashMapCompact>
class JceHashMapCompact : public StorePolicy<TC_HashMapCompact, LockPolicy>
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
            uint32_t  _iSyncTime;
			uint32_t _expiret;
			uint8_t	_ver;


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
         * @return int, ��ȡ������, ����:TC_HashMapCompact::RT_OK
         *              û������,����:TC_HashMapCompact::RT_NO_DATA,
         *              ϵͳĬ��GET,����:TC_HashMapCompact::RT_NO_GET
         *              ����,�򷵻�:TC_HashMapCompact::RT_LOAD_DATA_ERR
         */
        virtual int get(DataRecord &stDataRecord)
        {
            return TC_HashMapCompact::RT_NO_GET;
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
        JhmLockItem(const TC_HashMapCompact::HashMapLockItem &item)
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
         * @return uint32_t
         */
        uint32_t getSyncTime()    { return _item.getSyncTime(); }

    	/**
    	 * ��ȡֵ
         * @return int
         *          TC_HashMapCompact::RT_OK:���ݻ�ȡOK
         *          ����ֵ, �쳣
    	 */
    	int get(K& k)
        {
            string sk;
            int ret = _item.get(sk);
            if(ret != TC_HashMapCompact::RT_OK)
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
         *          TC_HashMapCompact::RT_OK:���ݻ�ȡOK
         *          TC_HashMapCompact::RT_ONLY_KEY: key��Ч, v��ЧΪ��
         *          ����ֵ, �쳣
    	 */
    	int get(K& k, V& v)
        {
            string sk;
            string sv;
            int ret = _item.get(sk, sv);
            if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
            {
                return ret;
            }

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sk.c_str(), sk.length());
            k.readFrom(is);

            if(ret != TC_HashMapCompact::RT_ONLY_KEY)
            {
                is.setBuffer(sv.c_str(), sv.length());
                v.readFrom(is);
            }

            return ret;
        }

    protected:
        TC_HashMapCompact::HashMapLockItem _item;
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
        JhmLockIterator(const TC_HashMapCompact::lock_iterator it, const JhmAutoLockPtr &lock)
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
        TC_HashMapCompact::lock_iterator  _it;

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
        JhmItem(const TC_HashMapCompact::HashMapItem &item, const JhmLockPtr &lock)
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
            vector<TC_HashMapCompact::BlockData> vtData;

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

		/**
         * ��ȡ��ǰhashͰ�����й�������, ע��ֻ��ȡ��key/value������
         * ����ֻ��key������, ����ȡ
         * ���Э�����������Ҳ����ȡ
         * @param
    	 */
    	void getExpire(uint32_t t, vector<pair<K, V> > &v)
        {
            vector<TC_HashMapCompact::BlockData> vtData;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                _item.getExpire(t, vtData);
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
        TC_HashMapCompact::HashMapItem _item;
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
        JhmIterator(const TC_HashMapCompact::hash_iterator &it, const JhmLockPtr &lock)
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
        TC_HashMapCompact::hash_iterator  _it;

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
    JceHashMapCompact()
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
    void setHashFunctor(TC_HashMapCompact::hash_functor hashf)     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctor(hashf); 
    }

    /**
     * ��ȡhash��ʽ
     * 
     * @return TC_HashMapCompact::hash_functor&
     */
    TC_HashMapCompact::hash_functor &getHashFunctor()              { return this->_t.getHashFunctor(); }

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
     * TC_HashMapCompact::ERASEBYGET
     * TC_HashMapCompact::ERASEBYSET
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
     * @return TC_HashMapCompact::tagMapHead
     */
    TC_HashMapCompact::tagMapHead& getMapHead()
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getMapHead(); 
    }

    /**
     * ���û�дʱ��(��)
     * @param iSyncTime
     */
    void setSyncTime(uint32_t iSyncTime)                      
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setSyncTime(iSyncTime); 
    }

    /**
     * ��ȡ��дʱ��
     *
     * @return uint32_t
     */
    uint32_t getSyncTime()                                    
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getSyncTime(); 
    }

    /**
     * dump���ļ�
     * @param sFile
     * @param bDoClear: �Ƿ����
     * @return int
     *          TC_HashMapCompact::RT_DUMP_FILE_ERR: dump���ļ�����
     *          TC_HashMapCompact::RT_OK: dump���ļ��ɹ�
     */
    int dump2file(const string &sFile, bool bDoClear = false)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        int ret = this->_t.dump2file(sFile);
        if(ret != TC_HashMapCompact::RT_OK)
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
     *          TC_HashMapCompact::RT_LOAL_FILE_ERR: load����
     *          TC_HashMapCompact::RT_VERSION_MISMATCH_ERR: �汾��һ��
     *          TC_HashMapCompact::RT_OK: load�ɹ�
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
     *          TC_HashMapCompact::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMapCompact::RT_DIRTY_DATA: ��������
     *          TC_HashMapCompact::RT_OK: �Ǹɾ�����
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
     *          TC_HashMapCompact::RT_READONLY: ֻ��
     *          TC_HashMapCompact::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMapCompact::RT_OK: ���óɹ�
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
     *          TC_HashMapCompact::RT_READONLY: ֻ��
     *          TC_HashMapCompact::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMapCompact::RT_OK: ���������ݳɹ�
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
     *          TC_HashMapCompact::RT_NO_DATA: û������
     *          TC_HashMapCompact::RT_READONLY: ֻ��ģʽ
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMapCompact::RT_OK:��ȡ���ݳɹ�
     *          TC_HashMapCompact::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const K& k, V &v, uint32_t &iSyncTime, uint32_t& iExpireTime, uint8_t& iVersion)
    {
        iSyncTime   = 0;
		iExpireTime = 0;
		iVersion	= 1;
        int ret = TC_HashMapCompact::RT_OK;

        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        string sv;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(sk, sv, iSyncTime, iExpireTime, iVersion);
        }

        //��ȡ��������, ���
        if(ret == TC_HashMapCompact::RT_OK)
        {
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            v.readFrom(is);

            return ret;
        }

        if(ret != TC_HashMapCompact::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //ֻ��ģʽ
        if(isReadOnly())
        {
            return TC_HashMapCompact::RT_READONLY;
        }

    	//��ȡ����
        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._key   = k;
    	ret = _todo_of->get(stDataRecord);
    	if(ret == TC_HashMapCompact::RT_OK)
    	{
            v = stDataRecord._value;
			iExpireTime = stDataRecord._expiret;
    		return this->set(stDataRecord._key, stDataRecord._value, stDataRecord._expiret, stDataRecord._ver, stDataRecord._dirty);
    	}
        else if(ret == TC_HashMapCompact::RT_NO_GET)
        {
            return TC_HashMapCompact::RT_NO_DATA;
        }
        else if(ret == TC_HashMapCompact::RT_NO_DATA)
        {
			iExpireTime = stDataRecord._expiret;
            ret = this->set(stDataRecord._key);
            if(ret == TC_HashMapCompact::RT_OK)
            {
                return TC_HashMapCompact::RT_ONLY_KEY;
            }
            return ret;
        }

    	return TC_HashMapCompact::RT_LOAD_DATA_ERR;
    }

    /**
     * ��ȡ����, �޸�GETʱ����
     * @param k
     * @param v
     *
     * @return int:
     *          TC_HashMapCompact::RT_NO_DATA: û������
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMapCompact::RT_OK:��ȡ���ݳɹ�
     *          TC_HashMapCompact::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const K& k, V &v)
    {
        uint32_t iSyncTime;
		uint32_t iExpireTime;
		uint8_t iVer;
        return get(k, v, iSyncTime, iExpireTime, iVer);
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
        int ret = TC_HashMapCompact::RT_OK;

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        TC_HashMapCompact::FailureRecover check(&this->_t);

        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;

        TC_HashMapCompact::Block block(&this->_t, iAddr);

        while(block.getHead() != 0)
        {
            TC_HashMapCompact::BlockData data;
            ret = block.getBlockData(data);
            if(ret == TC_HashMapCompact::RT_OK)
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

        return TC_HashMapCompact::RT_OK;
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
     *          TC_HashMapCompact::RT_READONLY: mapֻ��
     *          TC_HashMapCompact::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_HashMapCompact::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const K& k, const V& v, bool bDirty = true, uint32_t iExpireTime = 0, uint8_t iVersion = 0)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        taf::JceOutputStream<BufferWriter> osv;
        v.writeTo(osv);
        string sv(osv.getBuffer(), osv.getLength());

        int ret = TC_HashMapCompact::RT_OK;
        vector<TC_HashMapCompact::BlockData> vtData;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(sk, sv, iExpireTime, iVersion, bDirty, vtData);
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
					stDataRecord._expiret	= vtData[i]._expiret;
					stDataRecord._ver		= vtData[i]._ver;

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
     *          TC_HashMapCompact::RT_READONLY: mapֻ��
     *          TC_HashMapCompact::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_HashMapCompact::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const K& k, uint8_t iVersion = 0)
    {
        taf::JceOutputStream<BufferWriter> osk;
        k.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());

        int ret = TC_HashMapCompact::RT_OK;
        vector<TC_HashMapCompact::BlockData> vtData;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(sk, iVersion, vtData);
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
					stDataRecord._expiret	= vtData[i]._expiret;
					stDataRecord._ver		= vtData[i]._ver;

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
     *          TC_HashMapCompact::RT_READONLY: mapֻ��
     *          TC_HashMapCompact::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_HashMapCompact::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int del(const K& k)
    {
        int ret = TC_HashMapCompact::RT_OK;

        TC_HashMapCompact::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY && ret != TC_HashMapCompact::RT_NO_DATA)
        {
            return ret;
        }

        if(_todo_of)
        {
            typename ToDoFunctor::DataRecord stDataRecord;
            stDataRecord._key       = k;

            if(ret == TC_HashMapCompact::RT_OK)
            {
                V v;
                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._value.c_str(), data._value.length());
                v.readFrom(is);

                stDataRecord._value     = v;
                stDataRecord._dirty     = data._dirty;
                stDataRecord._iSyncTime = data._synct;
				stDataRecord._expiret	= data._expiret;
				stDataRecord._ver		= data._ver;
            }

            _todo_of->del((ret == TC_HashMapCompact::RT_OK), stDataRecord);
        }
        return ret;
    }

    /**
     * ɾ������
     * cache������,todo��erase������
     *
     * @param k, �ؼ���
     * @return int:
     *          TC_HashMapCompact::RT_READONLY: mapֻ��
     *          TC_HashMapCompact::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_HashMapCompact::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int erase(const K& k)
    {
        int ret = TC_HashMapCompact::RT_OK;

        TC_HashMapCompact::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMapCompact::RT_OK)
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
			stDataRecord._expiret	= data._expiret;
			stDataRecord._ver		= data._ver;

            _todo_of->erase(stDataRecord);
        }
        return ret;
    }

    /**
     * ǿ��ɾ������,������todo��erase������
     *
     * @param k, �ؼ���
     * @return int:
     *          TC_HashMapCompact::RT_READONLY: mapֻ��
     *          TC_HashMapCompact::RT_NO_DATA: û�е�ǰ����
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_HashMapCompact::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int eraseByForce(const K& k)
    {
        int ret = TC_HashMapCompact::RT_OK;

        TC_HashMapCompact::BlockData data;

        taf::JceOutputStream<BufferWriter> os;
        k.writeTo(os);
        string sk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(sk, data);
        }

        if(ret != TC_HashMapCompact::RT_OK)
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
     *          TC_HashMapCompact::RT_READONLY: mapֻ��
     *          TC_HashMapCompact::RT_OK:��̭���
     */
    int erase(int radio, bool bCheckDirty = false)
    {
        while(true)
        {
            int ret;
            TC_HashMapCompact::BlockData data;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.erase(radio, data, bCheckDirty);
                if(ret == TC_HashMapCompact::RT_OK || ret == TC_HashMapCompact::RT_READONLY)
                {
                    return ret;
                }

                if(ret != TC_HashMapCompact::RT_ERASE_OK)
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
				stDataRecord._expiret	= data._expiret;
				stDataRecord._ver		= data._ver;

                _todo_of->erase(stDataRecord);
            }
        }
        return TC_HashMapCompact::RT_OK;
    }

    /**
     * ��д������¼, �����¼������, �����κδ���
     * @param k
     * 
     * @return int
     *          TC_HashMapCompact::RT_NO_DATA: û������
     *          TC_HashMapCompact::RT_ONLY_KEY:ֻ��Key
     *          TC_HashMapCompact::RT_OK:��ȡ���ݳɹ�
     *          TC_HashMapCompact::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int sync(const K& k)
    {
        V v;
		uint32_t iSyncTime;
		uint32_t iExpireTime;
		uint8_t iVersion;
        int ret = get(k, v, iSyncTime, iExpireTime, iVersion);

        if(ret == TC_HashMapCompact::RT_OK)
        {
            bool bDirty = (checkDirty(k) == TC_HashMapCompact::RT_DIRTY_DATA);

            if(_todo_of)
            {
                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._key       = k;
                stDataRecord._value     = v;
                stDataRecord._dirty     = bDirty;
                stDataRecord._iSyncTime = iSyncTime;
				stDataRecord._expiret	= iExpireTime;
				stDataRecord._ver		= iVersion;

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
     *      TC_HashMapCompact::RT_OK: ��д�����
     */
    int sync(uint32_t iNowTime)
    {
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.sync();
        }

        while(true)
        {
            TC_HashMapCompact::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.sync(iNowTime, data);
                if(ret == TC_HashMapCompact::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_HashMapCompact::RT_NEED_SYNC)
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
				stDataRecord._expiret	= data._expiret;
				stDataRecord._ver		= data._ver;

                _todo_of->sync(stDataRecord);
            }
        }

        return TC_HashMapCompact::RT_OK;
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
     * ����������һ��ʱ��û�л�д�����ݻ�д,ֻ��дһ�������ݣ�Ŀ�������int sync(uint32_t iNowTime)
     * ����������ҵ�����ÿ�λ�д��������ʹ��ʱӦ���ȵ���void sync()
     * 
     * ���ݻ�дʱ���뵱ǰʱ�䳬��_pHead->_iSyncTime(setSyncTime)����Ҫ��д

     * mapֻ��ʱ��Ȼ���Ի�д
     * 
     * @param iNowTime: ��д��ʲôʱ��, ͨ���ǵ�ǰʱ��
     * @return int:
     *      TC_HashMapCompact::RT_OK: ��д�����
     * 
     * ʾ����
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_HashMapCompact::RT_OK )
	 *				break;
	 *		}
     */
    int syncOnce(uint32_t iNowTime)
    {
        

		TC_HashMapCompact::BlockData data;

		int ret;
		{
			TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
			ret = this->_t.sync(iNowTime, data);
			if(ret == TC_HashMapCompact::RT_OK)
			{
				return ret;
			}

			if(ret != TC_HashMapCompact::RT_NEED_SYNC)
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
			stDataRecord._expiret	= data._expiret;
			stDataRecord._ver		= data._ver;

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
     *      TC_HashMapCompact::RT_OK: ����OK��
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
            TC_HashMapCompact::BlockData data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.backup(data);
                if(ret == TC_HashMapCompact::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_HashMapCompact::RT_NEED_BACKUP)
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
				stDataRecord._expiret	= data._expiret;
				stDataRecord._ver		= data._ver;

                _todo_of->backup(stDataRecord);
            }
        }

        return TC_HashMapCompact::RT_OK;
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

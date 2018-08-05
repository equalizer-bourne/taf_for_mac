#ifndef _JMEM_MULTI_HASHMAP_H
#define _JMEM_MULTI_HASHMAP_H

#include "util/tc_multi_hashmap.h"
#include "util/tc_autoptr.h"
#include "jmem/jmem_policy.h"
#include "jce/Jce.h"

namespace taf
{

/************************************************************************
 ����˵������:
 ����JceЭ���֧�ֶ�key���ڴ�hashmap
 �����������׳�JceDecodeException��JceEncodeException
 ���Զ������Ժʹ洢���Խ������, ����:
 �����ź�����, �ļ��洢�Ķ�key hashmap:
 JceMultiHashMap<Test::QueueElement, SemLockPolicy, FileStorePolicy>
 �����ź�����, �����ڴ�洢�Ķ�key hashmap
 JceMultiHashMap<Test::QueueElement, SemLockPolicy, ShmStorePolicy>
 �����߳���, �ڴ�洢�Ķ�key hashmap
 JceMultiHashMap<Test::QueueElement, ThreadLockPolicy, MemStorePolicy>
 
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

 > erase(int ratio), ������̭����, ֱ�����п���ʵ���ratio;
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
 > ע�⺯������int�ķ���ֵ, �����ر�˵��, ��μ�TC_Multi_HashMap::RT_
 
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

template<typename MK,
		 typename UK,
         typename V,
         typename LockPolicy,
         template<class, class> class StorePolicy>
//class JceMultiHashMap : public LockPolicy, public StorePolicy<TC_Multi_HashMap>
class JceMultiHashMap : public StorePolicy<TC_Multi_HashMap, LockPolicy>
{
public:
	/**
	* �������ݽṹ
	*/
	struct Value
	{
		MK				_mkey;
		UK				_ukey;
		V				_value;
		bool			_dirty;
		uint8_t			_iVersion;	// ȡֵ��ΧΪ1-255��0Ϊ����ֵ����ʾ�����汾��ѭ��ʹ��
		time_t			_iSyncTime;
		Value() : _dirty(true), _iVersion(1), _iSyncTime(0)
		{
		}
	};
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
        typedef Value DataRecord;	// �����ϰ汾������

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
         * @return int, ��ȡ������, ����:TC_Multi_HashMap::RT_OK
         *              û������,����:TC_Multi_HashMap::RT_NO_DATA,
         *              ϵͳĬ��GET,����:TC_Multi_HashMap::RT_NO_GET
         *              ����,�򷵻�:TC_Multi_HashMap::RT_LOAD_DATA_ERR
         */
        virtual int get(DataRecord &stDataRecord)
        {
            return TC_Multi_HashMap::RT_NO_GET;
        }

		/**
		* ������key��ȡ���ݣ�Ĭ�Ϸ���RT_NO_GET
		* mk����Ҫ��ȡ���ݵ���key
		* vtRecords�����ص�����
		*/
		virtual int get(MK mk, vector<DataRecord>& vtRecords)
		{
			return TC_Multi_HashMap::RT_NO_GET;
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
        JhmLockItem(const TC_Multi_HashMap::HashMapLockItem &item)
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
    	 * ��ȡ��ǰitem��key
		 * @param mk, ��key
		 * @param uk, ����key�����������
         * @return int
         *          TC_Multi_HashMap::RT_OK:���ݻ�ȡOK
         *          ����ֵ, �쳣
    	 */
    	int get(MK &mk, UK &uk)
        {
            string smk, suk;
            int ret = _item.get(smk, suk);
            if(ret != TC_Multi_HashMap::RT_OK)
            {
                return ret;
            }

            taf::JceInputStream<BufferReader> is;
			is.setBuffer(smk.c_str(), smk.length());
			mk.readFrom(is);
            is.setBuffer(suk.c_str(), suk.length());
            uk.readFrom(is);

            return ret;
        }

    	/**
    	 * ��ȡֵ��ǰitem��value(��key)
		 * @param v
         * @return int
         *          TC_Multi_HashMap::RT_OK:���ݻ�ȡOK
         *          TC_Multi_HashMap::RT_ONLY_KEY: key��Ч, v��ЧΪ��
         *          ����ֵ, �쳣
    	 */
    	int get(Value &v)
        {
			TC_Multi_HashMap::Value hv;
            int ret = _item.get(hv);
            if(ret != TC_Multi_HashMap::RT_OK && ret != TC_Multi_HashMap::RT_ONLY_KEY)
            {
                return ret;
            }

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(hv._mkey.c_str(), hv._mkey.length());
			v._mkey.readFrom(is);
			is.setBuffer(hv._data._key.c_str(), hv._data._key.length());
            v._ukey.readFrom(is);
			if(ret != TC_Multi_HashMap::RT_ONLY_KEY)
			{
				is.setBuffer(hv._data._value.c_str(), hv._data._value.length());
				v._value.readFrom(is);
			}

			v._dirty = hv._data._dirty;
			v._iVersion = hv._data._iVersion;
			v._iSyncTime = hv._data._synct;

            return ret;
        }

    protected:
        TC_Multi_HashMap::HashMapLockItem _item;
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
        JhmLockIterator(const TC_Multi_HashMap::lock_iterator it, const JhmAutoLockPtr &lock)
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
        TC_Multi_HashMap::lock_iterator  _it;

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
        JhmItem(const TC_Multi_HashMap::HashMapItem &item, const JhmLockPtr &lock)
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
         * @param vs,
    	 */
		void get(vector<JceMultiHashMap::Value> &vs)
        {
            vector<TC_Multi_HashMap::Value> vtData;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(_lock->mutex());
                _item.get(vtData);
            }

            for(size_t i = 0; i < vtData.size(); i++)
            {

                try
                {
					Value v;

                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtData[i]._mkey.c_str(), vtData[i]._mkey.length());
                    v._mkey.readFrom(is);

                    is.setBuffer(vtData[i]._data._key.c_str(), vtData[i]._data._key.length());
                    v._ukey.readFrom(is);

					is.setBuffer(vtData[i]._data._value.c_str(), vtData[i]._data._value.length());
                    v._value.readFrom(is);

					v._iVersion = vtData[i]._data._iVersion;
					v._dirty = vtData[i]._data._dirty;
					v._iSyncTime = vtData[i]._data._synct;

                    vs.push_back(v);
                }
                catch(exception &ex)
                {
                }
            }
        }

    protected:
        TC_Multi_HashMap::HashMapItem _item;
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
        JhmIterator(const TC_Multi_HashMap::hash_iterator &it, const JhmLockPtr &lock)
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
        TC_Multi_HashMap::hash_iterator  _it;

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
    JceMultiHashMap()
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
     * @param fratio
     */
    void initHashRatio(float fratio)                       { this->_t.initHashRatio(fratio);}

    /**
     * ��ʼ��chunk����/��key hash����, Ĭ����1, ������һ����key�������ж��������
     * ����Ҫ���ı�����create֮ǰ����
     *
     * @param fratio
     */
    void initMainKeyHashRatio(float fratio)                { this->_t.initMainKeyHashRatio(fratio);}
    
    /**
     * ����hash��ʽ�����hash��������Ϊ����������hash����
     * @param hash_of
     */
    void setHashFunctor(TC_Multi_HashMap::hash_functor hashf)     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctor(hashf); 
    }

    /**
     * ������key��hash��ʽ��������裬��key��ʹ�����������������hash����
     * @param hash_of
     */
	void setHashFunctorM(TC_Multi_HashMap::hash_functor hashf)
	{
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        this->_t.setHashFunctorM(hashf); 
	}

    /**
     * ��ȡhash��ʽ
     * 
     * @return TC_Multi_HashMap::hash_functor&
     */
    TC_Multi_HashMap::hash_functor &getHashFunctor()              { return this->_t.getHashFunctor(); }

	/* ��ȡ��key hash��ʽ
	 * 
	 * @return TC_Multi_HashMap::hash_functor&
	 */
	TC_Multi_HashMap::hash_functor &getHashFunctorM()			  { return this->_t.getHashFunctorM(); }

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
	* ��ȡ��key hashͰ����
	*/
	size_t getMainKeyHashCount()
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.getMainKeyHashCount();
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
     * ������Only key����Ԫ�ظ���
     *
     * @return size_t
     */
    size_t onlyKeyCount()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.onlyKeyCount();
    }

	/**
     * ��key��Only key����Ԫ�ظ���
     *
     * @return size_t
     */
    size_t onlyKeyCountM()                                     
    { 
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.onlyKeyCountM();
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
     * TC_Multi_HashMap::ERASEBYGET
     * TC_Multi_HashMap::ERASEBYSET
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
     * @return TC_Multi_HashMap::tagMapHead
     */
    TC_Multi_HashMap::tagMapHead& getMapHead()                    { return this->_t.getMapHead(); }

    /**
     * ���û�дʱ����(��)
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
     *          TC_Multi_HashMap::RT_DUMP_FILE_ERR: dump���ļ�����
     *          TC_Multi_HashMap::RT_OK: dump���ļ��ɹ�
     */
    int dump2file(const string &sFile, bool bDoClear = false)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        int ret = this->_t.dump2file(sFile);
        if(ret != TC_Multi_HashMap::RT_OK)
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
     *          TC_Multi_HashMap::RT_LOAL_FILE_ERR: load����
     *          TC_Multi_HashMap::RT_VERSION_MISMATCH_ERR: �汾��һ��
     *          TC_Multi_HashMap::RT_OK: load�ɹ�
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
     * ���������״̬
     * @param mk, ��key
	 * @param uk, ����key�����������
     *
     * @return int
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_DIRTY_DATA: ��������
     *          TC_Multi_HashMap::RT_OK: �Ǹɾ�����
     *          ��������ֵ: ����
     */
    int checkDirty(const MK &mk, const UK &uk)
    {
        taf::JceOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

		taf::JceOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkDirty(smk, suk);
    }

    /**
     * �����key�µ�������״̬��ֻҪ��key���κ�һ����¼�������ݾͷ�����
     * @param mk, ��key
     *
     * @return int
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_DIRTY_DATA: ��������
     *          TC_Multi_HashMap::RT_OK: �Ǹɾ�����
     *          ��������ֵ: ����
     */
    int checkDirty(const MK &mk)
    {
        taf::JceOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkDirty(smk);
    }

    /**
     * ����Ϊ�ɾ�����i, �޸�SET/GETʱ����, �ᵼ�����ݲ���д
     * @param k
     *
     * @return int
     *          TC_Multi_HashMap::RT_READONLY: ֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int setClean(const MK &mk, const UK &uk)
    {
        taf::JceOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

		taf::JceOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setClean(smk, suk);
    }

    /**
     * ����Ϊ������, �޸�SET/GETʱ����, �ᵼ�����ݻ�д
     * @param mk
	 * @param uk
     * @return int
     *          TC_Multi_HashMap::RT_READONLY: ֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_OK: ���������ݳɹ�
     *          ��������ֵ: ����
     */
    int setDirty(const MK &mk, const UK &uk)
    {
        taf::JceOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

		taf::JceOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setDirty(smk, suk);
    }

    /**
     * �������ݻ�дʱ��
     * @param mk
	 * @param uk
	 * @param iSyncTime
     * @return int
     *          TC_Multi_HashMap::RT_READONLY: ֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_OK: ���������ݳɹ�
     *          ��������ֵ: ����
     */
    int setSyncTime(const MK &mk, const UK &uk, time_t iSyncTime)
    {
        taf::JceOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

		taf::JceOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setSyncTime(smk, suk, iSyncTime);
    }

    /**
     * ��ȡ����, �޸�GETʱ����
     * (���û�����Զ���Get����,û������ʱ����:RT_NO_DATA)
     * @param mk
	 * @param uk
     * @param v
     *
     * @return int:
     *          TC_Multi_HashMap::RT_NO_DATA: û������
     *          TC_Multi_HashMap::RT_READONLY: ֻ��ģʽ
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_OK:��ȡ���ݳɹ�
     *          TC_Multi_HashMap::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int get(const MK &mk, const UK &uk, Value &v)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        taf::JceOutputStream<BufferWriter> mosk;
        mk.writeTo(mosk);
        string smk(mosk.getBuffer(), mosk.getLength());

        taf::JceOutputStream<BufferWriter> uosk;
        uk.writeTo(uosk);
        string suk(uosk.getBuffer(), uosk.getLength());

		TC_Multi_HashMap::Value tv;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.get(smk, suk, tv);
        }

        // ��ȡ��������, ���
        if(ret == TC_Multi_HashMap::RT_OK)
        {
			v._mkey = mk;
			v._ukey = uk;
			v._dirty = tv._data._dirty;
			v._iSyncTime = tv._data._synct;
			v._iVersion = tv._data._iVersion;

            taf::JceInputStream<BufferReader> is;
            is.setBuffer(tv._data._value.c_str(), tv._data._value.length());
            v._value.readFrom(is);

            return ret;
        }

        if(ret != TC_Multi_HashMap::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

        //ֻ��ģʽ
        if(isReadOnly())
        {
            return TC_Multi_HashMap::RT_READONLY;
        }

    	// Hashmap��û�����ݣ����ⲿ��ȡ������ȡ����
        typename ToDoFunctor::DataRecord stDataRecord;
        stDataRecord._mkey   = mk;
		stDataRecord._ukey   = uk;
    	ret = _todo_of->get(stDataRecord);
    	if(ret == TC_Multi_HashMap::RT_OK)
    	{
            v = stDataRecord;
			// ���õ�hashmap��
    		return this->set(stDataRecord._mkey, stDataRecord._ukey, stDataRecord._value, 0, stDataRecord._dirty);
    	}
        else if(ret == TC_Multi_HashMap::RT_NO_GET)
        {
            return TC_Multi_HashMap::RT_NO_DATA;
        }
        else if(ret == TC_Multi_HashMap::RT_NO_DATA)
        {
			// û�����ݣ���only key����ʽ���õ�hashmap��
            ret = this->set(stDataRecord._mkey, stDataRecord._ukey);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                return TC_Multi_HashMap::RT_ONLY_KEY;
            }
            return ret;
        }

    	return TC_Multi_HashMap::RT_LOAD_DATA_ERR;
    }

	/**
	* ��������key��ȡ���е�����
	* @param mk
	* @param vs
	* 
	* @return int:
	*          RT_NO_DATA: û������
	*          RT_ONLY_KEY: ֻ��Key
	*          RT_PART_DATA: ���ݲ�ȫ��ֻ�в�������
	*          RT_OK: ��ȡ���ݳɹ�
	*          ��������ֵ: ����
	*/
	int get(const MK& mk, vector<Value> &vs)
	{
        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());

        int ret = TC_Multi_HashMap::RT_OK;
		vector<TC_Multi_HashMap::Value> hvs;
		{
			TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
			ret = this->_t.get(smk, hvs);
		}

		if(ret == TC_Multi_HashMap::RT_OK)
		{
			for(size_t i = 0; i < hvs.size(); i ++)
			{
				Value v;
				v._mkey = mk;
				
				taf::JceInputStream<BufferReader> is;
				is.setBuffer(hvs[i]._data._key.c_str(), hvs[i]._data._key.length());
				v._ukey.readFrom(is);
				
				is.setBuffer(hvs[i]._data._value.c_str(), hvs[i]._data._value.length());
				v._value.readFrom(is);
				
				v._iVersion = hvs[i]._data._iVersion;
				v._dirty = hvs[i]._data._dirty;
				v._iSyncTime = hvs[i]._data._synct;
				vs.push_back(v);
			}
			return ret;
		}

		if(ret != TC_Multi_HashMap::RT_NO_DATA || _todo_of == NULL)
        {
            return ret;
        }

		//ֻ��ģʽ
        if(isReadOnly())
        {
            return TC_Multi_HashMap::RT_READONLY;
        }

		// Hashmap��û�����ݣ����ⲿ��ȡ������ȡ����
		ret = _todo_of->get(mk, vs);
		if(ret == TC_Multi_HashMap::RT_OK)
		{
			// ���õ�hashmap��
			for(size_t i = 0; i < vs.size(); i ++)
			{
				ret = this->set(vs[i]._mkey, vs[i]._ukey, vs[i]._value, 0, vs[i]._dirty);
				if(ret != TC_Multi_HashMap::RT_OK)
				{
					// �����ý�ȥ��ȫ��ɾ��
					vector<TC_Multi_HashMap::Value> vtErased;
					{
						TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
						this->_t.del(smk, vtErased);
					}
					return ret;
				}
			}
		}
        else if(ret == TC_Multi_HashMap::RT_NO_GET)
        {
            return TC_Multi_HashMap::RT_NO_DATA;
        }
        else if(ret == TC_Multi_HashMap::RT_NO_DATA)
        {
			// û�����ݣ���only key����ʽ���õ�hashmap��
            ret = this->set(mk);
            if(ret == TC_Multi_HashMap::RT_OK)
            {
                return TC_Multi_HashMap::RT_ONLY_KEY;
            }
            return ret;
        }
		
		return TC_Multi_HashMap::RT_LOAD_DATA_ERR;
	}

    /**
     * ����hashֵ��ȡ��ͬhashֵ����������
     * ע��:cƥ����������, map�Ǽ�����, ��Ҫע��
     * @param h, hΪ��������(MK+UK)��hash
     * @param vv
     * @param c, ƥ��º���: bool operator()(MK, UK);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHash(uint32_t h, vector<Value> &vv, C c)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        size_t index = h % this->_t.getHashCount();
        size_t iAddr = this->_t.item(index)->_iBlockAddr;
		TC_Multi_HashMap::lock_iterator it(&this->_t, iAddr, TC_Multi_HashMap::lock_iterator::IT_UKEY, TC_Multi_HashMap::lock_iterator::IT_NEXT);

        while(it != this->_t.end())
        {
			Value v;
			TC_Multi_HashMap::Value hv;
			int ret = it->get(hv);
			if(ret == TC_Multi_HashMap::RT_OK)
			{
				taf::JceInputStream<BufferReader> is;
				is.setBuffer(hv._mkey.c_str(), hv._mkey.length());
				v._mkey.readFrom(is);

				is.setBuffer(hv._data._key.c_str(), hv._data._key.length());
				v._ukey.readFrom(is);
				
				if(c(v._mkey, v._ukey))
				{
					is.setBuffer(hv._data._value.c_str(), hv._data._value.length());
					v._value.readFrom(is);
					
					v._iVersion = hv._data._iVersion;
					v._dirty = hv._data._dirty;
					v._iSyncTime = hv._data._synct;
					vv.push_back(v);
				}
			}
			it ++;
        }

        return TC_Multi_HashMap::RT_OK;
    }

	 /**
     * ������key hashֵ��ȡ��ͬ��key hashֵ����������
     * ע��:cƥ����������, map�Ǽ�����, ��Ҫע��
     * @param h, hΪ��key(MK)��hash
     * @param mv, ���ؽ����������key����
     * @param c, ƥ��º���: bool operator()(MK);
     * 
     * @return int, RT_OK
     */
    template<typename C>
    int getHashM(uint32_t h, map<MK, vector<Value> > &mv, C c)
    {
        int ret = TC_Multi_HashMap::RT_OK;
		map<string, vector<TC_Multi_HashMap::Value> > hmv;
		{
			TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
			ret = this->_t.get(h, hmv);
		}
		
		if(ret == TC_Multi_HashMap::RT_OK)
		{
			map<string, vector<TC_Multi_HashMap::Value> >::iterator it = hmv.begin();
			while(it != hmv.end())
			{
				MK mk;
				taf::JceInputStream<BufferReader> is;
				is.setBuffer(it->first.c_str(), it->first.length());
				mk.readFrom(is);

				if(c(mk))
				{
					vector<Value> vs;
					vector<TC_Multi_HashMap::Value> &hvs = it->second;
					for(size_t i = 0; i < hvs.size(); i ++)
					{
						Value v;
						v._mkey = mk;
						
						is.setBuffer(hvs[i]._data._key.c_str(), hvs[i]._data._key.length());
						v._ukey.readFrom(is);
						
						is.setBuffer(hvs[i]._data._value.c_str(), hvs[i]._data._value.length());
						v._value.readFrom(is);
						
						v._iVersion = hvs[i]._data._iVersion;
						v._dirty = hvs[i]._data._dirty;
						v._iSyncTime = hvs[i]._data._synct;
						vs.push_back(v);
					}					
					mv[mk] = vs;
				}
				it ++;
			}
		}
		return ret;
    }

    /**
     * ��������, �޸�ʱ����, �ڴ治��ʱ���Զ���̭�ϵ�����
     * @param mk: ��key
	 * @param uk: ����key�����������
     * @param v: ֵ
	 * @param iVersion: ���õ����ݰ汾��Ӧ�ø���get�����ݰ汾�����ã�0��ʾ�����汾
     * @param bDirty: �Ƿ���������
	 * @param bHead: ���ݲ��뵽��key����ͷ������β��
	 * @param eType: ��������ݵ�����
	 *			PART_DATA: ����������ǲ�����������
	 *			FULL_DATA: �������������������
	 *			AUTO_DATA: ����Cache�����������;��������������ͣ�������������ǲ������ģ����յ�����Ҳ�ǲ������ģ�������������������ģ���������Ҳ�������ģ����Cache��û�����ݣ����������ǲ�������
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_Multi_HashMap::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const MK &mk, const UK &uk, const V &v, uint8_t iVersion, 
		bool bDirty = true, TC_Multi_HashMap::DATATYPE eType = TC_Multi_HashMap::AUTO_DATA, bool bHead = true)
    {
        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        taf::JceOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        taf::JceOutputStream<BufferWriter> vos;
        v.writeTo(vos);
        string sv(vos.getBuffer(), vos.getLength());

        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtErased;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(smk, suk, sv, iVersion, bDirty, eType, bHead, vtErased);
        }

        //������̭����
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
				UK euk;
                V tv;
                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
					stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
					stDataRecord._iVersion	= vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;

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
     * @param mk: ��key
	 * @param uk: ����key�����������
	 * @param bHead: ���ݲ��뵽��key����ͷ������β��
	 * @param eType: ��������ݵ�����
	 *			PART_DATA: ����������ǲ�����������
	 *			FULL_DATA: �������������������
	 *			AUTO_DATA: ����Cache�����������;��������������ͣ�������������ǲ������ģ����յ�����Ҳ�ǲ������ģ�������������������ģ���������Ҳ�������ģ����Cache��û�����ݣ����������ǲ�������
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_Multi_HashMap::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const MK &mk, const UK &uk, TC_Multi_HashMap::DATATYPE eType = TC_Multi_HashMap::AUTO_DATA, bool bHead = true)
    {
        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());
		
        taf::JceOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtErased;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(smk, suk, eType, bHead, vtErased);
        }

        //������̭����
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
				UK euk;
                V tv;

                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
					stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
					stDataRecord._iVersion	= vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;

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
     * ��������Key, �ڴ治��ʱ���Զ���̭�ϵ�����
     * @param mk: ��key
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          TC_Multi_HashMap::RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const MK &mk)
    {
        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());
		
        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtErased;

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(smk, vtErased);
        }

        //������̭����
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
				UK euk;
                V tv;

                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);

                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);

                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
					stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
					stDataRecord._iVersion	= vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;

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
	* �����������ݣ��ڴ治��ʱ���Զ���̭
	* ע�⣬�˽ӿ�ͨ�����ڴ����ݿ���ȡ����ͬ��key�µ�һ�����ݲ�ͬ����Cache
	* �������ô˽ӿ��������������ݣ�Ҫע�Ᵽ֤Cache�����ݿ����ݵ�һ��
	* @param vs, �������ݼ�
	* @param eType: ��������ݵ�����
	*			PART_DATA: ����������ǲ�����������
	*			FULL_DATA: �������������������
	*			AUTO_DATA: ����Cache�����������;��������������ͣ�������������ǲ������ģ����յ�����Ҳ�ǲ������ģ�������������������ģ���������Ҳ�������ģ����Cache��û�����ݣ����������ǲ�������
	* @param bHead, ���ݲ��뵽��key����ͷ������β��
	* @param bForce, �Ƿ�ǿ�Ʋ������ݣ�Ϊfalse���ʾ��������Ѿ������򲻸���
	*
	* @return int:
	*          TC_Multi_HashMap::RT_READONLY: mapֻ��
	*          TC_Multi_HashMap::RT_NO_MEMORY: û�пռ�(����̭��������»����)
	*          TC_Multi_HashMap::RT_OK: ���óɹ�
	*          ��������ֵ: ����
	*/
	int set(const vector<Value> &vs, TC_Multi_HashMap::DATATYPE eType = TC_Multi_HashMap::AUTO_DATA, bool bHead = true, bool bForce = true)
	{
        int ret = TC_Multi_HashMap::RT_OK;
        vector<TC_Multi_HashMap::Value> vtSet, vtErased;

		for(size_t i = 0; i < vs.size(); i ++)
		{
			TC_Multi_HashMap::Value v;

			taf::JceOutputStream<BufferWriter> mos;
			vs[i]._mkey.writeTo(mos);
			v._mkey.assign(mos.getBuffer(), mos.getLength());
			
			taf::JceOutputStream<BufferWriter> uos;
			vs[i]._ukey.writeTo(uos);
			v._data._key.assign(uos.getBuffer(), uos.getLength());

			taf::JceOutputStream<BufferWriter> vos;
			vs[i]._value.writeTo(vos);
			v._data._value.assign(vos.getBuffer(), vos.getLength());

			v._data._dirty = vs[i]._dirty;
			v._data._iVersion = vs[i]._iVersion;
			v._data._synct = vs[i]._iSyncTime;

			vtSet.push_back(v);
		}
		
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.set(vtSet, eType, bHead, bForce, vtErased);
        }

		//������̭����
        if(_todo_of)
        {
            for(size_t i = 0; i < vtErased.size(); i++)
            {
                MK emk;
				UK euk;
                V tv;
				
                try
                {
                    taf::JceInputStream<BufferReader> is;
                    is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
                    emk.readFrom(is);
					
                    is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
                    euk.readFrom(is);
					
                    is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
                    tv.readFrom(is);
					
                    typename ToDoFunctor::DataRecord stDataRecord;
                    stDataRecord._mkey      = emk;
					stDataRecord._ukey      = euk;
                    stDataRecord._value     = tv;
					stDataRecord._iVersion	= vtErased[i]._data._iVersion;
                    stDataRecord._dirty     = vtErased[i]._data._dirty;
                    stDataRecord._iSyncTime = vtErased[i]._data._synct;
					
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
     * @param mk, ��key
	 * @param uk, ����key�����������
	 *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_Multi_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int del(const MK &mk, const UK &uk)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        TC_Multi_HashMap::Value data;

        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        taf::JceOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, suk, data);
        }

        if(ret != TC_Multi_HashMap::RT_OK && ret != TC_Multi_HashMap::RT_ONLY_KEY && ret != TC_Multi_HashMap::RT_NO_DATA)
        {
            return ret;
        }

        if(_todo_of)
        {
            typename ToDoFunctor::DataRecord stDataRecord;

			stDataRecord._mkey = mk;
            stDataRecord._ukey = uk;

            if(ret == TC_Multi_HashMap::RT_OK)
            {
                V v;
                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                v.readFrom(is);

                stDataRecord._value     = v;
				stDataRecord._iVersion	= data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;
            }

            _todo_of->del((ret == TC_Multi_HashMap::RT_OK), stDataRecord);
        }
        return ret;
    }

    /**
     * ɾ����key�µ���������
     * cache������,todo��erase������
     *
     * @param mk, ��key
	 *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_Multi_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int erase(const MK &mk)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        vector<TC_Multi_HashMap::Value> vtErased;

        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, vtErased);
        }

        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }

        if(_todo_of)
        {
			for(size_t i = 0; i < vtErased.size(); i ++)
			{
				MK emk;
				UK euk;
				V tv;
				
				try
				{
					taf::JceInputStream<BufferReader> is;
					is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
					emk.readFrom(is);
					
					is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
					euk.readFrom(is);
					
					is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
					tv.readFrom(is);
					
					typename ToDoFunctor::DataRecord stDataRecord;
					stDataRecord._mkey      = emk;
					stDataRecord._ukey      = euk;
					stDataRecord._value     = tv;
					stDataRecord._iVersion	= vtErased[i]._data._iVersion;
					stDataRecord._dirty     = vtErased[i]._data._dirty;
					stDataRecord._iSyncTime = vtErased[i]._data._synct;
					
					_todo_of->erase(stDataRecord);
				}
				catch(exception &ex)
				{
				}
			}
        }
        return ret;
    }

    /**
     * ɾ��ָ������
     * cache������,todo��erase������
     *
     * @param mk, ��key
	 * @param uk, ����key�����������
	 *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_Multi_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
	int erase(const MK &mk, const UK &uk)
	{
        int ret = TC_Multi_HashMap::RT_OK;
		
        TC_Multi_HashMap::Value data;
		
        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());
		
        taf::JceOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());
		
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, suk, data);
        }
		
        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }
        if(_todo_of)
        {
			MK emk;
			UK euk;
			V tv;
			
			try
			{
				taf::JceInputStream<BufferReader> is;
				is.setBuffer(data._mkey.c_str(), data._mkey.length());
				emk.readFrom(is);
				
				is.setBuffer(data._data._key.c_str(), data._data._key.length());
				euk.readFrom(is);
				
				is.setBuffer(data._data._value.c_str(), data._data._value.length());
				tv.readFrom(is);
				
				typename ToDoFunctor::DataRecord stDataRecord;
				stDataRecord._mkey      = emk;
				stDataRecord._ukey      = euk;
				stDataRecord._value     = tv;
				stDataRecord._iVersion	= data._data._iVersion;
				stDataRecord._dirty     = data._data._dirty;
				stDataRecord._iSyncTime = data._data._synct;
				
				_todo_of->erase(stDataRecord);
			}
			catch(exception &ex)
			{
			}
        }
        return ret;
	}

    /**
     * ǿ��ɾ������,������todo��erase������
     *
     * @param mk, ��key
	 * @param uk, ����key�����������
	 *
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_Multi_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
    int eraseByForce(const MK &mk, const UK &uk)
    {
        int ret = TC_Multi_HashMap::RT_OK;

        TC_Multi_HashMap::Value data;

        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        taf::JceOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, suk, data);
        }

        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }

        return ret;
    }

    /**
     * ǿ��ɾ����key�µ���������,������todo��erase������
     *
     * @param mk, ��key
     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_NO_DATA: û�е�ǰ����
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key, Ҳɾ����
     *          TC_Multi_HashMap::RT_OK: ɾ�����ݳɹ�
     *          ��������ֵ: ����
     */
	int eraseByForce(const MK &mk)
    {
        int ret = TC_Multi_HashMap::RT_OK;
		
        vector<TC_Multi_HashMap::Value> data;
		
        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
		
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            ret = this->_t.del(smk, data);
        }
		
        if(ret != TC_Multi_HashMap::RT_OK)
        {
            return ret;
        }
		
        return ret;
    }

    /**
     * ��̭����, ����Getʱ����̭
     * ֱ��: Ԫ�ظ���/chunks * 100 < ratio��bCheckDirty Ϊtrueʱ����������������̭����
     * @param ratio: �����ڴ�chunksʹ�ñ��� 0< ratio < 100
	 * @param bCheckDirty: �Ƿ���������״̬�������������������ݲ���̭

     * @return int:
     *          TC_Multi_HashMap::RT_READONLY: mapֻ��
     *          TC_Multi_HashMap::RT_OK:��̭���
     */
    int erase(int ratio, bool bCheckDirty = false)
    {
        while(true)
        {
            int ret;
            vector<TC_Multi_HashMap::Value> vtErased;

            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.erase(ratio, vtErased, bCheckDirty);
                if(ret == TC_Multi_HashMap::RT_OK || ret == TC_Multi_HashMap::RT_READONLY)
                {
                    return ret;
                }

                if(ret != TC_Multi_HashMap::RT_ERASE_OK)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
				for(size_t i = 0; i < vtErased.size(); i++)
				{
					MK emk;
					UK euk;
					V tv;
					try
					{
						taf::JceInputStream<BufferReader> is;
						is.setBuffer(vtErased[i]._mkey.c_str(), vtErased[i]._mkey.length());
						emk.readFrom(is);
						
						is.setBuffer(vtErased[i]._data._key.c_str(), vtErased[i]._data._key.length());
						euk.readFrom(is);
						
						is.setBuffer(vtErased[i]._data._value.c_str(), vtErased[i]._data._value.length());
						tv.readFrom(is);
						
						typename ToDoFunctor::DataRecord stDataRecord;
						stDataRecord._mkey      = emk;
						stDataRecord._ukey      = euk;
						stDataRecord._value     = tv;
						stDataRecord._iVersion	= vtErased[i]._data._iVersion;
						stDataRecord._dirty     = vtErased[i]._data._dirty;
						stDataRecord._iSyncTime = vtErased[i]._data._synct;
						
						_todo_of->erase(stDataRecord);
					}
					catch(exception &ex)
					{
					}
				}
             }
        }
        return TC_Multi_HashMap::RT_OK;
    }

    /**
     * ��д������¼, �����¼������, �����κδ���
     * @param mk
	 * @param uk
     * 
     * @return int
     *          TC_Multi_HashMap::RT_NO_DATA: û������
     *          TC_Multi_HashMap::RT_ONLY_KEY:ֻ��Key
     *          TC_Multi_HashMap::RT_OK:��ȡ���ݳɹ�
     *          TC_Multi_HashMap::RT_LOAD_DATA_ERR: load����ʧ��
     *          ��������ֵ: ����
     */
    int sync(const MK &mk, const UK &uk)
    {
		Value v;
        int ret = get(mk, uk, v);

        if(ret == TC_Multi_HashMap::RT_OK)
        {
            if(_todo_of)
            {
                _todo_of->sync(v);
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
     *      TC_Multi_HashMap::RT_OK: ��д�����
     */
    int sync(time_t iNowTime)
    {
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
            this->_t.sync();
        }

        while(true)
        {
            TC_Multi_HashMap::Value data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.sync(iNowTime, data);
                if(ret == TC_Multi_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_Multi_HashMap::RT_NEED_SYNC)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                MK mk;
				UK uk;
                V tv;

                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._mkey.c_str(), data._mkey.length());
                mk.readFrom(is);

                is.setBuffer(data._data._key.c_str(), data._data._key.length());
                uk.readFrom(is);

                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._mkey      = mk;
				stDataRecord._ukey      = uk;
                stDataRecord._value     = tv;
				stDataRecord._iVersion	= data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;

                _todo_of->sync(stDataRecord);
            }
        }

        return TC_Multi_HashMap::RT_OK;
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
     *      TC_Multi_HashMap::RT_OK: ��д�����
     * 
     * ʾ����
     *      p->sync();
     *      while(true) {
     *          int iRet = pthis->SyncOnce(tNow);
     *          if( iRet == TC_Multi_HashMap::RT_OK )
	 *				break;
	 *		}
     */
    int syncOnce(time_t iNowTime)
    {
		TC_Multi_HashMap::Value data;

		int ret;
		{
			TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
			ret = this->_t.sync(iNowTime, data);
			if(ret == TC_Multi_HashMap::RT_OK)
			{
				return ret;
			}

			if(ret != TC_Multi_HashMap::RT_NEED_SYNC)
			{
				return ret;
			}
		}

		if(_todo_of)
		{
			MK mk;
			UK uk;
			V tv;

			taf::JceInputStream<BufferReader> is;
			is.setBuffer(data._mkey.c_str(), data._mkey.length());
			mk.readFrom(is);

			is.setBuffer(data._data._key.c_str(), data._data._key.length());
			uk.readFrom(is);

			is.setBuffer(data._data._value.c_str(), data._data._value.length());
			tv.readFrom(is);

			typename ToDoFunctor::DataRecord stDataRecord;
			stDataRecord._mkey      = mk;
			stDataRecord._ukey      = uk;
			stDataRecord._value     = tv;
			stDataRecord._iVersion	= data._data._iVersion;
			stDataRecord._dirty     = data._data._dirty;
			stDataRecord._iSyncTime = data._data._synct;

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
     *      TC_Multi_HashMap::RT_OK: ����OK��
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
            TC_Multi_HashMap::Value data;

            int ret;
            {
                TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
                ret = this->_t.backup(data);
                if(ret == TC_Multi_HashMap::RT_OK)
                {
                    return ret;
                }

                if(ret != TC_Multi_HashMap::RT_NEED_BACKUP)
                {
                    continue;
                }
            }

            if(_todo_of)
            {
                MK mk;
				UK uk;
                V tv;

                taf::JceInputStream<BufferReader> is;
                is.setBuffer(data._mkey.c_str(), data._mkey.length());
                mk.readFrom(is);

                is.setBuffer(data._data._key.c_str(), data._data._key.length());
                uk.readFrom(is);

                is.setBuffer(data._data._value.c_str(), data._data._value.length());
                tv.readFrom(is);

                typename ToDoFunctor::DataRecord stDataRecord;
                stDataRecord._mkey      = mk;
				stDataRecord._ukey		= uk;
                stDataRecord._value     = tv;
				stDataRecord._iVersion	= data._data._iVersion;
                stDataRecord._dirty     = data._data._dirty;
                stDataRecord._iSyncTime = data._data._synct;

                _todo_of->backup(stDataRecord);
            }
        }

        return TC_Multi_HashMap::RT_OK;
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
     * ������������(MK+UK)��������
     * @param mk
	 * @param uk
	 * @return lock_interator
	 *		����end()��ʾû�в鵽
     */
    lock_iterator find(const MK &mk, const UK &uk)
    {
        taf::JceOutputStream<BufferWriter> mos;
        mk.writeTo(mos);
        string smk(mos.getBuffer(), mos.getLength());

        taf::JceOutputStream<BufferWriter> uos;
        uk.writeTo(uos);
        string suk(uos.getBuffer(), uos.getLength());

        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(smk, suk), jlock);
    }

	/**
	* ������key��������������
	* @param mk, ��key
	*
	* @return size_t, ��key�µļ�¼��
	*/
	size_t count(const MK &mk)
	{
        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());

		TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
		return this->_t.count(smk);
	}

	/**
	* ������key���ҵ�һ�����ݵ�λ��
	* �������count������Ͽ��Ա���������key�µ�����
	* Ҳ����ֱ��ʹ�õ�������ֱ��end
	* @param mk, ��key
	* @return lock_iterator, ����end()��ʾû������
	*/
	lock_iterator find(const MK& mk)
	{
        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
		
        JhmAutoLockPtr jlock(new JhmAutoLock(this->mutex()));
        return JhmLockIterator(this->_t.find(smk), jlock);
	}

	/**
	* �ж���key�Ƿ����
	* @param mk, ��key
	*
	* @return int
	*		TC_Multi_HashMap::RT_OK, ��key���ڣ���������
	*		TC_Multi_HashMap::RT_ONLY_KEY, ��key���ڣ�û������
	*		TC_Multi_HashMap::RT_PART_DATA, ��key���ڣ���������ݿ��ܲ�����
	*		TC_Multi_HashMap::RT_NO_DATA, ��key������
	*/
	int checkMainKey(const MK& mk)
	{
        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
		
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.checkMainKey(smk);
	}

	/**
	* ������key�����ݵ�������
	* @param mk, ��key
	* @param bFull, trueΪ�������ݣ�falseΪ����������
	*
	* @return
	*          RT_READONLY: ֻ��
	*          RT_NO_DATA: û�е�ǰ����
	*          RT_OK: ���óɹ�
	*          ��������ֵ: ����
	*/
	int setFullData(const MK &mk, bool bFull)
	{
        taf::JceOutputStream<BufferWriter> os;
        mk.writeTo(os);
        string smk(os.getBuffer(), os.getLength());
		
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());
        return this->_t.setFullData(smk, bFull);
	}

	/**
	* ��黵block�����ɽ����޸�
	* @param bRepaire, �Ƿ�����޸�
	*
	* @return size_t, ���ػ����ݸ���
	*/
	size_t checkBadBlock(bool bRepair)
	{
        size_t c     = this->_t.getHashCount();
        size_t e     = 0;
        for(size_t i = 0; i < c; i++)
        {
            TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

            e += this->_t.checkBadBlock(i, bRepair);
        }

        return e;
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

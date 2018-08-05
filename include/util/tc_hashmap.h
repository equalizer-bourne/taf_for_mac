#ifndef	__TC_HASHMAP_H__
#define __TC_HASHMAP_H__

#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include "util/tc_ex.h"
#include "util/tc_mem_vector.h"
#include "util/tc_pack.h"
#include "util/tc_mem_chunk.h"
#include "util/tc_functor.h"
#include "util/tc_hash_fun.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: hashmap��
// Author : j@syswin.com              
// Modify: @syswin.com              
/////////////////////////////////////////////////
/**
* Hash map�쳣��
*/
struct TC_HashMap_Exception : public TC_Exception
{
	TC_HashMap_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_HashMap_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_HashMap_Exception() throw(){};
};

////////////////////////////////////////////////////////////////////////////////////
/**
 * �����ڴ��hashmap
 * ���в�����Ҫ�Լ�����
 */
class TC_HashMap
{
public:
    struct HashMapIterator;
    struct HashMapLockIterator;

    //��������
    struct BlockData
    {
        string  _key;       //����Key
        string  _value;     //����value
        bool    _dirty;     //�Ƿ���������
        time_t  _synct;     //sync time, ��һ���������Ļ�дʱ��
        BlockData()
        : _dirty(false)
        , _synct(0)
        {
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////
    /**
    * �ڴ����ݿ�
    *
    * ��ȡ�ʹ������
    */
    class Block
    {
    public:

        /**
         * block����ͷ
         */
        struct tagBlockHead
        {
            uint32_t    _iSize;         /**block��������С*/
            uint32_t    _iIndex;        /**hash������*/
            size_t      _iBlockNext;    /**��һ��Block,tagBlockHead, û����Ϊ0*/
            size_t      _iBlockPrev;    /**��һ��Block,tagBlockHead, û����Ϊ0*/
            size_t      _iSetNext;      /**Set���ϵ���һ��Block*/
            size_t      _iSetPrev;      /**Set���ϵ���һ��Block*/
            size_t      _iGetNext;      /**Get���ϵ���һ��Block*/
            size_t      _iGetPrev;      /**Get���ϵ���һ��Block*/
            time_t      _iSyncTime;     /**�ϴλ�дʱ��*/
            bool        _bDirty;        /**�Ƿ���������*/
            bool        _bOnlyKey;      /**�Ƿ�ֻ��key, û������*/
            bool        _bNextChunk;    /**�Ƿ�����һ��chunk*/
            union
            {
                size_t  _iNextChunk;    /**��һ��Chunk��, _bNextChunk=trueʱ��Ч, tagChunkHead*/
                size_t  _iDataLen;      /**��ǰ���ݿ���ʹ���˵ĳ���, _bNextChunk=falseʱ��Ч*/
            };
            char        _cData[0];      /**���ݿ�ʼ����*/
        }__attribute__((packed));

        /**
         * ��ͷ����block, ��Ϊchunk
         */
        struct tagChunkHead
        {
            uint32_t    _iSize;         /**block��������С*/
            bool        _bNextChunk;    /**�Ƿ�����һ��chunk*/
            union
            {
                size_t  _iNextChunk;    /**��һ�����ݿ�, _bNextChunk=trueʱ��Ч, tagChunkHead*/
                size_t  _iDataLen;      /**��ǰ���ݿ���ʹ���˵ĳ���, _bNextChunk=falseʱ��Ч*/
            };
            char        _cData[0];      /**���ݿ�ʼ����*/
        }__attribute__((packed));

        /**
         * ���캯��
         * @param Map
         * @param ��ǰMemBlock�ĵ�ַ
         * @param pAdd
         */
        Block(TC_HashMap *pMap, size_t iAddr)
        : _pMap(pMap)
        , _iHead(iAddr)
        {
        }

        /**
         * copy
         * @param mb
         */
        Block(const Block &mb)
        : _pMap(mb._pMap)
        , _iHead(mb._iHead)
        {
        }

        /**
         *
         * @param mb
         *
         * @return Block&
         */
        Block& operator=(const Block &mb)
        {
            _iHead  = mb._iHead;
            _pMap   = mb._pMap;
            return (*this);
        }

        /**
         *
         * @param mb
         *
         * @return bool
         */
        bool operator==(const Block &mb) const { return _iHead == mb._iHead && _pMap == mb._pMap; }

        /**
         *
         * @param mb
         *
         * @return bool
         */
        bool operator!=(const Block &mb) const { return _iHead != mb._iHead || _pMap != mb._pMap; }

        /**
         * ��ȡblockͷ���Ե�ַ
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagBlockHead *getBlockHead(size_t iAddr) { return ((tagBlockHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * ��ȡMemBlockͷ��ַ
         *
         * @return void*
         */
        tagBlockHead *getBlockHead() {return getBlockHead(_iHead);}

        /**
         * ͷ��
         *
         * @return size_t
         */
        size_t getHead() { return _iHead;}

        /**
         * ��ǰͰ�������һ��block��ͷ��
         *
         * @return size_t
         */
        size_t getLastBlockHead();

        /**
         * ����Getʱ��
         *
         * @return time_t
         */
        time_t getSyncTime() { return getBlockHead()->_iSyncTime; }

        /**
         * ���û�дʱ��
         * @param iSyncTime
         */
        void setSyncTime(time_t iSyncTime) { getBlockHead()->_iSyncTime = iSyncTime; }

        /**
         * ��ȡBlock�е�����
         *
         * @return int
         *          TC_HashMap::RT_OK, ����, �����쳣
         *          TC_HashMap::RT_ONLY_KEY, ֻ��Key
         *          �����쳣
         */
        int getBlockData(TC_HashMap::BlockData &data);

        /**
         * ��ȡ����
         * @param pData
         * @param iDatalen
         * @return int,
         *          TC_HashMap::RT_OK, ����
         *          �����쳣
         */
        int get(void *pData, size_t &iDataLen);

        /**
         * ��ȡ����
         * @param s
         * @return int
         *          TC_HashMap::RT_OK, ����
         *          �����쳣
         */
        int get(string &s);

        /**
         * ��������
         * @param pData
         * @param iDatalen
         * @param vtData, ��̭������
         */
        int set(const void *pData, size_t iDataLen, bool bOnlyKey, vector<TC_HashMap::BlockData> &vtData);

        /**
         * �Ƿ���������
         *
         * @return bool
         */
        bool isDirty()      { return getBlockHead()->_bDirty; }

        /**
         * ��������
         * @param b
         */
        void setDirty(bool b);

        /**
         * �Ƿ�ֻ��key
         *
         * @return bool
         */
        bool isOnlyKey()    { return getBlockHead()->_bOnlyKey; }

        /**
         * ��ǰԪ���ƶ�����һ��block
         * @return true, �Ƶ���һ��block��, false, û����һ��block
         *
         */
        bool nextBlock();

        /**
         * ��ǰԪ���ƶ�����һ��block
         * @return true, �Ƶ���һ��block��, false, û����һ��block
         *
         */
        bool prevBlock();

		/**
         * �ͷ�block�����пռ�
         */
        void deallocate();

        /**
         * ��blockʱ���øú���
         * ����һ���µ�block
         * @param index, hash����
         * @param iAllocSize, �ڴ��С
         */
        void makeNew(size_t index, size_t iAllocSize);

        /**
         * ��Block������ɾ����ǰBlock
         * ֻ��Block��Ч, ��Chunk����Ч��
         * @return
         */
        void erase();

        /**
         * ˢ��set����, ����Set����ͷ��
         */
        void refreshSetList();

        /**
         * ˢ��get����, ����Get����ͷ��
         */
        void refreshGetList();

    protected:

        /**
         * ��ȡChunkͷ���Ե�ַ
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead() {return getChunkHead(_iHead);}

        /**
         * ��ȡchunkͷ���Ե�ַ
         * @param iAddr
         *
         * @return tagChunkHead*
         */
        tagChunkHead *getChunkHead(size_t iAddr) { return ((tagChunkHead*)_pMap->getAbsolute(iAddr)); }

        /**
         * �ӵ�ǰ��chunk��ʼ�ͷ�
         * @param iChunk �ͷŵ�ַ
         */
        void deallocate(size_t iChunk);

		/**
         * ���������������, ��������chunk, ��Ӱ��ԭ������
         * ʹ�����ӵ�����������iDataLen
         * �ͷŶ����chunk
         * @param iDataLen
         *
         * @return int,
         */
        int allocate(size_t iDataLen, vector<TC_HashMap::BlockData> &vtData);

        /**
         * �ҽ�chunk, ���core��ҽ�ʧ��, ��֤�ڴ�黹������
         * @param pChunk
         * @param chunks
         *
         * @return int
         */
        int joinChunk(tagChunkHead *pChunk, const vector<size_t> chunks);

        /**
         * ����n��chunk��ַ, ע���ͷ��ڴ��ʱ�����ͷ����ڷ���Ķ���
         * @param fn, ����ռ��С
         * @param chunks, ����ɹ����ص�chunks��ַ�б�
         * @param vtData, ��̭������
         * @return int
         */
        int allocateChunk(size_t fn, vector<size_t> &chunks, vector<TC_HashMap::BlockData> &vtData);

        /**
         * ��ȡ���ݳ���
         *
         * @return size_t
         */
        size_t getDataLen();

    private:

        /**
         * Map
         */
        TC_HashMap         *_pMap;

        /**
         * block�����׵�ַ, ��Ե�ַ
         */
        size_t              _iHead;

    };

    ////////////////////////////////////////////////////////////////////////
    /*
    * �ڴ����ݿ������
    *
    */
    class BlockAllocator
    {
    public:

        /**
         * ���캯��
         */
        BlockAllocator(TC_HashMap *pMap)
        : _pMap(pMap)
        , _pChunkAllocator(new TC_MemMultiChunkAllocator())
        {
        }

        /**
         * ��������
         */
        ~BlockAllocator()
        {
            if(_pChunkAllocator != NULL)
            {
                delete _pChunkAllocator;
            }
            _pChunkAllocator = NULL;
        }


        /**
         * ��ʼ��
         * @param pHeadAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
         * @param iSize, �ڴ��С
         * @param iMinBlockSize, ��С���ݿ��С
         * @param iMaxBlockSize, ������ݿ��С
         * @param fFactor, ����
         */
        void create(void *pHeadAddr, size_t iSize, size_t iMinBlockSize, size_t iMaxBlockSize, float fFactor)
        {
            _pChunkAllocator->create(pHeadAddr, iSize, iMinBlockSize, iMaxBlockSize, fFactor);
        }

        /**
         * ������
         * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
         */
        void connect(void *pHeadAddr)
        {
            _pChunkAllocator->connect(pHeadAddr);
        }

        /**
         * ��չ�ռ�
         * @param pAddr
         * @param iSize
         */
        void append(void *pAddr, size_t iSize)
        {
            _pChunkAllocator->append(pAddr, iSize);
        }

        /**
         * �ؽ�
         */
        void rebuild()
        {
            _pChunkAllocator->rebuild();
        }

        /**
         * ��ȡÿ�����ݿ�ͷ����Ϣ
         *
         * @return TC_MemChunk::tagChunkHead
         */
        vector<TC_MemChunk::tagChunkHead> getBlockDetail() const  { return _pChunkAllocator->getBlockDetail(); }

        /**
         * �ڴ��С
         *
         * @return size_t
         */
        size_t getMemSize() const       { return _pChunkAllocator->getMemSize(); }

        /**
         * ��������������
         *
         * @return size_t
         */
        size_t getCapacity() const      { return _pChunkAllocator->getCapacity(); }

        /**
         * ÿ��block�е�chunk����(ÿ��block�е�chunk������ͬ)
         *
         * @return vector<size_t>
         */
        vector<size_t> singleBlockChunkCount() const { return _pChunkAllocator->singleBlockChunkCount(); }

        /**
         * ����block��chunk����
         *
         * @return size_t
         */
        size_t allBlockChunkCount() const    { return _pChunkAllocator->allBlockChunkCount(); }

        /**
         * ���ڴ��з���һ���µ�Block
         *
         * @param index, block hash����
         * @param iAllocSize: in/��Ҫ����Ĵ�С, out/����Ŀ��С
         * @param vtData, �����ͷŵ��ڴ������
         * @return size_t, ��Ե�ַ,0��ʾû�пռ���Է���
         */
        size_t allocateMemBlock(size_t index, size_t &iAllocSize, vector<TC_HashMap::BlockData> &vtData);

        /**
         * Ϊ��ַΪiAddr��Block����һ��chunk
         *
         * @param iAddr,�����Block�ĵ�ַ
         * @param iAllocSize, in/��Ҫ����Ĵ�С, out/����Ŀ��С
         * @param vtData �����ͷŵ��ڴ������
         * @return size_t, ��Ե�ַ,0��ʾû�пռ���Է���
         */
        size_t allocateChunk(size_t iAddr, size_t &iAllocSize, vector<TC_HashMap::BlockData> &vtData);

        /**
         * �ͷ�Block
         * @param v
         */
        void deallocateMemBlock(const vector<size_t> &v);

        /**
         * �ͷ�Block
         * @param v
         */
        void deallocateMemBlock(size_t v);

    protected:
        //������copy����
        BlockAllocator(const BlockAllocator &);
        //������ֵ
        BlockAllocator& operator=(const BlockAllocator &);
        bool operator==(const BlockAllocator &mba) const;
        bool operator!=(const BlockAllocator &mba) const;
    public:
        /**
         * map
         */
    	TC_HashMap                  *_pMap;

        /**
         * chunk������
         */
        TC_MemMultiChunkAllocator   *_pChunkAllocator;
    };

    ////////////////////////////////////////////////////////////////
    // map��������
    class HashMapLockItem
    {
    public:

    	/**
    	 *
         * @param pMap
         * @param iAddr
    	 */
    	HashMapLockItem(TC_HashMap *pMap, size_t iAddr);

        /**
         *
         * @param mcmdi
         */
        HashMapLockItem(const HashMapLockItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return HashMapLockItem&
         */
        HashMapLockItem &operator=(const HashMapLockItem &mcmdi);

    	/**
    	 *
    	 * @param mcmdi
    	 *
    	 * @return bool
    	 */
    	bool operator==(const HashMapLockItem &mcmdi);

    	/**
    	 *
    	 * @param mcmdi
    	 *
    	 * @return bool
    	 */
    	bool operator!=(const HashMapLockItem &mcmdi);

        /**
         * �Ƿ���������
         *
         * @return bool
         */
        bool isDirty();

        /**
         * �Ƿ�ֻ��Key
         *
         * @return bool
         */
        bool isOnlyKey();

        /**
         * ���Syncʱ��
         *
         * @return time_t
         */
        time_t getSyncTime();

    	/**
         * ��ȡֵ, ���ֻ��Key(isOnlyKey)�������, vΪ��
         * @return int
         *          RT_OK:���ݻ�ȡOK
         *          RT_ONLY_KEY: key��Ч, v��ЧΪ��
         *          ����ֵ, �쳣
         *
    	 */
    	int get(string& k, string& v);

    	/**
    	 * ��ȡֵ
         * @return int
         *          RT_OK:���ݻ�ȡOK
         *          ����ֵ, �쳣
    	 */
    	int get(string& k);

        /**
         * ���ݿ���Ե�ַ
         *
         * @return size_t
         */
        size_t getAddr() const { return _iAddr; }

    protected:

        /**
         * ��������
         * @param k
         * @param v
         * @param vtData, ��̭������
         * @return int
         */
        int set(const string& k, const string& v, vector<TC_HashMap::BlockData> &vtData);

        /**
         * ����Key, ������
         * @param k
         * @param vtData
         *
         * @return int
         */
        int set(const string& k, vector<TC_HashMap::BlockData> &vtData);

        /**
         *
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string &k, string &v, int &ret);

        /**
         *
         * @param pKey
         * @param iKeyLen
         *
         * @return bool
         */
        bool equal(const string& k, int &ret);

    	/**
         * ��һ��item
    	 *
    	 * @return HashMapLockItem
    	 */
    	void nextItem(int iType);

        /**
         * ��һ��item
         * @param iType
         */
    	void prevItem(int iType);

        friend class TC_HashMap;
        friend struct TC_HashMap::HashMapLockIterator;

    private:
        /**
         * map
         */
    	TC_HashMap *_pMap;

        /**
         * block�ĵ�ַ
         */
        size_t      _iAddr;
    };

    /////////////////////////////////////////////////////////////////////////
    // ���������
    struct HashMapLockIterator
    {
    public:

        //���������ʽ
        enum
        {
            IT_BLOCK    = 0,        //��ͨ��˳��
            IT_SET      = 1,        //Setʱ��˳��
            IT_GET      = 2,        //Getʱ��˳��
        };

        /**
         * ��������˳��
         */
        enum
        {
            IT_NEXT     = 0,        //˳��
            IT_PREV     = 1,        //����
        };

        /**
         *
         */
        HashMapLockIterator();

    	/**
         * ���캯��
         * @param iAddr, ��ַ
    	 * @param type
    	 */
    	HashMapLockIterator(TC_HashMap *pMap, size_t iAddr, int iType, int iOrder);

        /**
         * copy
         * @param it
         */
        HashMapLockIterator(const HashMapLockIterator &it);

        /**
         * ����
         * @param it
         *
         * @return HashMapLockIterator&
         */
        HashMapLockIterator& operator=(const HashMapLockIterator &it);

        /**
         *
         * @param mcmi
         *
         * @return bool
         */
        bool operator==(const HashMapLockIterator& mcmi);

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const HashMapLockIterator& mcmi);

    	/**
    	 * ǰ��++
    	 *
    	 * @return HashMapLockIterator&
    	 */
    	HashMapLockIterator& operator++();

    	/**
         * ����++
         *
         * @return HashMapLockIterator&
    	 */
    	HashMapLockIterator operator++(int);

    	/**
    	 *
    	 *
         * @return HashMapLockItem&i
    	 */
    	HashMapLockItem& operator*() { return _iItem; }

    	/**
    	 *
    	 *
    	 * @return HashMapLockItem*
    	 */
    	HashMapLockItem* operator->() { return &_iItem; }

    public:
        /**
         *
         */
        TC_HashMap  *_pMap;

        /**
         *
         */
        HashMapLockItem _iItem;

    	/**
    	 * �������ķ�ʽ
    	 */
    	int        _iType;

        /**
         * ��������˳��
         */
        int        _iOrder;

    };

    ////////////////////////////////////////////////////////////////
    // map��HashItem��, һ��HashItem��Ӧ���������
    class HashMapItem
    {
    public:

        /**
         *
         * @param pMap
         * @param iIndex
         */
        HashMapItem(TC_HashMap *pMap, size_t iIndex);

        /**
         *
         * @param mcmdi
         */
        HashMapItem(const HashMapItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return HashMapItem&
         */
        HashMapItem &operator=(const HashMapItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator==(const HashMapItem &mcmdi);

        /**
         *
         * @param mcmdi
         *
         * @return bool
         */
        bool operator!=(const HashMapItem &mcmdi);

        /**
         * ��ȡ��ǰhashͰ����������, ע��ֻ��ȡ��key/value������
         * ����ֻ��key������, ����ȡ
         * 
         * @return
         */
        void get(vector<TC_HashMap::BlockData> &vtData);

        /**
         * 
         * 
         * @return int
         */
        int getIndex() const { return _iIndex; }

        /**
         * ��һ��item
         *
         */
        void nextItem();

        friend class TC_HashMap;
        friend struct TC_HashMap::HashMapIterator;

    private:
        /**
         * map
         */
        TC_HashMap *_pMap;

        /**
         * ���ݿ��ַ
         */
        size_t      _iIndex;
    };

    /////////////////////////////////////////////////////////////////////////
    // ���������
    struct HashMapIterator
    {
    public:

        /**
         * ���캯��
         */
        HashMapIterator();

    	/**
         * ���캯��
         * @param iIndex, ��ַ
    	 * @param type
    	 */
    	HashMapIterator(TC_HashMap *pMap, size_t iIndex);

        /**
         * copy
         * @param it
         */
        HashMapIterator(const HashMapIterator &it);

        /**
         * ����
         * @param it
         *
         * @return HashMapLockIterator&
         */
        HashMapIterator& operator=(const HashMapIterator &it);

        /**
         *
         * @param mcmi
         *
         * @return bool
         */
        bool operator==(const HashMapIterator& mcmi);

        /**
         *
         * @param mv
         *
         * @return bool
         */
        bool operator!=(const HashMapIterator& mcmi);

    	/**
    	 * ǰ��++
    	 *
    	 * @return HashMapIterator&
    	 */
    	HashMapIterator& operator++();

    	/**
         * ����++
         *
         * @return HashMapIterator&
    	 */
    	HashMapIterator operator++(int);

    	/**
    	 *
    	 *
         * @return HashMapItem&i
    	 */
    	HashMapItem& operator*() { return _iItem; }

    	/**
    	 *
    	 *
    	 * @return HashMapItem*
    	 */
    	HashMapItem* operator->() { return &_iItem; }

    public:
        /**
         *
         */
        TC_HashMap  *_pMap;

        /**
         *
         */
        HashMapItem _iItem;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // map����
    //
    /**
     * mapͷ
     */
    struct tagMapHead
    {
        char   _cMaxVersion;        //��汾
        char   _cMinVersion;        //С�汾
        bool   _bReadOnly;          //�Ƿ�ֻ��
        bool   _bAutoErase;         //�Ƿ�����Զ���̭
        char   _cEraseMode;         //��̭��ʽ:0x00:����Get����̭, 0x01:����Set����̭
        size_t _iMemSize;           //�ڴ��С
        size_t _iMinDataSize;       //��С���ݿ��С
        size_t _iMaxDataSize;       //������ݿ��С
        float  _fFactor;            //����
        float   _fRadio;             //chunks����/hash����
        size_t _iElementCount;      //��Ԫ�ظ���
        size_t _iEraseCount;        //ÿ��ɾ������
        size_t _iDirtyCount;        //�����ݸ���
        size_t _iSetHead;           //Setʱ������ͷ��
        size_t _iSetTail;           //Setʱ������β��
        size_t _iGetHead;           //Getʱ������ͷ��
        size_t _iGetTail;           //Getʱ������β��
        size_t _iDirtyTail;         //��������β��
        time_t _iSyncTime;          //��дʱ��
        size_t _iUsedChunk;         //�Ѿ�ʹ�õ��ڴ��
        size_t _iGetCount;          //get����
        size_t _iHitCount;          //���д���
        size_t _iBackupTail;        //�ȱ�ָ��
        size_t _iSyncTail;          //��д����
		size_t _iOnlyKeyCount;		// OnlyKey����
        size_t _iReserve[4];       //����
    }__attribute__((packed));

    /**
     * ��Ҫ�޸ĵĵ�ַ
     */
    struct tagModifyData
    {
        size_t  _iModifyAddr;       //�޸ĵĵ�ַ
        char    _cBytes;            //�ֽ���
        size_t  _iModifyValue;      //ֵ
    }__attribute__((packed));

    /**
     * �޸����ݿ�ͷ��
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         //�޸�״̬: 0:Ŀǰû�����޸�, 1: ��ʼ׼���޸�, 2:�޸����, û��copy���ڴ���
        size_t          _iNowIndex;             //���µ�Ŀǰ������, ���ܲ���10��
        tagModifyData   _stModifyData[20];      //һ�����20���޸�
    }__attribute__((packed));

    /**
     * HashItem
     */
    struct tagHashItem
    {
        size_t   _iBlockAddr;     //ָ���������ƫ�Ƶ�ַ
        uint32_t _iListCount;     //�������
    }__attribute__((packed));

    //64λ����ϵͳ�û����汾��, 32λ����ϵͳ��64λ�汾��
#if __WORDSIZE == 64

    //����汾��
    enum
    {
        MAX_VERSION         = 0,    //��ǰmap�Ĵ�汾��
        MIN_VERSION         = 3,    //��ǰmap��С�汾��
    };

#else
    //����汾��
    enum
    {
        MAX_VERSION         = 0,    //��ǰmap�Ĵ�汾��
        MIN_VERSION         = 2,    //��ǰmap��С�汾��
    };

#endif

    //������̭��ʽ
    enum
    {
        ERASEBYGET          = 0x00, //����Get������̭
        ERASEBYSET          = 0x01, //����Set������̭
    };

    /**
     * get, set��int����ֵ
     */
    enum
    {
        RT_OK                   = 0,    //�ɹ�
        RT_DIRTY_DATA           = 1,    //������
        RT_NO_DATA              = 2,    //û������
        RT_NEED_SYNC            = 3,    //��Ҫ��д
        RT_NONEED_SYNC          = 4,    //����Ҫ��д
        RT_ERASE_OK             = 5,    //��̭���ݳɹ�
        RT_READONLY             = 6,    //mapֻ��
        RT_NO_MEMORY            = 7,    //�ڴ治��
        RT_ONLY_KEY             = 8,    //ֻ��Key, û��Value
        RT_NEED_BACKUP          = 9,    //��Ҫ����
        RT_NO_GET               = 10,   //û��GET��
        RT_DECODE_ERR           = -1,   //��������
        RT_EXCEPTION_ERR        = -2,   //�쳣
        RT_LOAD_DATA_ERR        = -3,   //���������쳣
        RT_VERSION_MISMATCH_ERR = -4,   //�汾��һ��
        RT_DUMP_FILE_ERR        = -5,   //dump���ļ�ʧ��
        RT_LOAL_FILE_ERR        = -6,   //load�ļ����ڴ�ʧ��
		RT_NOTALL_ERR           = -7,   //û�и�����ȫ
    };

    //���������
    typedef HashMapIterator     hash_iterator;
    typedef HashMapLockIterator lock_iterator;

    //����hash������
    typedef TC_Functor<size_t, TL::TLMaker<const string &>::Result> hash_functor;

    //////////////////////////////////////////////////////////////////////////////////////////////
    //map�Ľӿڶ���

    /**
     * ���캯��
     */
    TC_HashMap()
    : _iMinDataSize(0)
    , _iMaxDataSize(0)
    , _fFactor(1.0)
    , _fRadio(2)
    , _pDataAllocator(new BlockAllocator(this))
    , _lock_end(this, 0, 0, 0)
    , _end(this, (size_t)(-1))
    , _hashf(hash<string>())
    {
    }

    /**
     * ��ʼ�����ݿ�ƽ����С
     * ��ʾ�ڴ�����ʱ�򣬻����n����С�飬 n������С��*�������ӣ�, n������С��*��������*�������ӣ�..., ֱ��n������
     * n��hashmap�Լ����������
     * ���ַ������ͨ�������ݿ��¼�䳤�Ƚ϶��ʹ�ã� ���ڽ�Լ�ڴ棬������ݼ�¼�������Ǳ䳤�ģ� ����С��=���죬��������=1�Ϳ�����
     * @param iMinDataSize: ��С���ݿ��С
     * @param iMaxDataSize: ������ݿ��С
     * @param fFactor: ��������
     */
    void initDataBlockSize(size_t iMinDataSize, size_t iMaxDataSize, float fFactor);

    /**
     * ʼ��chunk���ݿ�/hash���ֵ, Ĭ����2,
     * ����Ҫ���ı�����create֮ǰ����
     *
     * @param fRadio
     */
    void initHashRadio(float fRadio)                { _fRadio = fRadio;}

    /**
     * ��ʼ��, ֮ǰ��Ҫ����:initDataAvgSize��initHashRadio
     * @param pAddr ���Ե�ַ
     * @param iSize ��С
     * @return ʧ�����׳��쳣
     */
    void create(void *pAddr, size_t iSize);

    /**
     * ���ӵ��ڴ��
     * @param pAddr, ��ַ
     * @param iSize, �ڴ��С
     * @return ʧ�����׳��쳣
     */
    void connect(void *pAddr, size_t iSize);

    /**
     * ԭ�������ݿ��������չ�ڴ�, ע��ͨ��ֻ�ܶ�mmap�ļ���Ч
     * (���iSize�ȱ������ڴ��С,�򷵻�-1)
     * @param pAddr, ��չ��Ŀռ�
     * @param iSize
     * @return 0:�ɹ�, -1:ʧ��
     */
    int append(void *pAddr, size_t iSize);

    /**
     * ��ȡÿ�ִ�С�ڴ���ͷ����Ϣ
     *
     * @return vector<TC_MemChunk::tagChunkHead>: ��ͬ��С�ڴ��ͷ����Ϣ
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail() { return _pDataAllocator->getBlockDetail(); }

    /**
     * ����block��chunk�ĸ���
     *
     * @return size_t
     */
    size_t allBlockChunkCount()                     { return _pDataAllocator->allBlockChunkCount(); }

    /**
     * ÿ��block��chunk�ĸ���(��ͬ��С�ڴ��ĸ�����ͬ)
     *
     * @return vector<size_t>
     */
    vector<size_t> singleBlockChunkCount()          { return _pDataAllocator->singleBlockChunkCount(); }

    /**
     * ��ȡhashͰ�ĸ���
     *
     * @return size_t
     */
    size_t getHashCount()                           { return _hash.size(); }

    /**
     * Ԫ�صĸ���
     *
     * @return size_t
     */
    size_t size()                                   { return _pHead->_iElementCount; }

    /**
     * ������Ԫ�ظ���
     *
     * @return size_t
     */
    size_t dirtyCount()                             { return _pHead->_iDirtyCount;}

	/**
     * OnlyKey����Ԫ�ظ���
     *
     * @return size_t
     */
    size_t onlyKeyCount()                             { return _pHead->_iOnlyKeyCount;}

    /**
     * ����ÿ����̭����
     * @param n
     */
    void setEraseCount(size_t n)                    { _pHead->_iEraseCount = n; }

    /**
     * ��ȡÿ����̭����
     *
     * @return size_t
     */
    size_t getEraseCount()                          { return _pHead->_iEraseCount; }

    /**
     * ����ֻ��
     * @param bReadOnly
     */
    void setReadOnly(bool bReadOnly)                { _pHead->_bReadOnly = bReadOnly; }

    /**
     * �Ƿ�ֻ��
     *
     * @return bool
     */
    bool isReadOnly()                               { return _pHead->_bReadOnly; }

    /**
     * �����Ƿ�����Զ���̭
     * @param bAutoErase
     */
    void setAutoErase(bool bAutoErase)              { _pHead->_bAutoErase = bAutoErase; }

    /**
     * �Ƿ�����Զ���̭
     *
     * @return bool
     */
    bool isAutoErase()                              { return _pHead->_bAutoErase; }

    /**
     * ������̭��ʽ
     * TC_HashMap::ERASEBYGET
     * TC_HashMap::ERASEBYSET
     * @param cEraseMode
     */
    void setEraseMode(char cEraseMode)              { _pHead->_cEraseMode = cEraseMode; }

    /**
     * ��ȡ��̭��ʽ
     *
     * @return bool
     */
    char getEraseMode()                             { return _pHead->_cEraseMode; }

    /**
     * ���û�дʱ��(��)
     * @param iSyncTime
     */
    void setSyncTime(time_t iSyncTime)              { _pHead->_iSyncTime = iSyncTime; }

    /**
     * ��ȡ��дʱ��
     *
     * @return time_t
     */
    time_t getSyncTime()                            { return _pHead->_iSyncTime; }

    /**
     * ��ȡͷ��������Ϣ
     * 
     * @return tagMapHead&
     */
    tagMapHead& getMapHead()                        { return *_pHead; }

    /**
     * ����hash��ʽ
     * @param hash_of
     */
    void setHashFunctor(hash_functor hashf)         { _hashf = hashf; }

    /**
     * ����hash������
     * 
     * @return hash_functor&
     */
    hash_functor &getHashFunctor()                  { return _hashf; }

    /**
     * hash item
     * @param index
     *
     * @return tagHashItem&
     */
    tagHashItem *item(size_t iIndex)                { return &_hash[iIndex]; }

    /**
     * dump���ļ�
     * @param sFile
     *
     * @return int
     *          RT_DUMP_FILE_ERR: dump���ļ�����
     *          RT_OK: dump���ļ��ɹ�
     */
    int dump2file(const string &sFile);

    /**
     * ���ļ�load
     * @param sFile
     *
     * @return int
     *          RT_LOAL_FILE_ERR: load����
     *          RT_VERSION_MISMATCH_ERR: �汾��һ��
     *          RT_OK: load�ɹ�
     */
    int load5file(const string &sFile);

    /**
     * �޸�hash����Ϊi��hash��(i���ܲ���hashmap������ֵ)
     * @param i
     * @param bRepair
     * 
     * @return int
     */
    int recover(size_t i, bool bRepair);

    /**
     * ���hashmap
     * ����map�����ݻָ�����ʼ״̬
     */
    void clear();

    /**
     * ������ݸɾ�״̬
     * @param k
     *
     * @return int
     *          RT_NO_DATA: û�е�ǰ����
     *          RT_ONLY_KEY:ֻ��Key
     *          RT_DIRTY_DATA: ��������
     *          RT_OK: �Ǹɾ�����
     *          ��������ֵ: ����
     */
    int checkDirty(const string &k);

    /**
     * ����Ϊ������, �޸�SETʱ����, �ᵼ�����ݻ�д
     * @param k
     *
     * @return int
     *          RT_READONLY: ֻ��
     *          RT_NO_DATA: û�е�ǰ����
     *          RT_ONLY_KEY:ֻ��Key
     *          RT_OK: ���������ݳɹ�
     *          ��������ֵ: ����
     */
    int setDirty(const string& k);

    /**
     * ����Ϊ�ɾ�����, �޸�SET��, �������ݲ���д
     * @param k
     *
     * @return int
     *          RT_READONLY: ֻ��
     *          RT_NO_DATA: û�е�ǰ����
     *          RT_ONLY_KEY:ֻ��Key
     *          RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int setClean(const string& k);

    /**
     * ��ȡ����, �޸�GETʱ����
     * @param k
     * @param v
     * @param iSyncTime:�����ϴλ�д��ʱ��
     *
     * @return int:
     *          RT_NO_DATA: û������
     *          RT_ONLY_KEY:ֻ��Key
     *          RT_OK:��ȡ���ݳɹ�
     *          ��������ֵ: ����
     */
    int get(const string& k, string &v, time_t &iSyncTime);

    /**
     * ��ȡ����, �޸�GETʱ����
     * @param k
     * @param v
     *
     * @return int:
     *          RT_NO_DATA: û������
     *          RT_ONLY_KEY:ֻ��Key
     *          RT_OK:��ȡ���ݳɹ�
     *          ��������ֵ: ����
     */
    int get(const string& k, string &v);

    /**
     * ��������, �޸�ʱ����, �ڴ治��ʱ���Զ���̭�ϵ�����
     * @param k: �ؼ���
     * @param v: ֵ
     * @param bDirty: �Ƿ���������
     * @param vtData: ����̭�ļ�¼
     * @return int:
     *          RT_READONLY: mapֻ��
     *          RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const string& k, const string& v, bool bDirty, vector<BlockData> &vtData);

    /**
     * ����key, ��������
     * @param k
     * @param vtData
     *
     * @return int
     *          RT_READONLY: mapֻ��
     *          RT_NO_MEMORY: û�пռ�(����̭��������»����)
     *          RT_OK: ���óɹ�
     *          ��������ֵ: ����
     */
    int set(const string& k, vector<BlockData> &vtData);

    /**
     * ɾ������
     * @param k, �ؼ���
     * @param data, ��ɾ���ļ�¼
     * @return int:
     *          RT_READONLY: mapֻ��
     *          RT_NO_DATA: û�е�ǰ����
     *          RT_ONLY_KEY:ֻ��Key, ɾ���ɹ�
     *          RT_OK: ɾ�����ݳɹ�
     *         ��������ֵ: ����
     */
    int del(const string& k, BlockData &data);

    /**
     * ��̭����, ÿ��ɾ��һ��, ����Getʱ����̭
     * �ⲿѭ�����øýӿ���̭����
     * ֱ��: Ԫ�ظ���/chunks * 100 < radio, bCheckDirty Ϊtrueʱ����������������̭����
     * @param radio: �����ڴ�chunksʹ�ñ��� 0< radio < 100
     * @param data: ��ǰ��ɾ����һ����¼
     * @return int:
     *          RT_READONLY: mapֻ��
     *          RT_OK: �����ټ�����̭��
     *          RT_ONLY_KEY:ֻ��Key, ɾ���ɹ�
     *          RT_DIRTY_DATA:�����������ݣ���bCheckDirty=trueʱ���п��ܲ������ַ���ֵ
     *          RT_ERASE_OK:��̭��ǰ���ݳɹ�, ������̭
     *          ��������ֵ: ����, ͨ������, ��������erase��̭
     */
    int erase(int radio, BlockData &data, bool bCheckDirty = false);

    /**
     * ��д, ÿ�η�����Ҫ��д��һ��
     * ���ݻ�дʱ���뵱ǰʱ�䳬��_pHead->_iSyncTime����Ҫ��д
     * _pHead->_iSyncTime��setSyncTime�����趨, Ĭ��10����

     * �ⲿѭ�����øú������л�д
     * mapֻ��ʱ��Ȼ���Ի�д
     * @param iNowTime: ��ǰʱ��
     *                  ��дʱ���뵱ǰʱ�����_pHead->_iSyncTime����Ҫ��д
     * @param data : ��д������
     * @return int:
     *          RT_OK: ������������ͷ����, ����sleepһ���ٳ���
     *          RT_ONLY_KEY:ֻ��Key, ɾ���ɹ�, ��ǰ���ݲ�Ҫ��д,��������sync��д
     *          RT_NEED_SYNC:��ǰ���ص�data������Ҫ��д
     *          RT_NONEED_SYNC:��ǰ���ص�data���ݲ���Ҫ��д
     *          ��������ֵ: ����, ͨ������, ��������sync��д
     */
    int sync(time_t iNowTime, BlockData &data);

    /**
     * ��ʼ��д, ������дָ��
     */
    void sync();

    /**
     * ��ʼ����֮ǰ���øú���
     *
     * @param bForceFromBegin: �Ƿ�ǿ����ͷ��ʼ����
     * @return void
     */
    void backup(bool bForceFromBegin = false);

    /**
     * ��ʼ��������, ÿ�η�����Ҫ���ݵ�һ������
     * @param data
     *
     * @return int
     *          RT_OK: �������
     *          RT_NEED_BACKUP:��ǰ���ص�data������Ҫ����
     *          RT_ONLY_KEY:ֻ��Key, ��ǰ���ݲ�Ҫ����
     *          ��������ֵ: ����, ͨ������, ��������backup
     */
    int backup(BlockData &data);

    /////////////////////////////////////////////////////////////////////////////////////////
    // �����Ǳ���map����, ��Ҫ��map����

    /**
     * ����
     *
     * @return
     */
    lock_iterator end() { return _lock_end; }


    /**
     * ����Key��������
     * @param string
     */
    lock_iterator find(const string& k);

    /**
     * block����
     *
     * @return lock_iterator
     */
    lock_iterator begin();

    /**
     * block����
     *
     * @return lock_iterator
     */
    lock_iterator rbegin();

    /**
     * ��Setʱ������ĵ�����
     *
     * @return lock_iterator
     */
    lock_iterator beginSetTime();

    /**
     * Set������ĵ�����
     *
     * @return lock_iterator
     */
    lock_iterator rbeginSetTime();

    /**
     * ��Getʱ������ĵ�����
     *
     * @return lock_iterator
     */
    lock_iterator beginGetTime();

    /**
     * Get������ĵ�����
     *
     * @return lock_iterator
     */
    lock_iterator rbeginGetTime();

    /**
     * ��ȡ������β��������(�ʱ��û�в�����������)
     *
     * ���صĵ�����++��ʾ����ʱ��˳��==>(���ʱ��û�в�����������)
     *
     * @return lock_iterator
     */
    lock_iterator beginDirty();

    /////////////////////////////////////////////////////////////////////////////////////////
    // �����Ǳ���map����, ����Ҫ��map����

    /**
     * ����hashͰ����
     * 
     * @return hash_iterator
     */
    hash_iterator hashBegin();

    /**
     * ����
     *
     * @return
     */
    hash_iterator hashEnd() { return _end; }
    
    /**
     * ����
     *
     * @return string
     */
    string desc();

    /**
     * �޸ĸ��µ��ڴ���
     */
    void doUpdate(bool bUpdate = false);

protected:

    friend class Block;
    friend class BlockAllocator;
    friend class HashMapIterator;
    friend class HashMapItem;
    friend class HashMapLockIterator;
    friend class HashMapLockItem;

    //��ֹcopy����
    TC_HashMap(const TC_HashMap &mcm);
    //��ֹ����
    TC_HashMap &operator=(const TC_HashMap &mcm);

    /**
     * ��ʼ��
     * @param pAddr
     */
    void init(void *pAddr);


    /**
     * ���������ݸ���
     */
    void incDirtyCount()    { update(&_pHead->_iDirtyCount, _pHead->_iDirtyCount+1); }

    /**
     * ���������ݸ���
     */
    void delDirtyCount()    { update(&_pHead->_iDirtyCount, _pHead->_iDirtyCount-1); }

    /**
     * �������ݸ���
     */
    void incElementCount()  { update(&_pHead->_iElementCount, _pHead->_iElementCount+1); }

    /**
     * �������ݸ���
     */
    void delElementCount()  { update(&_pHead->_iElementCount, _pHead->_iElementCount-1); }

	/**
     * ����OnlyKey���ݸ���
     */
	void incOnlyKeyCount()    { update(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount+1); }

	/**
     * ����OnlyKey���ݸ���
     */
	void delOnlyKeyCount()    { update(&_pHead->_iOnlyKeyCount, _pHead->_iOnlyKeyCount-1); }

    /**
     * ����Chunk��
     * ֱ�Ӹ���, ��Ϊ�п���һ�η����chunk����
     * �������������ڴ�ռ�, ����Խ�����
     */
    void incChunkCount()    { _pHead->_iUsedChunk++; }

    /**
     * ����Chunk��
     * ֱ�Ӹ���, ��Ϊ�п���һ���ͷŵ�chunk����
     * �������������ڴ�ռ�, ����Խ�����
     */
    void delChunkCount()    { _pHead->_iUsedChunk--; }

    /**
     * ����hit����
     */
    void incGetCount()      { update(&_pHead->_iGetCount, _pHead->_iGetCount+1); }

    /**
     * �������д���
     */
    void incHitCount()      { update(&_pHead->_iHitCount, _pHead->_iHitCount+1); }

    /**
     * ĳhash�������ݸ���+1
     * @param index
     */
    void incListCount(uint32_t index) { update(&item(index)->_iListCount, item(index)->_iListCount+1); }

    /**
     * ĳhash�������ݸ���+1
     * @param index
     */
    void delListCount(size_t index) { update(&item(index)->_iListCount, item(index)->_iListCount-1); }

    /**
     * ��Ե�ַ���ɾ��Ե�ַ
     * @param iAddr
     *
     * @return void*
     */
    void *getAbsolute(size_t iAddr) { return (void *)((char*)_pHead + iAddr); }

    /**
     * ���Ե�ַ������Ե�ַ
     *
     * @return size_t
     */
    size_t getRelative(void *pAddr) { return (char*)pAddr - (char*)_pHead; }

    /**
     * ��̭iNowAddr֮�������(������̭������̭)
     * @param iNowAddr, ��ǰBlock�������ڷ���ռ�, ���ܱ���̭
     *                  0��ʾ��ֱ�Ӹ�����̭������̭
     * @param vector<BlockData>, ����̭������
     * @return size_t,��̭�����ݸ���
     */
    size_t eraseExcept(size_t iNowAddr, vector<BlockData> &vtData);

    /**
     * ����Key����hashֵ
     * @param pKey
     * @param iKeyLen
     *
     * @return size_t
     */
    size_t hashIndex(const string& k);

    /**
     * ����Key��������
     *
     */
    lock_iterator find(const string& k, size_t index, string &v, int &ret);

	/**
     * ����Key��������
     * @param mb
     */
    lock_iterator find(const string& k, size_t index, int &ret);

    /**
     * ����hash������
     * @param iMaxHash
     * @param iMinHash
     * @param fAvgHash
     */
    void analyseHash(uint32_t &iMaxHash, uint32_t &iMinHash, float &fAvgHash);

    /**
     * �޸ľ����ֵ
     * @param iModifyAddr
     * @param iModifyValue
     */
    void update(void* iModifyAddr, size_t iModifyValue);

#if __WORDSIZE == 64
	void update(void* iModifyAddr, uint32_t iModifyValue);
#endif

	/**
     *
     * @param iModifyAddr
     * @param iModifyValue
     */
    void update(void* iModifyAddr, bool bModifyValue);

    /**
     * ��ȡ����n����n���������
     * @param n
     *
     * @return size_t
     */
    size_t getMinPrimeNumber(size_t n);

protected:

    /**
     * ����ָ��
     */
    tagMapHead                  *_pHead;

    /**
     * ��С�����ݿ��С
     */
    size_t                      _iMinDataSize;

    /**
     * �������ݿ��С
     */
    size_t                      _iMaxDataSize;

    /**
     * �仯����
     */
    float                       _fFactor;

    /**
     * ����chunk���ݿ�/hash���ֵ
     */
    float                       _fRadio;

    /**
     * hash����
     */
    TC_MemVector<tagHashItem>   _hash;

    /**
     * �޸����ݿ�
     */
    tagModifyHead               *_pstModifyHead;

    /**
     * block����������
     */
    BlockAllocator              *_pDataAllocator;

    /**
     * β��
     */
    lock_iterator               _lock_end;

    /**
     * β��
     */
    hash_iterator               _end;

    /**
     * hashֵ���㹫ʽ
     */
    hash_functor                _hashf;
};

}

#endif


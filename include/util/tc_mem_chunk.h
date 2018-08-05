#ifndef	__TC_MEM_CHUNK_H__
#define __TC_MEM_CHUNK_H__

#include <string>
#include <vector>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �ڴ������
// Author : j@syswin.com              
// Author : .com              
/////////////////////////////////////////////////
/**
* ԭʼ�ڴ��, ��TC_MemChunkAllocator����������ά��
*
* ���������ڴ�ֳɴ�С��ͬ�Ŀ�, �γ�����
* ���ܹ�������ͷ���Щ��С��ͬ�Ŀ�
*/
class TC_MemChunk
{
public:

    /**
    * ���캯��
    */
    TC_MemChunk();

    /**
     * ����Chunk��Ҫ���ڴ���С
     * @param iBlockSize
     * @param iBlockCount
     *
     * @return size_t
     */
    static size_t calcMemSize(size_t iBlockSize, size_t iBlockCount);

    /**
     * ����block����
     * @param iMemSize
     * @param iBlockSize
     *
     * @return size_t
     */
    static size_t calcBlockCount(size_t iMemSize, size_t iBlockSize);

     /**
     * tagChunkHead�Ĵ�С
     *
     * @return size_t
     */
    static size_t getHeadSize() { return sizeof(tagChunkHead); }

    /**
     * ��ʼ��, Ҫ��֤pָ����ڴ�ָ��=getMemSize��С
     * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
     * @param iBlockSize, block��С
     * @param iBlockCount, block����
     */
    void create(void *pAddr, size_t iBlockSize, size_t iBlockCount);

    /**
     * ������
     * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
     */
    void connect(void *pAddr);

    /**
     * block�Ĵ�С
     *
     * @return size_t
     */
    size_t getBlockSize() const { return _pHead->_iBlockSize; }

    /**
     * ���е��ڴ��С
     *
     * @return size_t
     */
    size_t getMemSize() const { return _pHead->_iBlockSize * _pHead->_iBlockCount + sizeof(tagChunkHead); }

    /**
     * ���Դ�����ݵ�������
     *
     * @return size_t
     */
    size_t getCapacity() const { return _pHead->_iBlockSize * _pHead->_iBlockCount; }

    /**
     * block�ĸ���
     *
     * @return unsigned char
     */
    size_t getBlockCount() const { return _pHead->_iBlockCount; }

    /**
     * �Ƿ��п���block
     *
     * @return bool
     */
    bool isBlockAvailable() const { return _pHead->_blockAvailable > 0; }

    /**
     * �������õ�block�ĸ���
     *
     * @return unsigned char
     */
    size_t getBlockAvailableCount() const { return _pHead->_blockAvailable; }

    /**
     * ����һ������
     *
     * @return void*
     */
    void* allocate();

	/**
    * ����һ�����飬������1Ϊ����������������û�пɷ���ռ�ʱ����0
    * ͨ���������ǱȽ�С(��ʹ��64λ����ϵͳ��), 4���ֽ�����
    * ���ڽ�ʡ�ڴ�
	*/
	void* allocate2(size_t &iIndex);

    /**
     * �ͷ�����
     * @param p
     */
    void deallocate(void *pAddr);

    /**
     * ���������ͷ�����
     * @param iIndex
     */
	void deallocate2(size_t iIndex);

    /**
     * �ؽ�
     */
    void rebuild();

    /**
     * chunkͷ��
     */
    struct tagChunkHead
    {
        size_t  _iBlockSize;            /**�����С*/
        size_t  _iBlockCount;           /**block����*/
        size_t  _firstAvailableBlock;   /**��һ�����õ�block����*/
        size_t  _blockAvailable;        /**����block����*/
    }__attribute__((packed));

    /**
     * ��ȡͷ����Ϣ
     *
     * @return tagChunkHead
     */
    tagChunkHead getChunkHead() const;

	/**
	* ����������ȡ���Ե�ַ
	*/
	void* getAbsolute(size_t iIndex);

    /**
     * ���Ե�ַ��������
     * 
     * @param pAddr 
     * 
     * @return size_t 
     */
	size_t getRelative(void *pAddr);

protected:
    /**
     * ��ʼ��
     */
    void init(void *pAddr);

private:

    /**
     * ����ͷָ��
     */
    tagChunkHead    *_pHead;

    /**
     * ������ָ��
     */
    unsigned char   *_pData;
};

/**
* �ڴ�������
* ���²��ԭʼ�ڴ�����
* �ڴ�ṹ:
* �ڴ�鳤��, 4���ֽ�
* Block��С, 4���ֽ�
* Chunk����, 4���ֽ�
* TC_MemChunk
* ��ʱֻ֧��ͬһ��Block��С��MemChunk
*/
class TC_MemChunkAllocator
{
public:

    /**
    * ���캯��
    */
    TC_MemChunkAllocator();

    /**
     * ��ʼ��
     * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
     * @param iSize, �ڴ��С
     * @param iBlockSize, block�Ĵ�С
     */
    void create(void *pAddr, size_t iSize, size_t iBlockSize);

    /**
     * ������
     * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
     */
    void connect(void *pAddr);

	/**
	 * ͷ��ַָ��
	 */
	void *getHead()	const       { return _pHead; }

    /**
     * ÿ��block�Ĵ�С
     *
     * @return size_t
     */
    size_t getBlockSize()  const { return _pHead->_iBlockSize; }

    /**
     * �ܼ��ڴ��С
     *
     * @return size_t
     */
    size_t getMemSize()  const { return _pHead->_iSize; }

    /**
     * ���Դ�����ݵ�������
     *
     * @return size_t
     */
    size_t getCapacity() const { return _chunk.getCapacity(); }

    /**
     * ����һ������,���Ե�ַ
     *
     * @return void*
     */
    void* allocate();

	/**
	* ����һ�����飬������1Ϊ����������������û�пɷ���ռ�ʱ����0
	*/
	void* allocate2(size_t &iIndex);

    /**
     * �ͷ�����, ���Ե�ַ
     * @param p
     */
    void deallocate(void *pAddr);

    /**
     * �ͷ�����, ��������
     * @param p
     */
    void deallocate2(size_t iIndex);

    /**
     * ����chunk������ϼƵ�block�ĸ���
     *
     * @return size_t
     */
    size_t blockCount() const           { return _chunk.getBlockCount(); }

	/**
	* ����������ȡ���Ե�ַ
	*/
	void* getAbsolute(size_t iIndex)    { return _chunk.getAbsolute(iIndex); };

    /**
     * ���Ե�ַ��������
     * @param pAddr 
     * 
     * @return size_t 
     */
	size_t getRelative(void *pAddr)     { return _chunk.getRelative(pAddr); };

    /**
     * ��ȡͷ����Ϣ
     *
     * @return TC_MemChunk::tagChunkHead
     */
    TC_MemChunk::tagChunkHead getBlockDetail() const;

    /**
     * �ؽ�
     */
    void rebuild();

    /**
     * ͷ���ڴ��
     */
    struct tagChunkAllocatorHead
    {
        size_t  _iSize;
        size_t  _iBlockSize;
    }__attribute__((packed));

    /**
     * ͷ����С
     *
     * @return size_t
     */
    static size_t getHeadSize() { return sizeof(tagChunkAllocatorHead); }

protected:

    /**
     * ��ʼ��
     */
    void init(void *pAddr);

    /**
     * ��ʼ��
     */
    void initChunk();

    /**
     * ����
     */
    void connectChunk();

    //������copy����
    TC_MemChunkAllocator(const TC_MemChunkAllocator &);
    //������ֵ
    TC_MemChunkAllocator& operator=(const TC_MemChunkAllocator &);
    bool operator==(const TC_MemChunkAllocator &mca) const;
    bool operator!=(const TC_MemChunkAllocator &mca) const;

private:

    /**
     * ͷָ��
     */
    tagChunkAllocatorHead   *_pHead;

    /**
     *  chunk��ʼ��ָ��
     */
    void                    *_pChunk;

    /**
     * chunk����
     */
    TC_MemChunk             _chunk;
};

/**
 * ��������,���Է�������ͬ��С�Ŀ�
 *
 * ��ʼ��ʱָ��:��С���С, �����С, ����С��ֵ
 * �Զ��������ĸ���(ÿ�ִ�С��ĸ�����ͬ)
 *
 */
class TC_MemMultiChunkAllocator
{
public:

    /**
    * ���캯��
    */
    TC_MemMultiChunkAllocator();

    /**
     * ��������
     */
    ~TC_MemMultiChunkAllocator();


    /**
     * ��ʼ��
     * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
     * @param iSize, �ڴ��С
     * @param iMinBlockSize, block�Ĵ�С
     * @param iMaxBlockSize,
     * @param fFactor, ����
     */
    void create(void *pAddr, size_t iSize, size_t iMinBlockSize, size_t iMaxBlockSize, float fFactor = 1.1);

    /**
     * ������
     * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
     */
    void connect(void *pAddr);

    /**
     * ��չ�ռ�
     * 
     * @param pAddr, �Ѿ��ǿռ䱻��չ֮��ĵ�ַ
     * @param iSize
     */
    void append(void *pAddr, size_t iSize);

    /**
     * ÿ��block�Ĵ�С, �����������ӵ��ڴ��Ĵ�С
     *
     * @return vector<size_t>
     */
    vector<size_t> getBlockSize()  const;

    /**
     * ÿ��block��chunk����(������ȵ�)
     * 
     * @return size_t 
     */
    size_t getBlockCount() const { return _iBlockCount; }

    /**
     * ��ȡÿ����ͷ����Ϣ, �����������ӵ��ڴ��Ĵ�С
     * @param i
     *
     * @return vector<TC_MemChunk::tagChunkHead>
     */
    vector<TC_MemChunk::tagChunkHead> getBlockDetail() const;

    /**
     * �ܼ��ڴ��С, �����������ӵ��ڴ��Ĵ�С
     *
     * @return size_t
     */
    size_t getMemSize()  const              { return _pHead->_iTotalSize; }

    /**
     * �������Է����ݵ�����, �����������ӵ��ڴ�����������
     *
     * @return size_t
     */
    size_t getCapacity() const;

    /**
     * һ��chunk��block����, �����������ӵ��ڴ���
     *
     * @return vector<size_t>
     */
    vector<size_t> singleBlockChunkCount() const;

    /**
     * ����chunk������ϼƵ�block�ĸ���
     *
     * @return size_t
     */
    size_t allBlockChunkCount() const;

    /**
     * ����һ������,���Ե�ַ
     *
     * @param iNeedSize,��Ҫ����Ĵ�С
     * @param iAllocSize, ��������ݿ��С
     * @return void*
     */
    void* allocate(size_t iNeedSize, size_t &iAllocSize);

    /**
     * ����һ������, ������������
     * @param iNeedSize,��Ҫ����Ĵ�С
     * @param iAllocSize, ��������ݿ��С
     * @param size_t����1Ϊ������������0��ʾ��Ч
     * @return void *
     */
    void* allocate2(size_t iNeedSize, size_t &iAllocSize, size_t &iIndex);

    /**
     * �ͷ�����, ���Ե�ַ
     * @param p
     */
    void deallocate(void *pAddr);

    /**
     * �ͷ�����, ��������
     * @param p
     */
    void deallocate2(size_t iIndex);

    /**
     * �ؽ�
     */
    void rebuild();

    /**
     * �����������ɾ��Ե�ַ
     * @param iIndex
     *
     * @return void*
     */
    void *getAbsolute(size_t iIndex);

    /**
     * ���Ե�ַ����������ַ
     *
     * @return size_t
     */
    size_t getRelative(void *pAddr);

    /**
     * ͷ���ڴ��
     */
    struct tagChunkAllocatorHead
    {
        size_t  _iSize;             //��ǰ���С
        size_t  _iTotalSize;        //������������һ��Ĵ�С 
        size_t  _iMinBlockSize;
        size_t  _iMaxBlockSize;
        float   _fFactor;
        size_t  _iNext;             //��һ����������ַ, ���û����Ϊ0
    }__attribute__((packed));

    /**
     * ͷ����С
     *
     * @return size_t
     */
    static size_t getHeadSize() { return sizeof(tagChunkAllocatorHead); }

protected:

    /**
     * ��ʼ��
     */
    void init(void *pAddr);

    /**
     * ����
     */
	void calc();

    /**
     * ���
     */
    void clear();

    /**
     * ���һ��������
     * 
     * @return TC_MemMultiChunkAllocator*
     */
    TC_MemMultiChunkAllocator *lastAlloc();

    //������copy����
    TC_MemMultiChunkAllocator(const TC_MemMultiChunkAllocator &);
    //������ֵ
    TC_MemMultiChunkAllocator& operator=(const TC_MemMultiChunkAllocator &);
    bool operator==(const TC_MemMultiChunkAllocator &mca) const;
    bool operator!=(const TC_MemMultiChunkAllocator &mca) const;

private:

    /**
     * ͷָ��
     */
    tagChunkAllocatorHead   *_pHead;

    /**
     *  chunk��ʼ��ָ��
     */
    void                    *_pChunk;

    /**
     * �����С
     */
    vector<size_t>          _vBlockSize;

    /**
     * ÿ��chunk��block�ĸ���
     */
    size_t                  _iBlockCount;

    /**
     * chunk����
     */
    vector<TC_MemChunkAllocator*>       _allocator;

    /**
     * ���е���������
     */
    size_t                              _iAllIndex;

    /**
     * �����Ķ�������
     */
    TC_MemMultiChunkAllocator           *_nallocator;
};

}

#endif

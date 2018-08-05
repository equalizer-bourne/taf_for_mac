#ifndef	__TC_MEM_VECTOR_H__
#define __TC_MEM_VECTOR_H__

#include "util/tc_ex.h"
#include <sstream>
#include <string.h>

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �����ڴ�������
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* �쳣��
*/
struct TC_MemVectorException : public TC_Exception
{
	TC_MemVectorException(const string &buffer) : TC_Exception(buffer){};
    TC_MemVectorException(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_MemVectorException() throw(){};
};

/**
* �����ڴ�����������ݿ�
* ÿ�����ݿ��С���
* ģ�����Tֻ���Ǽ򵥵���������, ��Ҫ�߱�bit-copy���������
*/
template<typename T>
class TC_MemVector
{
public:

    /**
    * ���캯��
    */
    TC_MemVector() : _pHead(NULL) ,_pData(NULL)
    {
    }

    /**
     *
     * @param mv
     */
    TC_MemVector(const TC_MemVector<T> &mv)
    : _pHead(mv._pHead) ,_pData(mv._pData)
    {

    }

    /**
     *
     * @param mv
     *
     * @return bool
     */
    bool operator==(const TC_MemVector<T> &mv)
    {
        return _pHead == mv._pHead && _pData == mv._pData;
    }

    /**
     *
     * @param mv
     *
     * @return bool
     */
    bool operator!=(const TC_MemVector<T> &mv)
    {
        return _pHead != mv._pHead || _pData != mv._pData;
    }

    /**
     * ������Ҫ���ڴ�ռ�
     * @param iCount: ���ݸ���
     *
     * @return size_t
     */
    static size_t calcMemSize(size_t iCount)
    {
        return sizeof(T) * iCount + sizeof(tagMemQueueHead);
    }

    /**
    * ��ʼ��
    * @param pAddr: ָ����пռ��ָ��
    * @param iSize: �ռ��ָ��
    * @param iBlockSize: ÿ��block�Ĵ�С
    * @return void
    */
    void create(void *pAddr, size_t iSize);

    /**
    * �����϶���
    * @param pAddr: ָ����пռ��ָ��
    * @return void
    */
    void connect(void *pAddr) { init(pAddr); assert(_pHead->_iBlockSize == sizeof(T)); }

    /**
    * Ԫ�ظ���
    * @return size_t : ���г���
    */
    size_t size() { return _pHead->_iBlockCount; }

    /**
    * �����ڴ泤��
    * @return size_t : �����ڴ泤��
    */
    size_t getMemSize() { return _pHead->_iSize; }

    /**
     * �ؽ�
     */
    void clear();

    /**
     *
     * @param simple
     *
     * @return string
     */
    string desc() const;

    /**
     * ������
     */
    class TC_MemVectorIterator : public std::iterator<std::random_access_iterator_tag, T>
    {
    public:
        /**
         * ���캯��
         * @param pmv
         * @param iIndex
         */
        TC_MemVectorIterator(TC_MemVector *pmv, size_t iIndex) : _pmv(pmv), _iIndex(iIndex)
        {
        }

        /**
         * ǰ��++
         *
         * @return TC_MemVectorIterator&
         */
        TC_MemVectorIterator& operator++()
        {
            ++_iIndex;
            return *this;
        }

        /**
         * ����++
         */
        TC_MemVectorIterator operator++(int)
        {
            TC_MemVectorIterator tmp = *this;

            ++_iIndex;
            return tmp;
        }

        /**
         *
         * @param mv
         *
         * @return TC_MemVectorIterator
         */
        bool operator==(const TC_MemVectorIterator& mv)
        {
            return _iIndex == mv._iIndex && _pmv == mv._pmv;
        }

        /**
         *
         * @param mv
         *
         * @return TC_MemVectorIterator
         */
        bool operator!=(const TC_MemVectorIterator& mv)
        {
            return _iIndex != mv._iIndex || _pmv != mv._pmv;
        }

        /**
         *
         *
         * @return T&
         */
        T& operator*() const { return (*_pmv)[_iIndex]; }

        /**
         *
         *
         * @return T*
         */
        T* operator->() const { return &(*_pmv)[_iIndex]; }

    private:
        /**
         *
         */
        TC_MemVector    *_pmv;

        /**
         *
         */
        size_t          _iIndex;
    };

    typedef TC_MemVectorIterator iterator;

    /**
     *
     *
     * @return TC_MemVectorIterator
     */
    TC_MemVectorIterator begin()	{ return TC_MemVectorIterator(this, 0); }

    /**
     *
     *
     * @return TC_MemVectorIterator
     */
    TC_MemVectorIterator end()		{ return TC_MemVectorIterator(this, _pHead->_iBlockCount); }

    /**
     * ��ȡ����
     * @param pData
     * @param iDataLen
     */
    T& operator[](size_t iIndex)
    {
        if(iIndex >= _pHead->_iBlockCount)
        {
			ostringstream s;
			s << string("[TC_MemVector::get] index beyond : index = ") << iIndex << " > " << _pHead->_iBlockCount;

            throw TC_MemVectorException(s.str());
        }

        return *(T*)((char*)_pData + iIndex * _pHead->_iBlockSize);
    }

    /**
     * ��ȡͷ��ַ
     *
     * @return void*
     */
    void *getAddr() { return (void*)_pHead; }

    /**
    * ���п��ƽṹ
    */
    struct tagMemQueueHead
    {
        size_t _iSize;          //�ڴ��С
        size_t _iBlockCount;    //Ԫ�ظ���
        size_t _iBlockSize;     //�����С
    }__attribute__((packed));

protected:

    /**
     *
     * @param pAddr
     */
    void init(void *pAddr)
    {
        _pHead = static_cast<tagMemQueueHead*>(pAddr);
        _pData = (char*)_pHead + sizeof(tagMemQueueHead);
    }


    /**
    * ���п��ƿ�
    */
    tagMemQueueHead *_pHead;

    /**
    * �����ڴ��ַ
    */
    void            *_pData;
};

template<typename T>
void TC_MemVector<T>::create(void *pAddr, size_t iSize)
{
    size_t iBlockSize = sizeof(T);

    if(iSize <= sizeof(tagMemQueueHead) || ((iSize - sizeof(tagMemQueueHead)) / iBlockSize == 0))
    {
        throw TC_MemVectorException("[TC_MemVector::create] memory size not enough.");
    }

    init(pAddr);

    memset(pAddr, 0x00, iSize);

    _pHead->_iSize          = iSize;
    _pHead->_iBlockCount    = (iSize - sizeof(tagMemQueueHead)) / iBlockSize;
    _pHead->_iBlockSize     = iBlockSize;
}

template<typename T>
void TC_MemVector<T>::clear()
{
    assert(_pHead);

    memset(_pData, 0x00, _pHead->_iBlockSize * _pHead->_iBlockCount);
}

template<typename T>
string TC_MemVector<T>::desc() const
{
    ostringstream s;
    s << "[TC_MemVector] [_iSize=" << _pHead->_iSize << "] "
      << "[_iBlockCount="  << _pHead->_iBlockCount << "] "
      << "[_iBlockSize="  << _pHead->_iBlockSize << "] "
      << endl;
    s << "[~TC_MemVector]";

    return s.str();
}

}

#endif

#ifndef	__TC_BIT_MAP_H__
#define __TC_BIT_MAP_H__

#include <iostream>
#include <string>
#include <vector>
#include "util/tc_ex.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: ��λbitmap��
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
 * �쳣
 */
struct TC_BitMap_Exception : public TC_Exception
{
	TC_BitMap_Exception(const string &buffer) : TC_Exception(buffer){};
	~TC_BitMap_Exception() throw(){};
};


/**
 * ֧�ֶ�bit��bitmap
 * һ��������, ֧�ֶ��Bit
 * 
 */
class TC_BitMap
{
public:
    /**
     * �ڴ��bitmap
     * ÿ����������1λ
     *
     */
    class BitMap
    {
    public:
    	
        static const int _magic_bits[8];

        #define _set_bit(n,m)   (n|_magic_bits[m])
        #define _clear_bit(n,m) (n&(~_magic_bits[m]))
        #define _get_bit(n,m)   (n&_magic_bits[m])

        //�����ڴ�汾
        #define BM_VERSION      1

        /**
         * ����Ԫ�ظ���������Ҫ�ڴ�Ĵ�С
         * @param iElementCount, ��Ҫ�����Ԫ�ظ���(Ԫ�ش�0��ʼ��)
         * 
         * @return size_t
         */
        static size_t calcMemSize(size_t iElementCount);

        /**
         * ��ʼ��
         * @param pAddr ���Ե�ַ
         * @param iSize ��С, ����(calcMemSize)�������
         * @return 0: �ɹ�, -1:�ڴ治��
         */
        void create(void *pAddr, size_t iSize);

        /**
         * ���ӵ��ڴ��
         * @param pAddr  ,��ַ, ����(calcMemSize)�������
         * @return 0, �ɹ�, -1,�汾����, -2:��С����
         */
        int connect(void *pAddr, size_t iSize);

        /**
         * �Ƿ��б�ʶ
         * @param i
         * 
         * @return int, >0:�б�ʶ, =0:�ޱ�ʶ, <0:������Χ
         */
        int get(size_t i);

        /**
         * ���ñ�ʾ
         * @param i
         * 
         * @return int, >0:�б�ʶ, =0:�ޱ�ʶ, <0:������Χ
         */
        int set(size_t i);

        /**
         * �����ʶ
         * @param i
         * 
         * @return int, >0:�б�ʶ, =0:�ޱ�ʶ, <0:������Χ
         */
        int clear(size_t i);

		/**
		 * ������е�����
		 * 
		 * @author kevintian (2010-9-29)
		 * 
		 * @return int 
		 */
		int clear4all();

        /**
         * dump���ļ�
         * @param sFile
         * 
         * @return int
         */
        int dump2file(const string &sFile);

        /**
         * ���ļ�load
         * @param sFile
         * 
         * @return int
         */
        int load5file(const string &sFile);

        struct tagBitMapHead
        {
           char     _cVersion;          //�汾, ��ǰ�汾Ϊ1
           size_t   _iMemSize;          //�����ڴ��С
        }__attribute__((packed));

        /**
         * ��ȡͷ����ַ
         * 
         * @return tagBitMapHead*
         */
        BitMap::tagBitMapHead *getAddr() const   { return _pHead; }

        /**
         * �ڴ��С
         * 
         * @return size_t
         */
        size_t getMemSize() const                   { return _pHead->_iMemSize; }

    protected:

        /**
         * �����ڴ�ͷ��
         */
        tagBitMapHead               *_pHead;

        /**
         * ���ݿ�ָ��
         */
        unsigned char *             _pData;
    };

    /**
     * ����Ԫ�ظ���������Ҫ�ڴ�Ĵ�С
     * @param iElementCount, ��Ҫ�����Ԫ�ظ���(Ԫ�ش�0��ʼ��)
     * @param iBitCount, ÿ��Ԫ��֧�ּ�λ(Ĭ��1λ) (λ��>=1)
     * 
     * @return size_t
     */
    static size_t calcMemSize(size_t iElementCount, unsigned iBitCount = 1);

    /**
     * ��ʼ��
     * @param pAddr ���Ե�ַ
     * @param iSize ��С, ����(calcMemSize)�������
     * @return 0: �ɹ�, -1:�ڴ治��
     */
    void create(void *pAddr, size_t iSize, unsigned iBitCount = 1);

    /**
     * ���ӵ��ڴ��
     * @param pAddr  ,��ַ, ����(calcMemSize)�������
     * @return 0, �ɹ�, -1,�汾����, -2:��С����
     */
    int connect(void *pAddr, size_t iSize, unsigned iBitCount = 1);

    /**
     * �Ƿ��б�ʶ
     * @param i, Ԫ��ֵ
     * @param iBit, �ڼ�λ
     * 
     * @return int, >0:�б�ʶ, =0:�ޱ�ʶ, <0:������Χ
     */
    int get(size_t i, unsigned iBit = 1);

    /**
     * ���ñ�ʾ
     * @param i, Ԫ��ֵ
     * @param iBit, �ڼ�λ
     * 
     * @return int, >0:�б�ʶ, =0:�ޱ�ʶ, <0:������Χ
     */
    int set(size_t i, unsigned iBit = 1);

    /**
     * �����ʶ
     * @param i, Ԫ��ֵ
     * @param iBit, �ڼ�λ
     * 
     * @return int, >0:�б�ʶ, =0:�ޱ�ʶ, <0:������Χ
     */
    int clear(size_t i, unsigned iBit = 1);

	/**
	 * ������еı�ʶ
	 * 
	 * @author kevintian (2010-9-29)
	 * 
	 * @param iBit 
	 * 
	 * @return int 
	 */
	int clear4all(unsigned iBit = (unsigned)(-1));

    /**
     * dump���ļ�
     * @param sFile
     * 
     * @return int
     */
    int dump2file(const string &sFile);

    /**
     * ���ļ�load
     * @param sFile
     * 
     * @return int
     */
    int load5file(const string &sFile);

protected:
    vector<BitMap>   _bitmaps;
};

}

#endif


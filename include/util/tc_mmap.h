#ifndef __TC_MMAP_H
#define __TC_MMAP_H

#include <sys/mman.h>
#include <string>
#include "util/tc_ex.h"
using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: mmap��װ��
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* MMap�쳣
*/
struct TC_Mmap_Exception : public TC_Exception
{
    TC_Mmap_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Mmap_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Mmap_Exception() throw() {};
};


/**
* mmap�Ĳ�����
* ˵��:
* 1 fd����ָ��Ϊ-1����ʱ��ָ��flags�����е�MAP_ANONYMOUS
* �������е�������ӳ��,���漰������ļ���,�������ļ��Ĵ�������,����Ȼֻ�����ھ�����Ե��ϵ�Ľ��̼�ͨ��
* 2 ����mapʱ,�ļ�һ����Ҫ��length�ĳ���, ������ܵ���Խ��
* 3 2��˵�����һ��ͨ�������ն��ļ�����,int mmap(const char *file, size_t length);
*   ʵ�������Ƶķ�װ, �Ƽ�ֱ��ʹ��
*/
class TC_Mmap
{
public:

    /**
     * ���캯��
     * @param bOwner, ���ӵ��, ��������ʱ��unmap
     */
    TC_Mmap(bool bOwner = true);

    /**
     * ����
     */
    ~TC_Mmap();

    /**
     * ӳ�䵽���̿ռ�
     * @param length, ӳ�䵽���̵�ַ�ռ���ֽ���,��offset���ֽ�����
     * @param prot,ָ�������ڴ�ķ���Ȩ��,��ȡֵΪ(����ֵ�Ļ�):
     *              PROT_READ: �ɶ�
     *              PROT_WRITE: ��д
     *              PROT_EXEC : ��ִ��
     *              PROT_NONE: ���ɷ���
     * @param flags, MAP_SHARED,MAP_PRIVATE,MAP_FIXED:һ��ȡ:MAP_SHARED
     * @param fd, ���̿ռ��е��ļ�������
     * @param offset,�ļ���ƫ�Ƶ�ַ, һ��Ϊ0
     * @throws TC_Mmap_Exception
     * @return
     */
	void mmap(size_t length, int prot, int flags, int fd, off_t offset = 0);

    /**
     * ӳ�䵽���̿ռ�, ����:PROT_READ|PROT_WRITE,MAP_SHARED��ʽ
     * ע����ļ���С���length��һ���ֽ�(��ʼ��ʱ���ɿն��ļ���ԭ��)
     * @param file, �ļ���
     * @param length, ӳ���ļ��ĳ���
     * @throws TC_Mmap_Exception
     * @return
     */
	void mmap(const char *file, size_t length);

    /**
     * ���ӳ���ϵ, ��������ڷ�����οռ���
     * @throws TC_Mmap_Exception
     * @return
     */
	void munmap();

    /**
     * �ѹ����ڴ��еĸı�д�ش�����
     *
     * @param bSync, true:ͬ��д��, false:�첽д��
     * @throws TC_Mmap_Exception
     * @return
     */
	void msync(bool bSync = false);

    /**
     * ��ȡӳ���ָ���ַ
     *
     * @return void*
     */
    void *getPointer() const    { return _pAddr; }

    /**
     * ӳ��Ŀռ��С
     *
     * @return size_t
     */
    size_t getSize() const      { return _iLength; }

    /**
     * �Ƿ񴴽�������(�ļ��Ƿ��Ǵ�����, ���ǿ�ʼ�ʹ��ڵ�)
     *
     *
     * @return bool
     */
    bool iscreate() const       { return _bCreate; }

    /**
     * �����Ƿ�ӵ��
     * @param bOwner
     */
    void setOwner(bool bOwner)  { _bOwner = bOwner; }

protected:

    /**
     * �Ƿ�ӵ��
     */
    bool    _bOwner;

    /**
     * ӳ�䵽���̿ռ�ĵ�ַ
     */
    void    *_pAddr;

    /**
     * ӳ��Ŀռ��С
     */
    size_t  _iLength;

    /**
     * �Ƿ񴴽�������
     */
    bool    _bCreate;
};

}
#endif


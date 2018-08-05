#ifndef	__TC_SHM_H__
#define __TC_SHM_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �����ڴ��װ��
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* �����ڴ��쳣��
*/
struct TC_Shm_Exception : public TC_Exception
{
    TC_Shm_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Shm_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Shm_Exception() throw() {};
};

/**
* ˵��:
* 1 �����ڴ�������, �������ӹ����ڴ�, �����ڴ��Ȩ���� 0666
* 2 _bOwner=false: ����ʱ��detach�����ڴ�
* 3 _bOwner=true: ����ʱdetach�����ڴ�
*/
class TC_Shm
{
public:

    /**
    * ���캯��
    */
    TC_Shm(bool bOwner = false) : _bOwner(bOwner), _pshm(NULL) {}

    /**
    * ���캯��
    * @param iShmSize: �����ڴ��С
    * @param iKey:�����ڴ�Key
    * @throws TC_Shm_Exception
    */
    TC_Shm(size_t iShmSize, key_t iKey, bool bOwner = false);

    /**
    * ����
    */
    ~TC_Shm();

    /**
    * ��ʼ��
    * @param iShmSize: �����ڴ��С
    * @param iKey:�����ڴ�Key
    * @param bOwner:�Ƿ�ӵ�й����ڴ�
    * @throws TC_Shm_Exception
    * @return ��
    */
    void init(size_t iShmSize, key_t iKey, bool bOwner = false);

    /**
    * ���ɵĹ����ڴ�,���������ϵĹ����ڴ�
    * ��������ɵĹ����ڴ�,��ʱ���Ը�����Ҫ����ʼ��
    * @return bool: true,���ɹ����ڴ�; false, �����ϵĹ����ڴ�
    */
    bool iscreate()     {return _bCreate;}

    /**
    * ��ȡ�����ڴ��ָ��
    * @return void* ,�����ڴ�ָ��
    */
    void *getPointer() {return _pshm;}

    /**
    * ��ȡ�����ڴ�Key
    * @return key_t* ,�����ڴ�key
    */
    key_t getkey()  {return _shmKey;}

    /**
    * ��ȡ�����ڴ�ID
    * @return int ,�����ڴ�Id
    */
    int getid()     {return _shemID;}

    /**
    * �����ڴ��С
    * return size_t,�����ڴ��С
    */
    size_t size()   {return _shmSize;}

    /**
    * �ڵ�ǰ�����н�������ڴ�
    * �����ڴ��ڵ�ǰ��������Ч
    * return int
    */
    int detach();

    /**
     * ��ȫɾ�������ڴ�
     */
    int del();

protected:

    /**
     * �Ƿ�ӵ�й����ڴ�
     */
    bool            _bOwner;

    /**
    * �����ڴ��С
    */
    size_t          _shmSize;

    /**
    * �����ڴ�key
    */
    key_t           _shmKey;

    /**
    * �Ƿ������ɵĹ����ڴ�
    */
    bool            _bCreate;

    /**
    * �����ڴ�
    */
    void            *_pshm;

    /**
    * �����ڴ�id
    */
    int             _shemID;

};

}

#endif

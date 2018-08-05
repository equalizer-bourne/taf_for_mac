#ifndef __TC_SEM_MUTEX_H
#define __TC_SEM_MUTEX_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "util/tc_lock.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �ź�������
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* �ź������쳣��
*/
struct TC_SemMutex_Exception : public TC_Lock_Exception
{
    TC_SemMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_SemMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_SemMutex_Exception() throw() {};
};

/**
* ���̼���, �ṩ����������:���������ų���
* 1 ������ͬ��key, ��ͬ���̳�ʼ��ʱ���ӵ���ͬ��sem��
* 2 ����IPC���ź���ʵ��
* 3 �ź���������SEM_UNDO����, �����̽���ʱ���Զ������ź���
*/
class TC_SemMutex
{
public:
    /**
     * ���캯��
     */
    TC_SemMutex();

    /**
    * ���캯��
    * @param iKey, key
    * @throws TC_SemMutex_Exception
    */
    TC_SemMutex(key_t iKey);

    /**
    * ��ʼ��
    * @param iKey, key
    * @throws TC_SemMutex_Exception
    * @return ��
     */
    void init(key_t iKey);

    /**
    * ��ȡ�����ڴ�Key
    * @return key_t ,�����ڴ�key
    */
    key_t getkey() const {return _semKey;}

    /**
    * ��ȡ�����ڴ�ID
    * @return int ,�����ڴ�Id
    */
    int getid() const   {return _semID;}

    /**
    * �Ӷ���
    *@return int
    */
    int rlock() const;

    /**
    * �����
    * @return int
    */
    int unrlock() const;

    /**
    * ���Զ���
    * @return bool : �����ɹ��򷵻�false, ���򷵻�false
    */
    bool tryrlock() const;

    /**
    * ��д��
    * @return int
    */
    int wlock() const;

    /**
    * ��д��
    */
    int unwlock() const;

    /**
    * ����д��
    * @throws TC_SemMutex_Exception
    * @return bool : �����ɹ��򷵻�false, ���򷵻�false
    */
    bool trywlock() const;

    /**
    * д��
    * @return int, 0 ��ȷ
    */
    int lock() const        {return wlock();};

    /**
    * ��д��
    */
    int unlock() const      {return unwlock();};

    /**
    * ���Խ���
    * @throws TC_SemMutex_Exception
    * @return int, 0 ��ȷ
    */
    bool trylock() const    {return trywlock();};

protected:
    /**
     * �ź���ID
     */
    int _semID;

    /**
     * �ź���key
     */
    key_t _semKey;
};

}

#endif

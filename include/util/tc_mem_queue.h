#ifndef	__TC_MEM_QUEUE_H__
#define __TC_MEM_QUEUE_H__

#include <string>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �����ڴ�ѭ��������
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* �ڴ�ѭ������
* ͬʱ��д�������
* ���˱�������ʹ��kill�������в��ỵ����������һ������
*/
class TC_MemQueue
{
public:

    /**
    * ���캯��
    */
    TC_MemQueue();

    /**
    * ����
    */
    ~TC_MemQueue(){}

    /**
    * ��ʼ��
    * @param pAddr: ָ����пռ��ָ��
    * @param iSize: �ռ��ָ��
    */
    void create(void *pAddr, size_t iSize);

    /**
    * �����϶���
    * @param pAddr: ָ����пռ��ָ��
    */
    void connect(void *pAddr, size_t iSize);

    /**
    * ��������
    * @param : sOut,��������ݿ�
    * @return bool , true:��ȷ, false: ����, ���������, ���п�
    */
    bool pop_front(string &sOut);

    /**
    * ��������
    * @param : sIn, ��������ݿ�
    * @return bool , true:��ȷ, false: ����, ������
    */
    bool push_back(const string &sIn);

    /**
    * ��������
    * @param : pvIn, ��������ݿ�
    * @param : iSize, �������ݿ鳤��
    * @return bool , true:��ȷ, false: ����, ������
    */
    bool push_back(const char *pvIn, size_t iSize);

    /**
    * �����Ƿ���
    * @param : iSize, �������ݿ鳤��
    * @return bool , true:��, false: ����
    */
    bool isFull(size_t iSize);

    /**
    * �����Ƿ��
    * @return bool , true: ��, false: ����
    */
    bool isEmpty();

    /**
    * ������Ԫ�ظ���, ������������²���֤һ����ȷ
    * @return size_t, Ԫ�ظ���
    */
    size_t elementCount();

    /**
    * ���г���(�ֽ�), С���ܴ洢������(�ܴ洢�����Ȱ����˿��ƿ�)
    * @return size_t : ���г���
    */
    size_t queueSize();

    /**
    * �����ڴ泤��
    * @return size_t : �����ڴ泤��
    */
    size_t memSize() const {return _size;};

protected:
    /**
     * �޸ľ����ֵ
     * @param iModifyAddr
     * @param iModifyValue
     */
    void update(void* iModifyAddr, size_t iModifyValue);

	/**
     *
     * @param iModifyAddr
     * @param iModifyValue
     */
    void update(void* iModifyAddr, bool bModifyValue);

    /**
     * �޸ĸ��µ��ڴ���
     */
    void doUpdate(bool bUpdate = false);

protected:

    /**
    * ���п��ƽṹ
    */
    struct CONTROL_BLOCK
    {
        size_t iMemSize;            //�ڴ��С
        size_t iTopIndex;           //����Ԫ������
        size_t iBotIndex;           //�ײ�Ԫ������
        size_t iPushCount;          //�����н���Ԫ�صĸ���
        size_t iPopCount;           //�����е���Ԫ�صĸ���
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
        tagModifyData   _stModifyData[5];       //һ�����5���޸�
    }__attribute__((packed));

    /**
    * ���п��ƿ�(�ڴ�����)
    */
    CONTROL_BLOCK   *_pctrlBlock;

    /**
    * �ڴ����ݵ�ַ
    */
    void            *_paddr;

    /**
    * �����ڴ�
    */
    size_t		    _size;

    /**
     * �޸����ݿ�
     */
    tagModifyHead   *_pstModifyHead;


};

}

#endif

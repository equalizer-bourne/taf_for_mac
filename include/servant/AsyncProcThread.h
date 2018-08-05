#ifndef __TAF_ASYNC_PROC_THREAD_H_
#define __TAF_ASYNC_PROC_THREAD_H_

#include "servant/Message.h"
#include "servant/Global.h"

namespace taf
{
/**
 * �첽�ص����ҵ�����̣߳��߳�����client����
 */
///////////////////////////////////////////////////////
class AsyncProcThread : public TC_HandleBase
{
public:
    /**
     * ���캯��
     */
    AsyncProcThread(Communicator * pcomm);
    /**
     * ��������
     */
    virtual ~AsyncProcThread();

public:
    /**
     * ��������runner
     */
    void terminate();

    /**
     * ����ص���Ϣ�����У���ִ��
     * @param req
     */
    void put(const ReqMessagePtr& req);

    /**
     * �Ӷ���ͷ��ȡ��һ����ִ�е���Ϣ
     * @param req
     * @return bool
     */
    bool pop(ReqMessagePtr& req);

    /**
     * ����n��runner��ÿ��runnerһ���߳�
     */
    void start(int n);

protected:

    /**
     * ����
     */
    bool _terminate;

    /**
     * ��Ϣ����
     */
    TC_ThreadQueue<ReqMessagePtr> _queue;       

    /**
     * runner����(ÿ������ά��һ���߳�)
     */
    vector<AsyncProcThreadRunnerPtr> _runners;  

	Communicator * _pComm;


	friend class AsyncProcThreadRunner;
};

///////////////////////////////////////////////////////
class AsyncProcThreadRunner : public TC_Thread, public TC_HandleBase
{
public:
    /**
     * ���캯��
     * @param proc
     */
    AsyncProcThreadRunner(AsyncProcThread* proc);

    /**
     * ���������߳�
     */
    void terminate();

    /**
     * �Ӷ�����ȡ��Ϣ��ִ�лص��߼�
     */
    void run();

	/**
	 * �ϱ������е�������
	 */
	void report();
private:
    /**
     * �Ƿ���Ҫ�˳�
     */
    bool _terminate;

    /**
     * �ʷ���Ϣ���е�������
     */
    AsyncProcThread* _proc;

	/**
	 * �ϱ�ʱ��
	 */
	time_t _timeLastReport;
};
///////////////////////////////////////////////////////
}
#endif

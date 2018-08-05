#ifndef __TC_THREAD_POOL_H_
#define __TC_THREAD_POOL_H_

#include "util/tc_thread.h"
#include "util/tc_thread_queue.h"
#include "util/tc_monitor.h"
#include "util/tc_functor.h"

#include <vector>
#include <set>
#include <iostream>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �̳߳���,���loki�Լ�wbl��˼��
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* �߳��쳣
*/
struct TC_ThreadPool_Exception : public TC_Exception
{
	TC_ThreadPool_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadPool_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadPool_Exception() throw(){};
};


/**
 * ��ͨ�̳߳���, ��tc_functor, tc_functorwrapper���ʹ��.
 * ʹ�÷�ʽ˵��:
 * 1 ����tc_functorwrapper��װһ������
 * 2 ��tc_threadpool�Ե��ý���ִ��
 *
 * ����ʾ��������μ�:test/test_tc_thread_pool.cpp
 */
class TC_ThreadPool : public TC_ThreadLock
{
public:

    /**
     * ���캯��
     *
     */
    TC_ThreadPool();

    /**
     * ����, ��ֹͣ�����߳�
     */
    ~TC_ThreadPool();

    /**
     * ��ʼ��
     * @param num, �����̸߳���
     */
    void init(size_t num);

    /**
     * ��ȡ�̸߳���
     *
     * @return size_t
     */
    size_t getThreadNum()   { Lock sync(*this); return _jobthread.size(); }

    /**
     * ��ȡ�̳߳ص�������(exec��ӽ�ȥ��)
     *
     * @return size_t
     */
    size_t getJobNum()     { return _jobqueue.size(); }

    /**
     * ֹͣ�����߳�, �ȴ������߳�ֹͣ
     */
    void stop();

    /**
     * ���������߳�
     */
    void start();

    /**
     * ���������̲߳�, ִ�г�ʼ������
     * @param ParentFunctor
     * @param tf
     */
    template<class ParentFunctor>
    void start(const TC_FunctorWrapper<ParentFunctor> &tf)
    {
        for(size_t i = 0; i < _jobthread.size(); i++)
        {
            _startqueue.push_back(new TC_FunctorWrapper<ParentFunctor>(tf));
        }

        start();
    }

    /**
     * ��Ӷ����̳߳�ִ��
     * �ú������Ϸ���
     * �̳߳ص��߳�ִ�ж���
	 */
    template<class ParentFunctor>
	void exec(const TC_FunctorWrapper<ParentFunctor> &tf)
    {
        _jobqueue.push_back(new TC_FunctorWrapper<ParentFunctor>(tf));
    }

    /**
     * �ȴ����й���ȫ������(����������, �޿����߳�)
     *
     * @param millsecond, �ȴ���ʱ��(ms), -1:��Զ�ȴ�
     * @return true, ���й������������
     *         false,��ʱ�˳�
     */
    bool waitForAllDone(int millsecond = -1);

public:

    /**
     * �߳����ݻ���,�����̵߳�˽�����ݼ̳��ڸ���
     */
    class ThreadData
    {
    public:
        /**
         * ����
         */
        ThreadData(){};
        /**
         * ����
         */
        virtual ~ThreadData(){};

        /**
         * ��������
         * @param T
         *
         * @return ThreadData*
         */
        template<typename T>
        static T* makeThreadData()
        {
            return new T;
        }
    };

    /**
     * �����߳�����
     * @param p
     */
    static void setThreadData(ThreadData *p);

    /**
     * ��ȡ�߳�����
     *
     * @return ThreadData*
     */
    static ThreadData* getThreadData();

    /**
     * �����߳�����, key��Ҫ�Լ�ά��
     * @param pkey, �߳�˽������key
     * @param p
     */
    static void setThreadData(pthread_key_t pkey, ThreadData *p);

    /**
     * ��ȡ�߳�����, key��Ҫ�Լ�ά��
     * @param pkey, �߳�˽������key
     * @return ThreadData*
     */
    static ThreadData* getThreadData(pthread_key_t pkey);

protected:

    /**
     * �ͷ���Դ
     * @param p
     */
    static void destructor(void *p);

    /**
     * ��ʼ��key
     */
    class KeyInitialize
    {
    public:
        /**
         * ��ʼ��key
         */
        KeyInitialize()
        {
            int ret = pthread_key_create(&TC_ThreadPool::g_key, TC_ThreadPool::destructor);
            if(ret != 0)
            {
                throw TC_ThreadPool_Exception("[TC_ThreadPool::KeyInitialize] pthread_key_create error", ret);
            }
        }

        /**
         * �ͷ�key
         */
        ~KeyInitialize()
        {
            pthread_key_delete(TC_ThreadPool::g_key);
        }
    };

    /**
     * ��ʼ��key�Ŀ���
     */
    static KeyInitialize g_key_initialize;

    /**
     * ����key
     */
    static pthread_key_t g_key;

protected:
    /**
     * �̳߳��еĹ����߳�
     */
    class ThreadWorker : public TC_Thread
    {
    public:
        /**
         * �����̹߳��캯��
         * @param tpool
         */
        ThreadWorker(TC_ThreadPool *tpool);

        /**
         * ֪ͨ�����߳̽���
         */
        void terminate();

    protected:
        /**
         * ����
         */
        virtual void run();

    protected:
        /**
         * �̳߳�ָ��
         */
        TC_ThreadPool   *_tpool;

        /**
         * �Ƿ�����߳�
         */
        bool            _bTerminate;
    };

protected:

    /**
     * ���
     */
    void clear();

    /**
     * ��ȡ����, ���û������, ��ΪNULL
     *
     * @return TC_FunctorWrapperInterface*
     */
    TC_FunctorWrapperInterface *get(ThreadWorker *ptw);

    /**
     * ��ȡ��������
     *
     * @return TC_FunctorWrapperInterface*
     */
    TC_FunctorWrapperInterface *get();

    /**
     * ������һ���߳�
     * @param ptw
     */
    void idle(ThreadWorker *ptw);

    /**
     * ֪ͨ�ȴ�����������ϵĹ����߳�����
     */
    void notifyT();

    /**
     * �Ƿ������
     *
     * @return bool
     */
    bool finish();

    /**
     * �߳��˳�ʱ����
     */
    void exit();

    friend class ThreadWorker;
protected:

    /**
     * �������
     */
    TC_ThreadQueue<TC_FunctorWrapperInterface*> _jobqueue;

    /**
     * ��������
     */
    TC_ThreadQueue<TC_FunctorWrapperInterface*> _startqueue;

    /**
     * �����߳�
     */
    std::vector<ThreadWorker*>                  _jobthread;

    /**
     * ��æ�߳�
     */
    std::set<ThreadWorker*>                     _busthread;

    /**
     * ������е���
     */
    TC_ThreadLock                               _tmutex;

};

}
#endif


#include "util/tc_thread_pool.h"
#include "util/tc_common.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace taf;

TC_ThreadPool tpool;
TC_ThreadLock l;

/**
 * �߳�˽������
 */
class MyThreadData : public TC_ThreadPool::ThreadData
{
public:
    virtual ~MyThreadData()
    {
        cout << pthread_self() << endl;
    }

public:
    pthread_t _idata;
};

/**
 * �̳߳�ʼ��
 */
void threadInitialize()
{
    MyThreadData *p = TC_ThreadPool::ThreadData::makeThreadData<MyThreadData>();
    p->_idata = pthread_self();
    TC_ThreadPool::setThreadData(p);

    cout << p->_idata << endl;
}

/**
 * �̵߳��ù���
 * @param s
 * @param i
 */
void TestFunction3(const string &s, int i)
{
    MyThreadData *p = (MyThreadData*)TC_ThreadPool::getThreadData();
    assert(pthread_self() == p->_idata);

//    cout << pthread_self() << " | TestFunction3('" << s << "', " << i << ")" << endl;
}

/**
 * �����̳߳�
 */
void testThreadPool()
{
    //4���߳�
    tpool.init(4);

    TC_Functor<void> init(threadInitialize);
    TC_Functor<void>::wrapper_type iwt(init);
    //�����߳�, ָ����ʼ������,Ҳ����û�г�ʼ������:tpool.start();
    tpool.start(iwt);

    string s("a");
    int i = 1000000;

    //����i��
    TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd(TestFunction3);
    while(i)
    {
        TC_Functor<void, TL::TLMaker<const string&, int>::Result>::wrapper_type fw(cmd, s, i);
        tpool.exec(fw);
        --i;
    } 

    //�ȴ��߳̽���
    cout << "waitForAllDone..." << endl;
    bool b = tpool.waitForAllDone(1000);
    cout << "waitForAllDone..." << b << ":" << tpool.getJobNum() << endl;

    //ֹͣ�߳�,������ʱ��Ҳ���Զ�ֹͣ�߳�
    //�߳̽���ʱ,���Զ��ͷ�˽������
    tpool.stop();
}

void test(int i, string &s)
{
    cout << i << ":" << s << endl;
    s = TC_Common::tostr(i + 10);
}

/**
 * �����̳߳�
 */
void testThreadPool1()
{
    //4���߳�
    tpool.init(1);

    //�����߳�, ָ����ʼ������,Ҳ����û�г�ʼ������:tpool.start();
    tpool.start();

    typedef void (*TpMem)(int, string&);
	TC_Functor<void, TL::TLMaker<int, string&>::Result> cmd(static_cast<TpMem>(&test));

    string bid;
   	for(int i=0; i<10; i++)
   	{
        cout << bid << endl;

   		bid = TC_Common::tostr(i);

   		cout << "index = " << i << ",bid = " << bid << endl;

   		TC_Functor<void, TL::TLMaker<int, string&>::Result>::wrapper_type fwrapper(cmd, i, bid);
   		tpool.exec(fwrapper);

//        sleep(1);
   	}

    //�ȴ��߳̽���
    cout << "waitForAllDone..." << endl;
    bool b = tpool.waitForAllDone(1000);
    cout << "waitForAllDone..." << b << ":" << tpool.getJobNum() << endl;

    //ֹͣ�߳�,������ʱ��Ҳ���Զ�ֹͣ�߳�
    //�߳̽���ʱ,���Զ��ͷ�˽������
    tpool.stop();
}
 
int main(int argc, char *argv[])
{
    try
    {
        testThreadPool1(); 
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}



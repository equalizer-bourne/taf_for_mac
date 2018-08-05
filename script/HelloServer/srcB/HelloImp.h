#ifndef _HELLO_IMP_H_
#define _HELLO_IMP_H_

#include "Hello.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

struct HelloCallback : public HelloPrxCallback
{
	HelloCallback(JceCurrentPtr &current): _current(current){}

	//�ص�����
	virtual void callback_testHello(int ret, const string &r)
	{
		assert(ret == 0);
//		assert(r == "hello world");
		cout << "callback:" << r << endl;
		Hello::async_response_testHello(_current, ret, r);
	}

	virtual void callback_testHello_exception(taf::Int32 ret)
	{
//		assert(ret == 0);
		cout << "callback exception:" << ret << endl;
	}

	JceCurrentPtr _current;
};

class HelloImp : public Hello
{
public:
    /**
     *
     */
    HelloImp(){};

     /**
     * ��ʼ����Hello�����⺯����HelloImp��ʼ��ʱ����
     *
     * @return int
     */
    virtual void initialize()
	{
//		pPrx1 = HelloServer::getCommunicator()->stringToProxy<HelloPrx>("Test.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 44444");
        pPrxSlow = HelloServer::getCommunicator()->stringToProxy<HelloPrx>("Test.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 44444");
        pPrxFast = HelloServer::getCommunicator()->stringToProxy<HelloPrx>("Test.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 34343");
	}
    /**
     * �˳���Hello�����⺯���������˳�ʱ����
     */
	virtual void destroy(){};

    /**
     * testHello��ʵ��jce�ļ��еĽӿ�
     *
     * @return int
     */
	int testHello(const string &s, string &r, JceCurrentPtr current)
	{
//        cout << "testHello:" << current->getRequestId() << ":" << s << endl;
		//���ò�Ҫ���ظ��ͻ��ˣ�
//		current->setResponse(false);

		//��������һ��������첽����
		LOG->debug() << s << ":"<< r << endl;
//		try
//		{
	//		pPrx1->testHello(s, r);
//		}
//		catch(exception &ex)
//		{
//				LOG->debug() << ex.what() << endl;
//		}

//		HelloPrxCallbackPtr cb = new HelloCallback(current);
 //   	pPtr->async_testHello(cb, s);
 //
 //   	

		return 0;
	}

	int testHelloSlow(const string &s, string &r, JceCurrentPtr current)
	{
        LOG->debug() << "testHelloSlow::" << s << endl;

        pPrxSlow->testHelloSlow(s, r);

	    return 0;
	}

    int testHelloFast(const string &s, string &r, JceCurrentPtr current)
    {
        LOG->debug() << "testHelloFast::" << s << endl;

        try
        {
            pPrxFast->testHelloFast(s, r);
        }
        catch(exception &ex)
        {
            LOG->error() << "testHelloFast:" << ex.what() << endl;
        }

        r = s;
        
        return 0;
    }

protected:
	HelloPrx pPrx1;
    HelloPrx pPrxSlow;
    HelloPrx pPrxFast;
	string _hello;
};


#endif




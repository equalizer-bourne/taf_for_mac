#ifndef _HELLO_IMP_H_
#define _HELLO_IMP_H_

#include "Hello.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

static string _transLocator = 
"Test.HelloServer.HelloObj@tcp -h 172.25.38.67 -p 45460 -g 0:tcp -h 172.25.38.67 -p 45461 -g 0";

class HelloImp : public Hello
{
public:
    /**
     *
     */
    HelloImp(){ _count = 0; };

     /**
     * ��ʼ����Hello�����⺯����HelloImp��ʼ��ʱ����
     *
     * @return int
     */
    virtual void initialize()
	{
		//ע�ᴦ������
		TAF_ADD_ADMIN_CMD_NORMAL("SETHELLO", HelloImp::procHello);

        _pPrx = Application::getCommunicator()->stringToProxy<HelloPrx>(_transLocator);
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
		++_count;

		try
		{
			string str = s;// + "wawa";

			_pPrx->testHello(str, r);

			LOG->debug() << "transfer:(" 
					     << current->getBindAdapter()->getEndpoint().toString() 
					     << ")(" << str << ")" << endl;
		}
		catch (exception& e)
		{
			LOG->error() << "transfer exception:" << e.what() << endl;
		}
		return 0;
	}

protected:
	bool procHello(const string& command, const string& params, string& result)
	{
		_hello = params;
		result = "succ!";
		return false;
	}

protected:
    HelloPrx _pPrx;
	string _hello;
	timeval _tv;
	uint32_t _count;
};

#endif




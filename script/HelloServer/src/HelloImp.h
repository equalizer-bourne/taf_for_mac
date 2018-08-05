#ifndef _HELLO_IMP_H_
#define _HELLO_IMP_H_

#include "Hello.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

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
		//ע�ᴦ������
		TAF_ADD_ADMIN_CMD_NORMAL("SETHELLO", HelloImp::procHello);

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
        DLOG << s << endl;
		r = s;
		sleep(10);
	    return 0;
	}

	int testHelloSlow(const string &s, string &r, JceCurrentPtr current)
	{
        LOG->debug() << "testHelloSlow::" << s << endl;
		r = s;
		sleep(10);
	    return 0;
	}

    int testHelloFast(const string &s, string &r, JceCurrentPtr current)
    {
        LOG->debug() << "testHelloFast::" << s << endl;

        r = s;
        
        return 0;
    }

protected:
	bool procHello(const string& command, const string& params, string& result)
	{
		_hello = params;
		return false;
	}

protected:
	string _hello;
};


#endif




#ifndef _HELLO_IMP_H_
#define _HELLO_IMP_H_

#include "Hello.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace taf;
using namespace Test;

/////////////////////////////////////////////////////////////////////////
class HelloImp : public Servant
{

public:
    /**
     * ���캯��
     */
    HelloImp();

public:
    /**
     * �����ʼ��
     */
    virtual void initialize();

    /**
     * ����ͻ��˵���������
     * @param current 
     * @param response 
     * @return int 
     */
    virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);

    /**
     * @param resp 
     * @return int 
     */
    virtual int doResponse(ReqMessagePtr resp);

    /**
     * ��������
     */
    virtual void destroy();

public:
    /**
     * @param s 
     * @param r 
     * @param current 
     * @return int 
     */
    int testHello(const string &s, string &r, JceCurrentPtr current);

    /**
     * @param command 
     * @param params 
     * @param result 
     * @return bool 
     */
    bool procHello(const string& command, const string& params, string& result);
};
///////////////////////////////////////////////////////////////////////////////
#endif

#ifndef __TC_EX_H
#define __TC_EX_H

#include <stdexcept>
using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �쳣����
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* �쳣��
*/
class TC_Exception : public exception
{
public:
    /**
     * ����
     * @param buffer
     *
     * @return explicit
     */
	explicit TC_Exception(const string &buffer);

    /**
     * ����
     * @param buffer
     * @param err, ������, ����strerror��ȡ������Ϣ
     */
	TC_Exception(const string &buffer, int err);

    /**
     * ����
     */
    virtual ~TC_Exception() throw();

    /**
     * ������Ϣ
     *
     * @return const char*
     */
    virtual const char* what() const throw();

    /**
     * 
     * 
     * @return int
     */
    int getErrCode() { return _code; }

private:
    void getBacktrace();

private:
    string  _buffer;
    int     _code;

};

}
#endif


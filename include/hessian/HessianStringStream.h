#ifndef _STRING_INPUT_STREAM_H
#define _STRING_INPUT_STREAM_H

#include <iostream>
#include <string>
#include <stdexcept>
#include "util/tc_ex.h"
#include "util/tc_autoptr.h"
#include "util/tc_clientsocket.h"

using namespace std;

namespace taf
{

/**
 * Hessian�쳣
 */
struct HessianException : public taf::TC_Exception
{
    HessianException(const string &buffer) : taf::TC_Exception(buffer){};
    ~HessianException() throw(){};
};

/**
 * �����쳣
 */
struct HessianProtocolException : public HessianException
{
    HessianProtocolException(const string &buffer) : HessianException(buffer){};
    ~HessianProtocolException() throw(){};
};

/**
 * ����Чservice����
 */
struct HessianNoActiveServiceException : public HessianException
{
    HessianNoActiveServiceException(const string &buffer) : HessianException(buffer){};
    ~HessianNoActiveServiceException() throw(){};
};

/**
 * δ֪����
 */
struct HessianUnknownException : public HessianException
{
    HessianUnknownException(const string &buffer) : HessianException(buffer){};
    ~HessianUnknownException() throw(){};
};

/**
 * ���ʹ���
 */
struct HessianSendException : public HessianException
{
    HessianSendException(const string &buffer) : HessianException(buffer){};
    ~HessianSendException() throw(){};
};

/**
 * Select�쳣
 */
struct HessianSelectException : public HessianException
{
    HessianSelectException(const string &buffer) : HessianException(buffer){};
    ~HessianSelectException() throw(){};
};

/**
 * ��ʱ�쳣
 */
struct HessianTimeOutException : public HessianException
{
    HessianTimeOutException(const string &buffer) : HessianException(buffer){};
    ~HessianTimeOutException() throw(){};
};

/**
 * ���������쳣
 */
struct HessianRecvException : public HessianException
{
    HessianRecvException(const string &buffer) : HessianException(buffer){};
    ~HessianRecvException() throw(){};
};

/**
 * �������رտͻ��˾���쳣
 */
struct HessianLostCloseException : public HessianException
{
    HessianLostCloseException(const string &buffer) : HessianException(buffer){};
    ~HessianLostCloseException() throw(){};
};

/**
 * �����쳣
 */
struct HessianConnectException : public HessianException
{
    HessianConnectException(const string &buffer) : HessianException(buffer){};
    ~HessianConnectException() throw(){};
};

/**
 * Socket�쳣
 */
struct HessianSocketException : public HessianException
{
    HessianSocketException(const string &buffer) : HessianException(buffer){};
    ~HessianSocketException() throw(){};
};

/**
 * �׳������쳣
 * @param ret
 */
inline void throwHessianException(int ret)
{
    switch(ret)
    {
    case taf::TC_ClientSocket::EM_SUCCESS:
        return;
    case taf::TC_ClientSocket::EM_SEND:
        throw HessianSendException("HessianSendException");
        break;
    case taf::TC_ClientSocket::EM_SELECT:
        throw HessianSelectException("HessianSelectException");
        break;
    case taf::TC_ClientSocket::EM_TIMEOUT:
        throw HessianTimeOutException("HessianTimeOutException");
        break;
    case taf::TC_ClientSocket::EM_RECV:
        throw HessianRecvException("HessianRecvException");
        break;
    case taf::TC_ClientSocket::EM_CLOSE:
        throw HessianLostCloseException("HessianLostCloseException");
        break;
    case taf::TC_ClientSocket::EM_CONNECT:
        throw HessianConnectException("HessianConnectException");
        break;
    case taf::TC_ClientSocket::EM_SOCKET:
        throw HessianSocketException("HessianSocketException");
        break;
    default:
        throw HessianUnknownException("HessianUnknownException");
        break;
    }
}

/**
 * û��service�쳣
 */
struct HessianNoSuchHObjectException : public HessianException
{
    HessianNoSuchHObjectException(const string &buffer) : HessianException(buffer){};
    ~HessianNoSuchHObjectException() throw(){};
};

/**
 * û�з����쳣
 */
struct HessianNoSuchMethodException : public HessianException
{
    HessianNoSuchMethodException(const string &buffer) : HessianException(buffer){};
    ~HessianNoSuchMethodException() throw(){};
};

/**
 * ����ͷ�쳣
 */
struct HessianRequireHeaderException : public HessianException
{
    HessianRequireHeaderException(const string &buffer) : HessianException(buffer){};
    ~HessianRequireHeaderException() throw(){};
};

/**
 * Service�׳��쳣
 */
struct HessianServiceException : public HessianException
{
    HessianServiceException(const string &buffer) : HessianException(buffer){};
    ~HessianServiceException() throw(){};
};

/**
 * ��ȡstring��,���ڽ���hessianЭ��
 */
class HessianStringStream : public taf::TC_HandleBase
{
public:
    /**
     * ���캯��
     * @param str
     */
	HessianStringStream(const string& str): _str(str), _pointer(0)
    {
    }

    /**
     * �Ƿ���β��
     *
     * @return bool
     */
	bool eof()
    {
        return _pointer >= _str.length();
    }

    /**
     * ÿ�ζ�ȡһ���ֽ�
     *
     * @return int
     */
	int read()
    {
    	if (eof())
    	{
    		throw HessianProtocolException("HessianProtocolException::read past end of stream");
    	}
    	char c;
    	c = _str[_pointer];
    	_pointer++;
    	return (int)c;
    }

    /**
     * ��������
     * 
     * @return string
     */
    string buffer() const { return _str; }

private:

    /**
     * �ַ�����
     */
	string              _str;

    /**
     * ��ǰָ��
     */
	string::size_type   _pointer;

};

typedef taf::TC_AutoPtr<HessianStringStream> HessianStringStreamPtr;

}

#endif


#ifndef __TC_ENCODER_H_
#define __TC_ENCODER_H_

#include <vector>

#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: ת����
// Author : j@syswin.com              
// Modify: j@syswin.com
/////////////////////////////////////////////////
/**
* ת���쳣��
*/
struct TC_Encoder_Exception : public TC_Exception
{
	TC_Encoder_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Encoder_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TC_Encoder_Exception() throw(){};
};

/**
* �����ṩ�������ñ����ת��
* 1 GBK ==> UTF8
* 2 UTF8 ==> GBK
*/
class TC_Encoder
{
public:
    /**
    * gbk ת���� utf8
    * @param sOut : ���buffer
    * @param iMaxOutLen : ���buffer���ĳ���/sOut�ĳ���
    * @param sIn : ����buffer
    * @param iInLen : ����buffer����
    * @throws TC_Encoder_Exception
    * @return ��
    */
    static void  gbk2utf8(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen);

    /**
    * gbk ת���� utf8
    * @param sOut : ���buffer
    * @param sIn : ����buffer*
    * @throws TC_Encoder_Exception
    * @return string : utf8����
    */
    static string gbk2utf8(const string &sIn);

    /**
    * gbk ת���� utf8
    * @param sIn : ����buffer
    * @param vtStr : ���gbk��vector
    * @throws TC_Encoder_Exception
    * @return ��
    */
    static void gbk2utf8(const string &sIn, vector<string> &vtStr);

    /**
    * utf8 ת���� gbk
    * @param sOut : ���buffer
    * @param iMaxOutLen : ���buffer���ĳ���/sOut�ĳ���
    * @param sIn : ����buffer
    * @param iInLen : ����buffer����
    * @throws TC_Encoder_Exception
    * @return ��
    */
    static void utf82gbk(char *sOut, int &iMaxOutLen, const char *sIn, int iInLen);

    /**
    * utf8 ת���� gbk
    * @param sIn : ����buffer
    * @throws TC_Encoder_Exception
    * @return string :
    */
    static string utf82gbk(const string &sIn);

	/**
	* ת���ַ����е�ĳ���ַ�
	* ȱʡ:\n ת��Ϊ \r\0; \r ת��Ϊ \r\r;
	* @param str: ��ת���ַ���
	* @param f: ��Ҫת����ַ�
	* @param t: ת�����ַ�
	* @param u: ���õ�ת���
	* @return str: ת������ַ���
	*/
	static string transTo(const string& str, char f = '\n', char t = '\r', char u = '\0');

	/**
	* �� transTo ���ַ�����ԭ
	* ȱʡ:\r\0 ��ԭΪ\n; \r\r ��ԭΪ \r;
	* @param str: ����ԭ���ַ���(������transTo��õ����ַ���)
	* @param f: ��ת����ַ�
	* @param t: ת�����ַ�
	* @param u: ���õ�ת���
	* @return str: ��ԭ����ַ���
	*/
	static string transFrom(const string& str, char f = '\n', char t = '\r', char u = '\0');
};

}


#endif



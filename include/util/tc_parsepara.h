#ifndef __TC_PARSEPARA_H
#define __TC_PARSEPARA_H

#include <map>
#include <string>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: name=value��ʽ������(�Ǳ�׼��httpЭ��)
// Author : j@syswin.com              
/////////////////////////////////////////////////
class TC_Parsepara
{
public:

	TC_Parsepara(){};

    /**
    * contructor
    * @param : name=value&name=value�ַ���
    */
	TC_Parsepara(const string &sParam);

    /**
    * copy contructor
    * @param : name=value&name=value�ַ���
    */
	TC_Parsepara(const map<string, string> &mpParam);

    /**
    * copy contructor
    */
	TC_Parsepara(const TC_Parsepara &para);

    /**
    * =
    */
	TC_Parsepara &operator=(const TC_Parsepara &para);

    /**
    * ==
    */
	bool operator==(const TC_Parsepara &para);

    /**
    *+
    */
	const TC_Parsepara operator+(const TC_Parsepara &para);

    /**
    * +=
    */
	TC_Parsepara& operator+=(const TC_Parsepara &para);

    /**
    * decontructor
    */
	~TC_Parsepara();

    /**
    * ��������
    * @param ��������
    * @param ����ֵ
    */
	typedef int (*TC_ParseparaTraverseFunc)(string, string, void *);

    /**
    *����[], ��ȡ����ֵ
    *@return string &����ֵ
    */
	string &operator[](const string &sName);

    /**
    * �����ַ���,������
    * @param sParam:�ַ�������
    * @return ��
    */
	void load(const string &sParam);

    /**
    * ����map,������
    * @param mpParam:�ַ�������
    * @return void
    */
	void load(const map<string, string> &mpParam);

    /**
    * ת���ַ���
    * @return string
    */
	string tostr() const;

    /**
    * ��ȡ����ֵ
    * @param sName ��������
    * @return string
    */
	string getValue(const string &sName) const;

    /**
    * ���ò���ֵ
    * @param sName ��������
    * @param sValue ����ֵ
    * @return void
    */
	void setValue(const string &sName, const string &sValue);

    /**
    * �����ǰ����ֵ��
    * return void
    */
	void clear();

    /**
    * ���÷�ʽ��ȡ����map
    * @return map<string,string>& ���ز���map
    */
    map<string,string> &toMap();

    /**
    * ���÷�ʽ��ȡ����map
    * @return map<string,string>& ���ز���map
    */
    const map<string,string> &toMap() const;

    /**
    * ����ÿ������ֵ��
    * @param func: ����
    * @param pParam: ����,����func��
    * @return void
    */
	void traverse(TC_ParseparaTraverseFunc func, void *pParam);

    /**
    * ���ַ�������,%XXת���ַ�,������httpЭ��ı���
    * @param sParam ����
    * @return string,�������ַ���
    */
	static string decodestr(const string &sParam);

    /**
    * ���ַ�������,�����ַ�ת��%XX,������httpЭ��ı���(���˶Կո�=>+�����⴦��)
    * @param sParam ����
    * @return string, �������ַ���
    */
	static string encodestr(const string &sParam);

protected:

    /**
    * �ַ���ת����map
    * @param sParam ����
    * @param mpParam map
    * @return void
    */
	void decodeMap(const string &sParam, map<string, string> &mpParam) const;

    /**
    * mapת�����ַ���
    * @param mpParam map
    * @return string, ת������ַ���
    */
	string encodeMap(const map<string, string> &mpParam) const;

    /**
    * "%xx" ת��Ϊ�ַ�
    * @param sWhat: %xx�ַ�������������ַ�
    * @return char �����ַ�
    */
	static char x2c(const string &sWhat);

protected:

	map<string, string> _param;
};

}
#endif /*_TC_PARSEPARA_H*/

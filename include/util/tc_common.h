#ifndef __TC_COMMON_H
#define __TC_COMMON_H

#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif

#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <stack>
#include <vector>
#include "util/tc_loki.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: ���ú�����
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* ������,���Ǿ�̬����,�ṩһЩ���õĺ���
*/
class TC_Common
{
public:

    /**
    * ȥ��ͷ���Լ�β�����ַ����ַ���
    * @param sStr: �����ַ���
    * @param s: ��Ҫȥ�����ַ�
    * @param bChar: ���Ϊtrue, ��ȥ��s��ÿ���ַ�; ���Ϊfalse, ��ȥ��s�ַ���
    * @return string ����ȥ������ַ���
    */
    static string trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * ȥ����ߵ��ַ����ַ���
    * @param sStr: �����ַ���
    * @param s: ��Ҫȥ�����ַ�
    * @param bChar: ���Ϊtrue, ��ȥ��s��ÿ���ַ�; ���Ϊfalse, ��ȥ��s�ַ���
    * @return string ����ȥ������ַ���
    */
    static string trimleft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * ȥ���ұߵ��ַ����ַ���
    * @param sStr: �����ַ���
    * @param s: ��Ҫȥ�����ַ�
    * @param bChar: ���Ϊtrue, ��ȥ��s��ÿ���ַ�; ���Ϊfalse, ��ȥ��s�ַ���
    * @return string ����ȥ������ַ���
    */
    static string trimright(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
    * �ַ���ת����Сд
    * @param sString: �ַ���
    * @return string
    */
    static string lower(const string &sString);

    /**
    * �ַ���ת���ɴ�д
    * @param sString: �ַ���
    * @return string: ��д���ַ���
    */
    static string upper(const string &sString);

    /**
    * �ַ����Ƿ������ֵ�
    * @param sString: �ַ���
    * @return bool: �Ƿ�������
    */
    static bool isdigit(const string &sInput);

    /**
    * �ַ���ת����ʱ��ṹ
    * @param sString: �ַ���ʱ���ʽ
    * @param sFormat: ��ʽ
    * @param stTm: ʱ��ṹ
    * @return 0: �ɹ�, -1:ʧ��
    */
    static int str2tm(const string &sString, const string &sFormat, struct tm &stTm);

    /**
     * GMT��ʽ��ʱ��ת��Ϊʱ��ṹ(Sat, 06 Feb 2010 09:29:29 GMT, %a, %d %b %Y %H:%M:%S GMT)
     * ������mktime����time_t, ����ע��ʱ��
     * ������mktime(&stTm) - timezone���ɱ��ص���(time(NULL)ֵ��ͬ)
     * timezone��ϵͳ��, ��Ҫextern long timezone;
     * @param sString
     * @param stTm
     * 
     * @return int, 0: �ɹ�, -1:ʧ��
     */
    static int strgmt2tm(const string &sString, struct tm &stTm);

    /**
    * ʱ��ת�����ַ���
    * @param stTm: ʱ��ṹ
    * @param sFormat: ��ʽ
    * @return string ʱ���ַ���
    */
    static string tm2str(const struct tm &stTm, const string &sFormat = "%Y%m%d%H%M%S");

    /**
    * ʱ��ת�����ַ���
    * @param t: ʱ��ṹ
    * @param sFormat: ��ʽ
    * @return string ʱ���ַ���
    */
    static string tm2str(const time_t &t, const string &sFormat = "%Y%m%d%H%M%S");

    /**
    * ��ǰʱ��ת�����ַ���,���ո�ʽ(%Y%m%d%H%M%S)
    * @param sFormat: ��ʽ
    * @return string ʱ���ַ���
    */
    static string now2str(const string &sFormat = "%Y%m%d%H%M%S");

    /**
    * ʱ��ת����GMT�ַ���,GMT��ʽ:Fri, 12 Jan 2001 18:18:18 GMT
    * @param stTm: ʱ��ṹ
    * @return string ʱ���ַ���
    */
    static string tm2GMTstr(const struct tm &stTm);

    /**
    * ʱ��ת����GMT�ַ���,GMT��ʽ:Fri, 12 Jan 2001 18:18:18 GMT
    * @param stTm: ʱ��ṹ
    * @return string ʱ���ַ���
    */
    static string tm2GMTstr(const time_t &t);

    /**
    * ��ǰʱ��ת����GMT�ַ���,GMT��ʽ:Fri, 12 Jan 2001 18:18:18 GMT
    * @return string ʱ���ַ���
    */
    static string now2GMTstr();

    /**
    * ��ǰ������(������)ת�����ַ���(%Y%m%d)
    * @return string ʱ���ַ���
    */
    static string nowdate2str();

    /**
    * ��ǰ��ʱ��(ʱ����)ת�����ַ���(%H%M%S)
    * @return string ʱ���ַ���
    */
    static string nowtime2str();

    /**
     * ��ǰ�ĺ���
     *
     * @return int64_t
     */
    static int64_t now2ms();

    /**
     * ȡ����ǰ΢��
     *
     * @return int64_t
     */
    static int64_t now2us();

    /**
    * �ַ���ת����T��(���T����ֵ����, ���strΪ��,��TΪ0)
    * @param sStr: �ַ���
    * @return T , T��
    */
    template<typename T>
    static T strto(const string &sStr);

    /**
    * �ַ���ת����T��
    * @param sStr: �ַ���
    * @param sDefault, ȱʡֵ
    * @return T , T��
    */
    template<typename T>
    static T strto(const string &sStr, const string &sDefault);

    /**
    * �����ַ���,�÷ָ����ŷָ�,������vector��
    * ����: |a|b||c|
    *
    * ���withEmpty=trueʱ, ����|�ָ�Ϊ:"","a", "b", "", "c", ""
    * ���withEmpty=falseʱ, ����|�ָ�Ϊ:"a", "b", "c"
    * ���T����Ϊint����ֵ����, ��ָ����ַ���Ϊ"", ��ǿ��ת��Ϊ0
    * 
    * @param sStr: �����ַ���
    * @param sSep: �ָ��ַ���(ÿ���ַ�����Ϊ�ָ���)
    * @param withEmpty: true: �յ�Ҳ��һ��Ԫ��, false:�յĹ���
    * @return vector<string>
    */
    template<typename T>
    static vector<T> sepstr(const string &sStr, const string &sSep, bool withEmpty = false);

    /**
    * T��ת�����ַ���,ֻҪT�ܹ�ʹ��ostream������<<����,�����Ա��ú���֧��
    * @param t:
    * @return string �ַ���
    */
    template<typename T>
    static string tostr(const T &t);

    /**
     * vectorת����string
     * @param T
     * @param sSep, ��ӵ��ַ���
     *
     * @return string
     */
    template<typename T>
    static string tostr(const vector<T> &t);

    /**
     * map���
     * @param K
     * @param V
     * @param t
     *
     * @return string
     */
    template<typename K, typename V, typename D, typename A>
    static string tostr(const map<K, V, D, A> &t);

    /**
     * map���
     * @param K
     * @param V
     * @param t
     *
     * @return string
     */
    template<typename K, typename V, typename D, typename A>
    static string tostr(const multimap<K, V, D, A> &t);

    /**
    * pair ת��Ϊ�ַ���, ��֤map�ȹ�ϵ��������ֱ����tostr�����
    * @param pair<F, S>: pair����
    * @return string �ַ���
    */
    template<typename F, typename S>
    static string tostr(const pair<F, S> &itPair);

    /**
    * container ת�����ַ���
    * @param iFirst: ������
    * @param iLast: ������
    * @param sSep: ����Ԫ��֮��ķָ���
    * @return string �ַ���
    */
    template <typename InputIter>
    static string tostr(InputIter iFirst, InputIter iLast, const string &sSep = "|");

    /**
    * ����������ת�����ַ���
    * @param buf: ������buffer
    * @param len: buffer����
    * @param sSep: �ָ���
    * @param lines: ���ٸ��ֽڻ�һ��, Ĭ��0��ʾ������
    * @return string �ַ���
    */
    static string bin2str(const void *buf, size_t len, const string &sSep = "", size_t lines = 0);

    /**
    * ����������ת�����ַ���
    * @param sBinData: ����������
    * @param sSep: �ָ���
    * @param lines: ���ٸ��ֽڻ�һ��, Ĭ��0��ʾ������
    * @return string �ַ���
    */
    static string bin2str(const string &sBinData, const string &sSep = "", size_t lines = 0);

    /**
    * �ַ���ת���ɶ�����
    * @param psAsciiData: �ַ���
    * @param sBinData: ����������
    * @param iBinSize: ��Ҫת�����ַ�������
    * @return int ת����ĳ���
    */
    static int str2bin(const char *psAsciiData, unsigned char *sBinData, int iBinSize);

    /**
     * �ַ���ת���ɶ�����
     * @param sBinData
     * @param sSep
     * @param lines
     *
     * @return string
     */
    static string str2bin(const string &sBinData, const string &sSep = "", size_t lines = 0);

    /**
    * �滻�ַ���
    * @param sString: �����ַ���
    * @param sSrc: ԭ�ַ���
    * @param sDest: Ŀ���ַ���
    * @return string: �滻����ַ���
    */
    static string replace(const string &sString, const string &sSrc, const string &sDest);

    /**
     * ƥ����.�ָ����ַ���, pat��*�����ͨ���:����ǿյ��κ��ַ���
     * sΪ��, ����false
     * patΪ��, ����true
     * sΪ��ͨ�ַ���
     * patΪ��*��ƥ����ַ���
     * ����ƥ��ip��ַ
     * @param s
     * @param pat
     *
     * @return bool
     */
    static bool matchPeriod(const string& s, const string& pat);
    
     /**
     * @param s
     * @param pat
     *
     * @return bool
     */
    static bool matchPeriod(const string& s, const vector<string>& pat);

    /**
     * �ж�һ�����Ƿ�������
     * @param n
     *
     * @return bool
     */
    static bool isPrimeNumber(size_t n);

    /**
     * daemon
     */
    static void daemon();

    /**
     * ���Թܵ��쳣
     */
    static void ignorePipe();

    /**
     * %xxת��һ���ֽ�
     * @param sWhat
     *
     * @return char
     */
    static char x2c(const string &sWhat);

    /**
     * ��С�ַ��������ֽ���, ֧��K, M, G����
     * ����: 1K, 3M, 4G, 4.5M, 2.3G
     * @param s
     * @param iDefaultSize, ȱʡ�Ĵ�С, ��ʽ����ʱ, ȱʡ�Ĵ�С
     * @return size_t
     */
    static size_t toSize(const string &s, size_t iDefaultSize);
};

namespace p
{
    template<typename D>
    struct strto1
    {
        D operator()(const string &sStr)
        {
            string s = "0";

            if(!sStr.empty())
            {
                s = sStr;
            }

            istringstream sBuffer(s);

            D t;
            sBuffer >> t;

            return t;
        }
    };

	template<>
    struct strto1<char>
    {
        char operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return sStr[0];
			}
			return 0;
        }
    };

    template<typename D>
    struct strto2
    {
        D operator()(const string &sStr)
        {
            istringstream sBuffer(sStr);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto2<string>
    {
        string operator()(const string &sStr)
        {
            return sStr;
        }
    };
	
}

template<typename T>
T TC_Common::strto(const string &sStr)
{
    typedef typename TL::TypeSelect<TL::TypeTraits<T>::isStdArith, p::strto1<T>, p::strto2<T> >::Result strto_type;

    return strto_type()(sStr);
}

template<typename T>
T TC_Common::strto(const string &sStr, const string &sDefault)
{
    string s;

    if(!sStr.empty())
    {
        s = sStr;
    }
    else
    {
        s = sDefault;
    }

    istringstream sBuffer(s);

    T t;
    sBuffer >> t;

    return t;
}


template<typename T>
vector<T> TC_Common::sepstr(const string &sStr, const string &sSep, bool withEmpty)
{
    vector<T> vt;

    string::size_type pos = 0;
    string::size_type pos1 = 0;

    while(true)
    {
        string s;
        pos1 = sStr.find_first_of(sSep, pos);
        if(pos1 == string::npos)
        {
            if(pos + 1 <= sStr.length())
            {
                s = sStr.substr(pos);
            }
        }
        else if(pos1 == pos)
        {
            s = "";
        }
        else
        {
            s = sStr.substr(pos, pos1 - pos);
            pos = pos1;
        }

        if(withEmpty)
        {
            vt.push_back(strto<T>(s));
        }
        else
        {
            if(!s.empty())
            {
                T tmp = strto<T>(s);
                vt.push_back(tmp);
            }
        }

        if(pos1 == string::npos)
        {
            break;
        }

        pos++;
    }

    return vt;
}

template<typename T>
string TC_Common::tostr(const T &t)
{
    ostringstream sBuffer;
    sBuffer << t;
    return sBuffer.str();
}

template<typename T>
string TC_Common::tostr(const vector<T> &t)
{
    ostringstream sBuffer;
    for(size_t i = 0; i < t.size(); i++)
    {
        sBuffer << tostr(t[i]) << " ";
    }
    return sBuffer.str();
}

template<typename K, typename V, typename D, typename A>
string TC_Common::tostr(const map<K, V, D, A> &t)
{
    ostringstream sBuffer;
    typename map<K, V, D, A>::const_iterator it = t.begin();
    while(it != t.end())
    {
        sBuffer << " [" << tostr(it->first) << "]=[" << tostr(it->second) << "] ";
        ++it;
    }
    return sBuffer.str();
}

template<typename K, typename V, typename D, typename A>
string TC_Common::tostr(const multimap<K, V, D, A> &t)
{
    ostringstream sBuffer;
    typename multimap<K, V, D, A>::const_iterator it = t.begin();
    while(it != t.end())
    {
        sBuffer << " [" << tostr(it->first) << "]=[" << tostr(it->second) << "] ";
        ++it;
    }
    return sBuffer.str();
}

template<typename F, typename S>
string TC_Common::tostr(const pair<F, S> &itPair)
{
    ostringstream sBuffer;
    sBuffer << "[" << (itPair.first) << "]=[" << (itPair.second) << "]";
    return sBuffer.str();
}

template <typename InputIter>
string TC_Common::tostr(InputIter iFirst, InputIter iLast, const string &sSep)
{
    string sBuffer;
    InputIter it = iFirst;

    while(it != iLast)
    {
        sBuffer += tostr(*it);
        ++it;

        if(it != iLast)
        {
            sBuffer += sSep;
        }
        else
        {
            break;
        }
    }

    return sBuffer;
}

}
#endif


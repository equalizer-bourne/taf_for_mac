#ifndef HESSIAN_PROXY_H
#define HESSIAN_PROXY_H

#include <string>
#include <vector>
#include <map>

#include "hessian/HessianWrappers.h"

namespace taf
{

//////////////////////////////////////////////////////////

/**
 * ��װbinary������
 */
class BinaryWrapper
{
public:
    BinaryWrapper(){};
    BinaryWrapper(const string &s) : _binary(s)
    {
    }

    string &value() { return _binary; }
    const string &value() const { return _binary; }
    bool operator==(const BinaryWrapper &wrapper) const
    {
        return _binary == wrapper._binary;
    }
protected:
    string _binary;
};

/**
 * bool����
 * @param i
 * 
 * @return HBooleanPtr
 */
inline HBooleanPtr encode(bool i)
{
    return new HBoolean(i);
}

/**
 * int����
 * @param i
 * 
 * @return HIntegerPtr
 */
inline HIntegerPtr encode(int i)
{
    return new HInteger(i);
}

/**
 * byte����
 * @param i
 * 
 * @return IntegerPtr
 */
inline HIntegerPtr encode(char i)
{
    return encode((int)i);
}

/**
 * short����
 * @param i
 * 
 * @return HIntegerPtr
 */
inline HIntegerPtr encode(short i)
{
    return encode((int)i);
}

/**
 * string����
 * @param s
 * 
 * @return HStringPtr
 */
inline HStringPtr encode(const string &s)
{
    return new HString(s);
}

/**
 * binary����
 * @param bw
 * 
 * @return BinaryPtr
 */
inline HBinaryPtr encode(const BinaryWrapper &bw)
{
    return new HBinary(bw.value());
}

/**
 * float����
 * @param f
 * 
 * @return DoublePtr
 */
inline HDoublePtr encode(float f)
{
    return new HDouble(f);
}

/**
 * double����
 * @param f
 * 
 * @return DoublePtr
 */
inline HDoublePtr encode(double f)
{
    return new HDouble(f);
}

/**
 * �����α���
 * @param f
 * 
 * @return LongPtr
 */
inline HLongPtr encode(long long f)
{
    return new HLong(f);
}

/**
 * �ṹ����
 * @param T
 * @param t
 * 
 * @return HObjectPtr
 */
template<typename T>
inline HObjectPtr encode(const T &t)
{
    return t.encode();
}

/**
 * vector����
 * @param T
 * @param t
 * 
 * @return HListPtr
 */
template<typename T>
inline HListPtr encode(const vector<T> &t)
{
    HListPtr lsPtr = new HList();
    for(size_t i = 0; i < t.size(); i++)
    {
        lsPtr->value().push_back(encode(t[i]));
    }
    return lsPtr;
}

/**
 * Map����
 * @param K
 * @param V
 * @param t
 * 
 * @return MapPtr
 */
template<typename K, typename V>
inline HMapPtr encode(const map<K, V> &t)
{
    HMapPtr msPtr = new HMap();
    typename map<K, V>::const_iterator it = t.begin();
    while(it != t.end())
    {
        msPtr->value()[encode(it->first)] = encode(it->second);
        ++it;
    }

    return msPtr;
}

//////////////////////////////////////////////////////////

/**
 * bool����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, bool &t)
{
    taf::HBooleanPtr sPtr = taf::HBooleanPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: bool type not exists");
    }
    t = sPtr->value();
}

/**
 * short����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, short &t)
{
    taf::HIntegerPtr sPtr = taf::HIntegerPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: short type not exists");
    }
    t = sPtr->value();
}

/**
 * int����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, int &t)
{
    taf::HIntegerPtr sPtr = taf::HIntegerPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: int type not exists");
    }
    t = sPtr->value();
}

/**
 * byte����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, char &t)
{
    taf::HIntegerPtr sPtr = taf::HIntegerPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: byte type not exists");
    }
    t = sPtr->value();
}

/**
 * string����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, string &t)
{
    taf::HStringPtr sPtr = taf::HStringPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        t = "";
//        throw taf::HessianProtocolException("HessianProtocolException:: string type not exists");
    }
    else
    {
        t = sPtr->value();
    }
}

/**
 * binary����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, BinaryWrapper &t)
{
    taf::HBinaryPtr sPtr = taf::HBinaryPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        t.value() = "";
//        throw taf::HessianProtocolException("HessianProtocolException:: binary type not exists");
    }
    else
    {
        t.value() = sPtr->value();
    }
}

/**
 * float����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, float &t)
{
    taf::HDoublePtr sPtr = taf::HDoublePtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: float type not exists");
    }
    t = sPtr->value();
}

/**
 * double����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, double &t)
{
    taf::HDoublePtr sPtr = taf::HDoublePtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: double type not exists");
    }
    t = sPtr->value();
}

/**
 * long long����
 * @param oPtr
 * @param t
 */
inline void decode(const HObjectPtr &oPtr, long long &t)
{
    taf::HLongPtr sPtr = taf::HLongPtr::dynamicCast(oPtr);
    if(!sPtr) 
    {
        throw taf::HessianProtocolException("HessianProtocolException:: long long type not exists");
    }
    t = sPtr->value();
}

/**
 * �ṹ����
 * @param T
 * @param oPtr
 * @param t
 */
template<typename T>
inline void decode(const HObjectPtr &oPtr, T &t)
{
    t.decode(oPtr);
}

/**
 * vector����
 * @param T
 * @param oPtr
 * @param t
 */
template<typename T>
inline void decode(const HObjectPtr &oPtr, vector<T> &t)
{
    taf::HListPtr lsPtr = taf::HListPtr::dynamicCast(oPtr);
    if(!lsPtr) 
    {
        t.clear();
//        throw taf::HessianProtocolException("HessianProtocolException:: vector type not exists");
    }
    else
    {
        std::list<taf::HObjectPtr>::iterator it = lsPtr->value().begin();
        while(it != lsPtr->value().end())
        {
            T tt;
            decode(*it, tt);

            t.push_back(tt);
            ++it;
        }
    }
}

/**
 * map����
 * @param K
 * @param V
 * @param oPtr
 * @param t
 */
template<typename K, typename V>
inline void decode(const HObjectPtr &oPtr, map<K, V> &t)
{
    taf::HMapPtr msPtr = taf::HMapPtr::dynamicCast(oPtr);
    if(!msPtr) 
    {
        t.clear();
//        throw taf::HessianProtocolException("HessianProtocolException:: vector type not exists");
    }
    else
    {
        std::map<taf::HObjectPtr, taf::HObjectPtr>::iterator it = msPtr->value().begin();
        while(it != msPtr->value().end())
        {
            K k;
            V v;
            decode(it->first, k);
            decode(it->second, v);
            t[k] = v;
            ++it;
        }
    }
}

//////////////////////////////////////////////////////////
/**
 * URL����
 */
class UrlManage
{
public:
    /**
     * 
     */
    UrlManage();

    /**
     * ע�����URL
     * @param url
     */
	void registryUrl(const string &url);

    /**
     * �����ȡһ��URL
     * @param stUrl
     * 
     * @return bool
     */
    bool getRandomUrl(pair<vector<string>::iterator, string> &stUrl);

    /**
     * ��URL����Ϊ��Ч
     * @param it
     */
    void setInvalid(vector<string>::iterator it);

    /**
     * ������Ч��ʱ��, ����
     * @param seconds
     */
    void setInvalidSeconds(time_t seconds);

protected:
    /**
     * ʧЧ������
     */
    time_t          _iDefaultSeconds;

    /**
     * ʧЧ������
     */
    time_t          _iSeconds;

    /**
     * ����URL
     */
	vector<string>  _vUrl;

    /**
     * ��Ч��URL
     */
    vector<pair<time_t, string> > _vInvalidUrl;
};

//////////////////////////////////////////////////////////

/**
 * hessian���õĻ���
 * ����hessian�ĵ����඼�Ӹ���̳�
 */
class HessianProxy
{
public:
    /**
     * ע�����URL
     * @param url
     */
	void registryUrl(const string &url);

    /**
     * ����URLʧЧ�����ε�ʱ��
     * @param iSeconds
     */
    void setInvalidSeconds(time_t iSeconds);

    /**
     * ����
     * @param method
     * @param vParam
     *
     * @return HObjectPtr
     */
	HObjectPtr call(const string& method, vector<HObjectPtr> &vParam);

    /**
     * �ϴε��õ�buffer
     *
     * @return string
     */
    string lastBuffer() { return _buffer; }

    /**
     * �ͻ��˱������ó�UTF8
     */
    static void setUtf8();

    /**
     * �ͻ��˱������ó�GBK
     */
    static void setGbk();

    /**
     * �Ƿ���utf8����
     * 
     * @return bool
     */
    static bool isUtf8();

    /**
     * �Ƿ���gbk����
     * 
     * @return bool
     */
    static bool isGbk();

protected:
    /**
     * �ͻ��˵��ַ���
     */
    static string CLIENT_CHARSET;

private:
    /**
     * buffer
     */
    string          _buffer;

    /**
     * URL�ܾ���
     */
    UrlManage       _urlManage;
};

}
#endif


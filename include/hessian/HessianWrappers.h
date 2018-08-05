#ifndef _HESSIAN_WRAPPERS_H
#define _HESSIAN_WRAPPERS_H

#include <iostream>
#include <string>
#include <list>
#include <map>
#include "util/tc_autoptr.h"
#include "hessian/HessianStringStream.h"

/**
*֧������:
*
*binary						A binary value
*boolean					The byte 'F' represents false and the byte 'T' represents true
*date						HDate represented by a 64-bits long of milliseconds since the epoch
*double						A 64-bit IEEE floating pointer number
*int						A 32-bit signed integer
*list						An ordered list, like an array
*long						A 64-bit signed integer
*map						Represents serialized objects and HMaps
*null						HNull represents a null pointer
*ref						An integer referring to a previous list or map instance
*remote						A reference to a remote object
*string						A 16-bit unicode character string encoded in UTF-8
*xml						An XML document encoded as a 16-bit unicode character string encoded in UTF-8
*/

namespace taf
{

class HObject;
typedef taf::TC_AutoPtr<HObject> HObjectPtr;
class HNull;
typedef taf::TC_AutoPtr<HNull> HNullPtr;
class HBinary;
typedef taf::TC_AutoPtr<HBinary> HBinaryPtr;
class HBoolean;
typedef taf::TC_AutoPtr<HBoolean> HBooleanPtr;
class HDate;
typedef taf::TC_AutoPtr<HDate> HDatePtr;
class HDouble;
typedef taf::TC_AutoPtr<HDouble> HDoublePtr;
class HInteger;
typedef taf::TC_AutoPtr<HInteger> HIntegerPtr;
class HList;
typedef taf::TC_AutoPtr<HList> HListPtr;
class HLong;
typedef taf::TC_AutoPtr<HLong> HLongPtr;
class HMap;
typedef taf::TC_AutoPtr<HMap> HMapPtr;
class HRef;
typedef taf::TC_AutoPtr<HRef> HRefPtr;
class HRemote;
typedef taf::TC_AutoPtr<HRemote> HRemotePtr;
class HXml;
typedef taf::TC_AutoPtr<HXml> HXmlPtr;
class HString;
typedef taf::TC_AutoPtr<HString> HStringPtr;
class HFault;
typedef taf::TC_AutoPtr<HFault> HFaultPtr;

/**
 * �������
 */
class HObject : public taf::TC_HandleBase
{
public:
    /**
     * ���캯��
     */
	HObject() {}

    /**
     * ����һ������
     * @param tag
     *
     * @return HObjectPtr
     */
    static HObjectPtr buildHObject(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ��ȡ����
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag){};

    /**
     * �������
     * @param call
     *
     * @return string
     */
    virtual string& write(string &call) { return call; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject&) const;

protected:
	HObject(const HObject& other);
	HObject operator=(const HObject& right);

    /**
     * ��ȡһ���ַ���
     * @param hssPtr
     * @param tag
     * 
     * @return string
     */
    string readString(HessianStringStreamPtr &hssPtr, int tag);
};

bool operator!=(const HObject&, const HObject&);
bool operator<(const HObject&, const HObject&);
bool operator<=(const HObject&, const HObject&);
bool operator>(const HObject&, const HObject&);
bool operator>=(const HObject&, const HObject&);

//////////////////////////////////////////////////////
/**
 * HNull����
 */
class HNull : public HObject
{
public:
	HNull() { }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag){}

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);
};

//////////////////////////////////////////////////////

/**
 * HRef����
 */
class HRef : public HNull
{
public:

    /**
     * ����
     */
    HRef()
    {
    }

    /**
     * ����
     * @param value
     */
	HRef(int value) : _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     * @param tag
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return int&
     */
    int &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HRef *s = dynamic_cast<const HRef*>(&l);
        return s && _value == s->_value;
    }
protected:
    int _value;
};
//////////////////////////////////////////////////////

class HRemote : public HNull
{
public:
    /**
     * ����
     */
	HRemote() { }

    /**
     * ��ȡ
     * @param hssPtr
     * @param tag
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag) { }

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);
};

//////////////////////////////////////////////////////
class HLong : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HLong(long long value = 0, char c = 'L'): _value(value), _c(c)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ����ֵ
     *
     * @return long long
     */
    long long& value() { return _value; }

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HLong *s = dynamic_cast<const HLong*>(&l);
        return s && _value == s->_value;
    }

protected:
    long long   _value;
    char        _c;
};

///////////////////////////////////////////
class HDate : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HDate(long long value = 0): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return int&
     */
    long long &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HDate *s = dynamic_cast<const HDate*>(&l);
        return s && _value == s->_value;
    }

protected:
    long long _value;
};

////////////////////////////////////////
class HInteger : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HInteger(int value = 0): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return int&
     */
    int &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        cout << _value << endl;
        const HInteger *s = dynamic_cast<const HInteger*>(&l);
        cout << _value << ":" << (_value == s->_value) << endl;
        return s && _value == s->_value;
    }
protected:
    int _value;
};

////////////////////////////////////////
class HBoolean : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HBoolean(bool value = false): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag) { }

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return bool&
     */
    bool &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HBoolean *s = dynamic_cast<const HBoolean*>(&l);
        return s && _value == s->_value;
    }

protected:
    bool _value;
};

////////////////////////////////////////
class HBinary : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HBinary(const string &value = ""): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return string&
     */
    string &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HBinary *s = dynamic_cast<const HBinary*>(&l);
        return s && _value == s->_value;
    }
protected:

    /**
     * ��ȡ�����ֽ�
     * @param hssPtr
     * @param bytes
     */
    void readByteChunk(HessianStringStreamPtr &hssPtr, string& bytes);

    /**
     * ���
     * @param call
     * @param value
     * @param tag
     * 
     * @return string&
     */
    string& writeByte(string &call, const string &value, int tag);
protected:
    string _value;
};

//////////////////////////////////////
class HString : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HString(const string &value = "", char c = 'S'): _value(value), _c(c)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ����ֵ
     *
     * @return string&
     */
    string &value() { return _value; }

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HString *s = dynamic_cast<const HString*>(&l);
        return s && (_value == s->_value);
    }

protected:
    /**
     * ��������ַ���
     * @param hssPtr
     * @param length
     * 
     * @return string
     */
    string readStringImpl(HessianStringStreamPtr &hssPtr, int length);

    /**
     * gbk��utf8
     * @param sIn
     * 
     * @return string
     */
    string toUtf8(const string &sIn);

    /**
     * utf8��gbk
     * @param sIn
     * 
     * @return string
     */
    string utf8To(const string &sIn);

protected:
    string _value;
    char   _c;
};

//////////////////////////////////////
class HXml : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HXml(const string &value = ""): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return string&
     */
    string &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HXml *s = dynamic_cast<const HXml*>(&l);
        return s && _value == s->_value;
    }
protected:
    string _value;
};

/////////////////////////////////////////
class HDouble : public HObject
{
public:
    /**
     * ���캯��
     * @param value
     */
	HDouble(double value = 0): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return string&
     */
    double &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HDouble *s = dynamic_cast<const HDouble*>(&l);
        return s && _value == s->_value;
    }
protected:
    double _value;
};

/////////////////////////////////////////
class HList : public HObject
{
public:

    /**
     * ���캯��
     * @param value
     */
	HList(const std::list<HObjectPtr>& value = std::list<HObjectPtr>()): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return string&
     */
    std::list<HObjectPtr> &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HList *s = dynamic_cast<const HList*>(&l);
        return s && _value == s->_value;
    }
protected:
    std::list<HObjectPtr> _value;
};

/////////////////////////////////////////
class HMap : public HObject
{
public:
    /**
     * ���캯��
     */
    HMap(){ }

    /**
     * ���캯��
     * @param value
     */
	HMap(const std::map<HObjectPtr, HObjectPtr>& value): _value(value)
    {
    }

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ���
     * @param call
     *
     * @return string&
     */
    virtual string& write(string &call);

    /**
     * ����ֵ
     * 
     * @return string&
     */
    std::map<HObjectPtr, HObjectPtr> &value() { return _value; }

    /**
     * ==
     * 
     * @return bool
     */
    virtual bool operator==(const HObject& l) const
    {
        const HMap *s = dynamic_cast<const HMap*>(&l);
        return s && _value == s->_value;
    }

    /**
     * ����Ԫ��
     * @param o
     * 
     * @return HObjectPtr
     */
    HObjectPtr& find(const HObjectPtr &o)
    {
        static HObjectPtr to;
        std::map<HObjectPtr, HObjectPtr>::iterator it = _value.begin();
        while(it != _value.end())
        {
            if(it->first == o)
            {
                return it->second;
            }
            ++it;
        }
        return to;
    }

protected:
    std::map<HObjectPtr, HObjectPtr> _value;
};

//////////////////////////////////////////////////////
/**
 * hessian������
 */
class HFault : public HObject
{
public:
	typedef std::string basic_type;

    /**
     * ���캯��
     */
    HFault(){};

    /**
     * ���캯��
     * @param code
     * @param message
     */
	HFault(const string& code, const string& message):	_code(code), _message(message)
	{
	}

    /**
     * ��������
     * @param other
     */
	HFault(const HFault& other) :	_code(other._code), _message(other._message)
	{
	}

    /**
     * ��ȡ
     * @param hssPtr
     */
    virtual void read(HessianStringStreamPtr &hssPtr, int tag);

    /**
     * ������ʾ
     * 
     * @return string
     */
	string value() { return string("HessianHFault: {code=").append(_code).append(", message=").append(_message).append("}"); }

    /**
     * ����
     * 
     * @return string
     */
	string code() const { return _code; }

    /**
     * ������ʾ
     * 
     * @return string
     */
	string message() const { return _message; }

    /**
     * �׳��쳣
     */
    void throwException();
protected:
	string _code;
	string _message;
};

}

#endif

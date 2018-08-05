#ifndef __TC_AUTOPTR_H
#define __TC_AUTOPTR_H

#include "util/tc_atomic.h"
#include "util/tc_ex.h"

namespace taf
{
///////////////////////////////////////////////////////
// ˵��: ����ָ����(�޸���ICEԴ��, ����ָ�벻���໥����, �����ڴ�й©)
// Author : j@syswin.com              
//////////////////////////////////////////////////////
/**
* ��ָ���쳣
*/
struct TC_AutoPtrNull_Exception : public TC_Exception
{
    TC_AutoPtrNull_Exception(const string &buffer) : TC_Exception(buffer){};
    ~TC_AutoPtrNull_Exception() throw(){};
};

/**
 * ����ָ�����
 * ������Ҫ����ָ��֧�ֵ��඼��Ҫ�Ӹö���̳�
 */
template<class  T>
class TC_HandleBaseT
{
public:

     /**
     * ԭ�Ӽ�������
     */
    typedef T atomic_type;

    /**
     * ����
     *
     * @return TC_HandleBase&
     */
    TC_HandleBaseT& operator=(const TC_HandleBaseT&)
    {
        return *this;
    }

    /**
     * ���Ӽ���
     */
    void incRef() { _atomic.inc_fast(); }

    /**
     * ���ټ���, ������==0ʱ, ����Ҫɾ������ʱ, �ͷŶ���
     */
    void decRef()
    {
        if(_atomic.dec_and_test() && !_bNoDelete)
        {
            _bNoDelete = true;
            delete this;
        }
    }

    /**
     * ��ȡ����
     *
     * @return int
     */
    int getRef() const        { return _atomic.get(); }

    /**
     * ���ò��Զ��ͷ�
     * @param b
     */
    void setNoDelete(bool b)  { _bNoDelete = b; }

protected:

    /**
     * ���캯��
     */
    TC_HandleBaseT() : _atomic(0), _bNoDelete(false)
    {
    }

    /**
     * ��������
     */
    TC_HandleBaseT(const TC_HandleBaseT&) : _atomic(0), _bNoDelete(false)
    {
    }

    /**
     * ����
     */
    virtual ~TC_HandleBaseT()
    {
    }

protected:

    /**
     * ����
     */
    atomic_type   _atomic;

    /**
     * �Ƿ��Զ�ɾ��
     */
    bool        _bNoDelete;
};

template<>
inline void TC_HandleBaseT<int>::incRef() 
{ 
    //__sync_fetch_and_add(&_atomic,1);
    ++_atomic; 
}

template<> 
inline void TC_HandleBaseT<int>::decRef()
{
    //int c = __sync_fetch_and_sub(&_atomic, 1);
    //if(c == 1 && !_bNoDelete)
    if(--_atomic == 0 && !_bNoDelete)
    {
        _bNoDelete = true;
        delete this;
    }
}

template<> 
inline int TC_HandleBaseT<int>::getRef() const        
{ 
    //return __sync_fetch_and_sub(const_cast<volatile int*>(&_atomic), 0);
    return _atomic; 
} 

typedef TC_HandleBaseT<TC_Atomic> TC_HandleBase;

/**
 * ���Է���������,���̰߳�ȫ������ָ��
 * T����̳���TC_HandleBase
 */
template<typename T>
class TC_AutoPtr
{
public:

    /**
     * Ԫ������
     */
    typedef T element_type;

    /**
     * ��ԭ��ָ���ʼ��, ����+1
     * @param p
     */
    TC_AutoPtr(T* p = 0)
    {
        _ptr = p;

        if(_ptr)
        {
            _ptr->incRef();
        }
    }

    /**
     * ����������ָ��r��ԭ��ָ���ʼ��, ����+1
     * @param Y
     * @param r
     */
    template<typename Y>
    TC_AutoPtr(const TC_AutoPtr<Y>& r)
    {
        _ptr = r._ptr;

        if(_ptr)
        {
            _ptr->incRef();
        }
    }

    /**
     * ��������, ����+1
     * @param r
     */
    TC_AutoPtr(const TC_AutoPtr& r)
    {
        _ptr = r._ptr;

        if(_ptr)
        {
            _ptr->incRef();
        }
    }

    /**
     * ����
     */
    ~TC_AutoPtr()
    {
        if(_ptr)
        {
            _ptr->decRef();
        }
    }

    /**
     * ��ֵ, ��ָͨ��
     * @param p
     *
     * @return TC_AutoPtr&
     */
    TC_AutoPtr& operator=(T* p)
    {
        if(_ptr != p)
        {
            if(p)
            {
                p->incRef();
            }

            T* ptr = _ptr;
            _ptr = p;

            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    /**
     * ��ֵ, ������������ָ��
     * @param Y
     * @param r
     *
     * @return TC_AutoPtr&
     */
    template<typename Y>
    TC_AutoPtr& operator=(const TC_AutoPtr<Y>& r)
    {
        if(_ptr != r._ptr)
        {
            if(r._ptr)
            {
                r._ptr->incRef();
            }

            T* ptr = _ptr;
            _ptr = r._ptr;

            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    /**
     * ��ֵ, ����������ִ��ָ��
     * @param r
     *
     * @return TC_AutoPtr&
     */
    TC_AutoPtr& operator=(const TC_AutoPtr& r)
    {
        if(_ptr != r._ptr)
        {
            if(r._ptr)
            {
                r._ptr->incRef();
            }

            T* ptr = _ptr;
            _ptr = r._ptr;

            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    /**
     * ���������͵�����ָ�뻻�ɵ�ǰ���͵�����ָ��
     * @param Y
     * @param r
     *
     * @return TC_AutoPtr
     */
    template<class Y>
    static TC_AutoPtr dynamicCast(const TC_AutoPtr<Y>& r)
    {
        return TC_AutoPtr(dynamic_cast<T*>(r._ptr));
    }

    /**
     * ������ԭ�����͵�ָ��ת���ɵ�ǰ���͵�����ָ��
     * @param Y
     * @param p
     *
     * @return TC_AutoPtr
     */
    template<class Y>
    static TC_AutoPtr dynamicCast(Y* p)
    {
        return TC_AutoPtr(dynamic_cast<T*>(p));
    }

    /**
     * ��ȡԭ��ָ��
     *
     * @return T*
     */
    T* get() const
    {
        return _ptr;
    }

    /**
     * ����
     *
     * @return T*
     */
    T* operator->() const
    {
        if(!_ptr)
        {
            throwNullHandleException();
        }

        return _ptr;
    }

    /**
     * ����
     *
     * @return T&
     */
    T& operator*() const
    {
        if(!_ptr)
        {
            throwNullHandleException();
        }

        return *_ptr;
    }

    /**
     * �Ƿ���Ч
     *
     * @return bool
     */
    operator bool() const
    {
        return _ptr ? true : false;
    }

    /**
     * ����ָ��
     * @param other
     */
    void swap(TC_AutoPtr& other)
    {
        std::swap(_ptr, other._ptr);
    }

protected:

    /**
     * �׳��쳣
     */
    void throwNullHandleException() const;

public:
    T*          _ptr;

};

/**
 * �׳��쳣
 * @param T
 * @param file
 * @param line
 */
template<typename T> inline void
TC_AutoPtr<T>::throwNullHandleException() const
{
    throw TC_AutoPtrNull_Exception("autoptr null handle error");
}

/**
 * ==�ж�
 * @param T
 * @param U
 * @param lhs
 * @param rhs
 *
 * @return bool
 */
template<typename T, typename U>
inline bool operator==(const TC_AutoPtr<T>& lhs, const TC_AutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
    if(l && r)
    {
        return *l == *r;
    }
    else
    {
        return !l && !r;
    }
}

/**
 * �������ж�
 * @param T
 * @param U
 * @param lhs
 * @param rhs
 *
 * @return bool
 */
template<typename T, typename U>
inline bool operator!=(const TC_AutoPtr<T>& lhs, const TC_AutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
    if(l && r)
    {
        return *l != *r;
    }
    else
    {
        return l || r;
    }
}

/**
 * С���ж�, ���ڷ���map��������
 * @param T
 * @param U
 * @param lhs
 * @param rhs
 *
 * @return bool
 */
template<typename T, typename U>
inline bool operator<(const TC_AutoPtr<T>& lhs, const TC_AutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
    if(l && r)
    {
        return *l < *r;
    }
    else
    {
        return !l && r;
    }
}

}

#endif

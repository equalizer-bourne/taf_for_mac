#ifndef _HESSIAN_OUTPUT_H
#define _HESSIAN_OUTPUT_H

#include "hessian/HessianWrappers.h"

#include <string>

namespace taf
{

/**
 * ���뷢�͵�����
 */
class HessianOutput
{
public:
    /**
     * ��ʼ����
     * @param method_name
     *
     * @return string
     */
	string start(const string& method_name);

    /**
     * ��ɵ���
     * @param call
     *
     * @return string&
     */
	string& complete(string& call);

    /**
     * ���ò���
     * @param call
     * @param object
     *
     * @return string&
     */
	string& setParameter(string& call, const HObjectPtr &object);

protected:

    /**
     * дobject����
     * @param call
     * @param object
     *
     * @return string&
     */
	string& writeObject(string& call, const HObjectPtr &object);
};

}

#endif


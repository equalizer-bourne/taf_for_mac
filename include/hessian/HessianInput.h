#ifndef _HESSIAN_INPUT_H
#define _HESSIAN_INPUT_H

#include "hessian/HessianWrappers.h"
#include "hessian/HessianStringStream.h"
#include <string>
#include <list>
#include <vector>
#include <memory>

using namespace std;

namespace taf
{

/**
 * ������Ӧ��
 */
class HessianInput
{
public:
    /**
     * ���캯��
     * @param buffer
     */
	HessianInput(const string &buffer)
    {
		_hssPtr = new HessianStringStream(buffer);
	}

    /**
     * ��ȡ���
     *
     * @return HObjectPtr
     */
	HObjectPtr result();

protected:

    /**
     * �����쳣
     * @param expect
     * @param ch
     *
     * @return HessianException
     */
	HessianException expect(const string& expect, int ch);

protected:
    /**
     * ������
     */
	HessianStringStreamPtr  _hssPtr;
};

}

#endif


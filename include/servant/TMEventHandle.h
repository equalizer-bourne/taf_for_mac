#ifndef __TAF_TM_EVENT_HANDLE_H_
#define __TAF_TM_EVENT_HANDLE_H_

#include "util/tc_autoptr.h"
#include <set>

namespace taf
{
/**
 * ��ʱ�¼��������Ļ���
 */
class TMEventHandle : virtual public TC_HandleBase
{
public:
    /**
     * ����������
     */
	virtual ~TMEventHandle() {}

    /**
     * ��ʱ��������ȱʡ������
     * @return int
     */
	virtual int handleTimeout() { return 0; }
};
///////////////////////////////////////////////////////
}
#endif

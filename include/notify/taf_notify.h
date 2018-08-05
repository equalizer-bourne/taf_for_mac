#ifndef __TAF_NOTIFY_H_
#define __TAF_NOTIFY_H_

#include "notify/Notify.h"
#include "servant/Global.h"
#include "util/tc_singleton.h"

using namespace std;

namespace taf
{

/**
 * �ϱ���Ϣ��Notify����
 * �첽�ϱ���notify����
 */
class TafRemoteNotify : public TC_Singleton<TafRemoteNotify>
{
public:
    /**
     * ��ʼ��
     * @param comm, ͨ����
     * @param obj, ��������
     * @param notifyPrx
     * @param app
     * @param serverName
     *
     * @return int
     */
	int setNotifyInfo(const CommunicatorPtr &comm, const string &obj, const string & app, const string &serverName);

    /**
     * ֪ͨ, һ�����첽�ϱ���
     * @param message
     */
    void notify(NOTIFYLEVEL level, const string &sMesage);

    /**
     * �ϱ�
     * @param sResult
     * @param bSync
     */
    void report(const string &sResult, bool bSync = false);

protected:
    /**
     * ͨ����
     */
    CommunicatorPtr _comm;

    /**
     * ֪ͨ����
     */
	NotifyPrx       _notifyPrx;

    /**
     * Ӧ��
     */
    string          _sApp;

    /**
     * ��������
     */
	string          _sServerName;
};

}

#endif

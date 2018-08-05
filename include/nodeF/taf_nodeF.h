#ifndef __TAF_NODEF_H__
#define __TAF_NODEF_H__

#include "nodeF/NodeF.h"
#include "servant/Global.h"
#include "util/tc_singleton.h"

namespace taf
{

/**
 * ��node��������
 * ����keepAlive�첽����������node
 */
class TafNodeFHelper : public TC_Singleton<TafNodeFHelper>,public TC_ThreadMutex 
{
public:

    /**
     * ����node��Ϣ
     * @param comm, ͨ����
     * @param obj,
     * @param app
     * @param server
     */
    void setNodeInfo(const CommunicatorPtr &comm, const string &obj, const string &app, const string &server);

    /**
     * keepAlive
     */
    void keepAlive(const string &adapter = "");

    /**
     * �ϱ�TAF�ı���汾
     * @param version
     */
    void reportVersion(const string &version);

protected:
    /**
     * ͨ����
     */
    CommunicatorPtr _comm;

    /**
     * Node
     */
    ServerFPrx      _nodePrx;

    /**
     * ��Ϣ
     */
    ServerInfo      _si;

    set<string>     _adapterSet;

};

}

#endif


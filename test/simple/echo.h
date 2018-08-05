#include "util/tc_common.h"
#include "util/tc_epoll_server.h"

#include <sys/un.h>
#include <iostream>

using namespace std;
using namespace taf;

extern TC_RollLogger g_logger;

/**
 * ������, ÿ�������߳�һ������
 */
class EchoHandle : public TC_EpollServer::Handle
{
public:
    /**
     * Э�����
     * @param in
     * @param out
     * 
     * @return int
     */
    static int parse(string &in, string &out)
    {
        if(in.at(in.length() - 1) == '\n')
        {
            out = in;
            in  = "";
            return TC_EpollServer::PACKET_FULL;
        }
        
        return TC_EpollServer::PACKET_LESS;
    }

    /**
     * ��ʼ��
     */
    virtual void initialize()
    {
        cout << "initialize:" << pthread_self() << endl;
    }

    /**
     * ���崦���߼�
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
     //   cout << stRecvData.buffer.length() << endl;
//     assert(stRecvData.buffer.length()==7193);
//        cout << "handle:" <<  stRecvData.buffer << endl;

        sendResponse(stRecvData.uid, stRecvData.buffer, stRecvData.ip, stRecvData.port);
    }

    /**
     * ���ر���
     * @param stRecvData
     */
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        cout << "EchoHandle::handleTimeout:" << pthread_self() << endl;
        close(stRecvData.uid);
    }
};


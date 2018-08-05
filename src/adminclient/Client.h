#ifndef __ONE_ADMIN_CLIENT_H__
#define __ONE_ADMIN_CLIENT_H__

#include "util/tc_common.h"
#include "util/tc_option.h"
#include "util/tc_config.h"

#include "servant/Communicator.h"
#include "AdminReg.h"
#include "servant/QueryF.h"
#include "Node.h"
#include "Parser.h"

#include <iostream>

using namespace taf;

class Client
{
    public:
        /**
         * ��ʼ������˿�
         * @param conf �����ļ�
         * @return
         */
        int init(TC_Config & conf);

        /**
         * notify����admin����
         * @param ipport 
         * @param command ����
         * @param result ���
         * @return
         */
        int doNotify(const string & ipport, const string & command, string & result);

        /**
         * ʹ�÷���
         * @param ��������������
         */
        void usage(int argc, char *argv[]);

        /**
         * �������
         */
        int run();

    protected:
        taf::CommunicatorPtr _comm;

        AdminRegPrx _adminPtr;
        NodePrx _nodePtr;

};



#endif

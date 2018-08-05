#include "log/taf_logger.h"
#include <iostream>

using namespace taf;


/**
 * LOG�Ŀ�˵��:
 * 1 ѭ����־����LOG->error(), LOG->debug()
 * 2 ѭ����־���ϴ���������
 * 3 ������־����DLOG, FDLOG����¼, FDLOG����ָ����־��׺��
 * 4 ������־Ҳ���Բ��ϴ���Զ�̷�����
 */

#define DT \
        n1 = TC_Common::now2ms();   \
        t = n1 - n;                 \
        n = n1;

#define OT if(t > 10) cout << i << ":" << t << endl;
void logger()
{
    int64_t n = TC_Common::now2ms();
    int64_t n1 = TC_Common::now2ms();
    int64_t t = TC_Common::now2ms();

	int i = 200000;
	while(i)
	{
//        if(i % 10000 == 0)
        {
            usleep(10000);
        }

        LOG->info() << i << ":" << t << endl;
		LOG->debug() << i << ":" << t << endl;
		LOG->error() << i << ":" << t << endl;

  		FDLOG("abc1") << i << ":" << t << endl;
  		FDLOG("abc2") << i << ":" << t << endl;
  		FDLOG("abc3") << i << ":" << t << endl;

		
        i--;
	}
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 2)
        {
            cout << "Usage:" << argv[0] << " <config file>" << endl;
            return 0;
        }

        CommunicatorPtr c = new Communicator();

        string logObj = "taf.taflog.LogObj@tcp -h 172.25.38.67 -p 17654";
        //��ʼ������ѭ����־
		TafRollLogger::getInstance()->setLogInfo("Test", "TestServer", "./");
        //��ʼ��ʱ����־
		TafTimeLogger::getInstance()->setLogInfo(c, logObj , "Test", "TestServer", "./");

//        TafTimeLogger::getInstance()->enableLocal("abc2", false);
/*
        TafRollLogger::getInstance()->sync(true);

        //ȱʡ�İ�����־�����ϴ���������
        TafTimeLogger::getInstance()->enableRemote("", false);
        //ȱʡ�İ�����־�����ӹ���
        TafTimeLogger::getInstance()->initFormat("", "%Y%m%d%H%M");

        //abc2�ļ������ϴ���������
        TafTimeLogger::getInstance()->enableRemote("abc2", false);
        //abc2�ļ���Сʱ����
        TafTimeLogger::getInstance()->initFormat("abc2", "%Y%m%d%H");

        //abc3�ļ����Ǳ���
        TafTimeLogger::getInstance()->enableLocal("abc3", false);

       TafRollLogger::getInstance()->logger()->setLogLevel(TC_RollLogger::INFO_LOG);
*/
		TC_ThreadPool tpoolA;
		tpoolA.init(10);
		tpoolA.start();

		cout << TC_Common::now2str() << endl;
		{
			TC_Functor<void> functor(logger);
			TC_Functor<void>::wrapper_type wtA(functor);

			for(size_t i = 0; i < tpoolA.getThreadNum(); i++)
			{
				tpoolA.exec(wtA);
			}
		}

		tpoolA.waitForAllDone();

		cout << TC_Common::now2str() << endl;
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}



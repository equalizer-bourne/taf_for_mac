#include "HelloServer.h"
#include "HelloImp.h"

using namespace std;

HelloServer g_app;

/////////////////////////////////////////////////////////////////
void
HelloServer::initialize()
{
	//initialize application here:
	//...

	addServant<HelloImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".HelloObj");

	//���������Ļ�ȡ�����ļ�
//	addConfig("HelloServer.conf");

	//ע�ᴦ������
    TAF_ADD_ADMIN_CMD_NORMAL("DISABLEDLOG", HelloServer::procDLOG);

	//����http����
//	bindHttp("tcp -h 172.25.38.67 -p 8000 -t 60000");

    //�����ַ�������
//	bindString("tcp -h 127.0.0.1 -p 8001 -t 60000");

	
}

/////////////////////////////////////////////////////////////////
void
HelloServer::destroyApp()
{
	//destroy application here:
	//...
	LOG->debug() << "destroyApp" << endl;
}

/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}
	return -1;
}
/////////////////////////////////////////////////////////////////

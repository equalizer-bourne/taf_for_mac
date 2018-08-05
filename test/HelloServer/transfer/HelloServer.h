#ifndef _HelloServer_H_
#define _HelloServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "servant/Application.h"

using namespace taf;

struct HttpProtocol
{
    static int parse(string &in, string &out)
    {
		//TAF��util�����ṩ��httpЭ�������
        taf::TC_HttpRequest request;
        if(request.decode(in))
        {
            out = in;
            in  = "";
            return 1;	//����1��ʾ�յ��İ��Ѿ���ȫ
        }

        return 0;		//����0��ʾ�յ��İ�����ȫ
//		return -1;	//����-1��ʾ�յ���Э����󣬿�ܻ��Զ��رյ�ǰ����
    }
};

/**
 * ������, ÿ�������߳�һ������
 * �̳�TC_EpollServer::Handle��ʵ����Ӧ�ķ���
 */
class HttpHandle : public TC_EpollServer::Handle
{
public:
    /**
     * ���캯��, �߳�����ʱ�ṹ�������
     */
    HttpHandle()
    {}

	//��ʼ������������󣬻���øú���
    virtual void initialize()
    { 
    }

    /**
     * ���崦���߼�
     * @param stRecvData
     */
	/*
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
        try
        {
            TC_HttpRequest request;
            request.decode(stRecvData.buffer);
			
            string sFileName = "/home/jarod/taf/test/util/" + TC_Common::now2str() + ".tmp";

            TC_Cgi cgi;

            //�����ϴ��ļ�·��������ļ�����
            cgi.setUpload(sFileName, 10);
            cgi.parseCgi(request);

            ostringstream os;
            os << TC_Common::tostr(cgi.getUploadFilesMap());

            os << TC_Common::tostr(cgi.getParamMap());
            os << cgi.isOverUploadFiles();

			//��װ��Ӧ��
            TC_HttpResponse response;
            response.setResponse(200, "OK", os.str());
            response.setConnection("close");
            string buffer = response.encode();

            sendRespone(stRecvData.uid, buffer, stRecvData.ip, stRecvData.port);
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            cout << ex.what() << endl;
        }
    }

	//���ر�����ʱ�����øú���
    virtual void handleOverload(const TC_EpollServer::tagRecvData &stRecvData)
    {
        close(stRecvData.uid);
    }
	*/
};

/**
 *
 **/
class HelloServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~HelloServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

protected:

	//����һ��������������ɳ�ʼ�����󶨶˿�֮��
	/*
    void bind(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(getEpollServer().get());

		//����ip,port������str�ĸ�ʽ��:tcp -h 127.0.0.1 -p 80 -t 60000
        lsPtr->setEndpoint(str);
		//���ô����߳���
        lsPtr->setHandleNum(4);
		//����Э�������
        lsPtr->setProtocol(&HttpProtocol::parse);
		//���ô�����
        lsPtr->setHandle<HttpHandle>();

		//��ɰ�
        getEpollServer()->bind(lsPtr);
    }
	*/

	bool procDLOG(const string& command, const string& params, string& result)
	{
		TafTimeLogger::getInstance()->enableLocal(params, false);
		return false;
	}
};

extern HelloServer g_app;

////////////////////////////////////////////
#endif

#ifndef _HelloServer_H_
#define _HelloServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "util/tc_parsepara.h"
#include "servant/Application.h"

using namespace taf;

/////////////////////////////////////////////////////////////////////////////
// ������httpЭ�������
struct HttpProtocol
{
    static int parse(string &in, string &out)
    {
        cout << in;
        try
        {
    		//TAF��util�����ṩ��httpЭ�������
            taf::TC_HttpRequest request;
            if(request.decode(in))
            {
                out = in;
                in  = "";
                return TC_EpollServer::PACKET_FULL;	//��ʾ�յ��İ��Ѿ���ȫ
            }
        }
        catch(exception &ex)
        {
            cout << ex.what() << endl;
            return TC_EpollServer::PACKET_ERR;
        }

        return TC_EpollServer::PACKET_LESS;		//��ʾ�յ��İ�����ȫ
//		return TC_EpollServer::PACKET_ERR;	//��ʾ�յ���Э����󣬿�ܻ��Զ��رյ�ǰ����
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
            os << TC_Common::tostr(cgi.isOverUploadFiles());

			//��װ��Ӧ��
            TC_HttpResponse response;
            response.setResponse(200, "OK", os.str());
            response.setConnection("close");
            string buffer = response.encode();

            cout << buffer << endl;
            sendResponse(stRecvData.uid, buffer, stRecvData.ip, stRecvData.port);
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            cout << ex.what() << endl;
        }
    }

	//���ر�����ʱ�����øú���
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        close(stRecvData.uid);
    }
};

//////////////////////////////////////////////////////////////////////////////////
// �������ַ���Э�������
//////////////////////////////////////////////////////////////////////////////////
// �������ַ���Э�������
struct StringProtocol
{
    static int parse(string &in, string &out)
    {
        cout << in;
        string::size_type pos = in.find("\r\n");

        if(pos != string::npos)
        {
            out = in.substr(0, pos);
            in  = in.substr(pos+2);

            return TC_EpollServer::PACKET_FULL;   //����1��ʾ�յ��İ��Ѿ���ȫ
        }

        return TC_EpollServer::PACKET_LESS;		//����0��ʾ�յ��İ�����ȫ
//		return TC_EpollServer::PACKET_ERR;	    //����-1��ʾ�յ���Э����󣬿�ܻ��Զ��رյ�ǰ����
    }
};


/**
 * ������, ÿ�������߳�һ������
 * �̳�TC_EpollServer::Handle��ʵ����Ӧ�ķ���
 */
class StringHandle : public TC_EpollServer::Handle
{
public:
    /**
     * ���캯��, �߳�����ʱ�ṹ�������
     */
    StringHandle()
    {}

	//��ʼ������������󣬻���øú���
    virtual void initialize()
    { 
    }

    /**
     * ���崦���߼�
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
        try
        {
            //taf�ṩ��name=value�Ľ�����
            taf::TC_Parsepara para(stRecvData.buffer);

            cout << TC_Common::tostr(para.toMap()) << endl;
			
            sendResponse(stRecvData.uid, "ok\r\n", stRecvData.ip, stRecvData.port);
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            cout << ex.what() << endl;
        }
    }

	//���ر�����ʱ�����øú���
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        close(stRecvData.uid);
    }
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
    void bindHttp(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(getEpollServer().get());

        lsPtr->setName("HttpAdapter");
		//����ip,port������str�ĸ�ʽ��:tcp -h 127.0.0.1 -p 80 -t 60000
        lsPtr->setEndpoint(str);
        //�������������
        lsPtr->setMaxConns(10240);
        //���ö��г���(Ĭ����10240��Ԫ��), �������������, ��ֱ�ӹرպ�������������
        lsPtr->setQueueCapacity(1000);
        //���ö������ݵĳ�ʱʱ��, ������ݳ�����ʱ��ʱ, ��handleTimeout������, Ĭ���ǹر����ӵ�, ҵ����Լ̳�handleʱ����
        lsPtr->setQueueTimeout(10000);
		//���ô����߳���
        lsPtr->setHandleNum(2);
		//����Э�������
        lsPtr->setProtocol(&HttpProtocol::parse);
		//���ô�����
        lsPtr->setHandle<HttpHandle>();

		//��ɰ�
        getEpollServer()->bind(lsPtr);

    }

	//����һ��������������ɳ�ʼ�����󶨶˿�֮��
    void bindString(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(getEpollServer().get());

        lsPtr->setName("StringAdapter");

		//����ip,port������str�ĸ�ʽ��:tcp -h 127.0.0.1 -p 80 -t 60000
        lsPtr->setEndpoint(str);
		//���ô����߳���
        lsPtr->setHandleNum(2);
		//����Э�������
        lsPtr->setProtocol(&StringProtocol::parse);
		//���ô�����
        lsPtr->setHandle<StringHandle>();

		//��ɰ�
        getEpollServer()->bind(lsPtr);
    }
protected:
	bool procDLOG(const string& command, const string& params, string& result)
	{
		TafTimeLogger::getInstance()->enableLocal(params, false);
		return false;
	}
};

extern HelloServer g_app;

////////////////////////////////////////////
#endif

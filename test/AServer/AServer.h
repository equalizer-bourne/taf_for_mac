#ifndef _AServer_H_
#define _AServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 * ������, ÿ�������߳�һ������
 * �̳�TC_EpollServer::Handle��ʵ����Ӧ�ķ���
 */
class StringHandle : public TC_EpollServer::Handle, public BaseNotify
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
		//ע�ᴦ������
		TAF_ADD_ADMIN_CMD_NORMAL("D", StringHandle::procDLOG);
    }

    /**
     * ���崦���߼�
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
		TC_LockT<TC_ThreadRecMutex> lock(*this);

        try
        {
            sendResponse(stRecvData.uid, "ok\r\n", stRecvData.ip, stRecvData.port);
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            cout << ex.what() << endl;
        }
    }

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
protected:
	bool procDLOG(const string& command, const string& params, string& result)
	{
		cout << command << endl;
		return true;
	}
};


/**
 *
 **/
class AServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~AServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();


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
        lsPtr->setProtocol(&StringHandle::parse);
		//���ô�����
        lsPtr->setHandle<StringHandle>();

		//��ɰ�
        getEpollServer()->bind(lsPtr);
    }

};

extern AServer g_app;

////////////////////////////////////////////
#endif

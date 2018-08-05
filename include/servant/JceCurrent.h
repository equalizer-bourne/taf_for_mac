#ifndef __TAF_JCE_CURRENT_H_
#define __TAF_JCE_CURRENT_H_

#include "util/tc_epoll_server.h"
#include "jce/RequestF.h"
#include "servant/BaseF.h"

namespace taf
{
    
class ServantHandle;

/**
 * ��ǰ�����������
 */
class JceCurrent : public TC_HandleBase
{
public:
    typedef std::map<string, string>    TAF_STATUS;

    typedef std::vector<char>           TAF_BUFFER;

    /**
     * ���캯��
     * @param pServantHandle
     */
    JceCurrent(ServantHandle *pServantHandle);

    /**
     * ����
     */
    ~JceCurrent();

    /**
     * ��ȡIP
     * @return string
     */
    string getIp() const;

    /**
     * ��ȡ�˿�
     * @return int
     */
    int getPort() const;

    /**
     * �Ƿ�������ʱ������Ӧ�����ͻ���
     * @return bool
     */
    bool isResponse() const;

    /**
     * �����Ƿ��Զ�����Ӧ��
     */
    void setResponse(bool value) { _bResponse = value; }

    /**
     * �رյ�ǰ����
     */
    void close();

    /**
     * ��ȡ������ServantHandle
     */
    ServantHandle* getServantHandle();

    /**
     * ��ȡ��Դ��Adapter
     * @return TC_EpollServer::BindAdapter* 
     */
    TC_EpollServer::BindAdapter* getBindAdapter();

    /**
     * ��ȡ����buffer
     * @return string
     */
    const vector<char> &getRequestBuffer() const;

    /**
     * ��ȡ����Servant����
     * @return string
     */
    string getServantName() const;

    /**
     * �����Э��İ汾��(��TAFЭ����Ч)
     * 
     * @return short
     */
    short getRequestVersion() const;

    /**
     * ��չmap(��TAFЭ����Ч)
     * @return map<string,string>&
     */
    map<string, string>& getContext();

    /**
     * ����״̬��Ϣ������Ҷȡ�Ⱦɫ��(��TAFЭ����Ч)
     * @return map<string,string>&
     */
    const map<string, string>& getRequestStatus() const;

    /**
     * ��������(��TAFЭ����Ч)
     * @return string
     */
    string getFuncName() const;

    /**
     * ����ID(��TAFЭ����Ч)
     * @return int
     */
    uint32_t getRequestId() const;


    /**
     * ��ȡ������(��TAFЭ����Ч)
     * @return char
     */
    char getPacketType() const;

    /**
     * ��ȡ��Ϣ����(��TAFЭ����Ч)
     * @return taf::Int32
     */
    taf::Int32 getMessageType() const;

	/**
	 * ��ȡ���յ������ʱ��
	 */
	struct timeval getRecvTime() const;


    /**
     * tafЭ��ķ�����Ӧ����(��TAFЭ����Ч)
     * @param iRet
     * @param status
     * @param buffer
     */
    void sendResponse(int iRet, const vector<char>& buffer = TAF_BUFFER(), 
                      const map<string, string>& status = TAF_STATUS(),
                      const string & sResultDesc = "");

    /**
     * ��ͨЭ��ķ�����Ӧ����(��TAFЭ����Ч)
     * @param buff 
     * @param len 
     */
    void sendResponse(const char* buff, uint32_t len);

protected:

    friend class ServantHandle;

    friend class Application;

    /**
     * ��ʼ��
     * @param stRecvData
     */
    void initialize(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * ��ʼ��
     * @param sRecvBuffer
     */
    void initialize(const string &sRecvBuffer);

    /**
     * ������ϱ�״̬����Ե�����ü�WUP����(����TAFЭ����Ч)
     */
    void reportToStat(const string & sObj);

    /**
    * ���������Ϣ(����TAFЭ����Ч)
    */
    void saveSampleKey();

protected:
    /**
     * ������ָ��
     */
    ServantHandle* _pServantHandle;

    /**
     * ��Ϣ_pBindAdapter
     */
    TC_EpollServer::BindAdapter* _pBindAdapter;

    /**
     * Ψһ��ʶ
     */
    uint32_t                _uid;

    /**
     * ip��ַ
     */
    string                  _ip;

    /**
     * �˿�
     */
    int                     _port;

    /**
     * �ͻ��������
     */
    RequestPacket           _request;

    /**
     * ��Ӧ
     */
    bool                    _bResponse;

    /**
     * �յ�����ʱ��
     */
    timeval                 _begintime;

    /**
     * �ӿڴ���ķ���ֵ
     */
    int                     _iRet;
};
//////////////////////////////////////////////////////////////
}
#endif

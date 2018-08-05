#ifndef __TAF_TRANSCEIVER_H_
#define __TAF_TRANSCEIVER_H_

#include "servant/EndpointInfo.h"
#include "servant/Message.h"
#include "servant/Global.h"
#include "servant/NetworkUtil.h"
#include <list>

using namespace std;

namespace taf
{
/**
 * ���紫����࣬��Ҫ�ṩsend/recv�ӿ�
 */
class Transceiver : public TC_HandleBase
{
public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    Transceiver(ObjectProxy *op, EndpointInfo ep, int fd);

    /**
     *
     *��������
     */
    virtual ~Transceiver();

    /**
     * ��ȡ�ļ�������
     * @return int
     */
    virtual int fd() const;

    /**
     * �ر�����
     */
    virtual void doClose();

    /**
     * �Ƿ���Ч
     * @return bool
     */
	virtual bool isValid() const;

	/**
	 * ��ȡ���ӵ�����
	 * 
	 * @author kevintian (2010-8-25)
	 * 
	 * @return EndpointInfo::EType 
	 */
	EndpointInfo::EType getEndpointType() { return _ep.type(); }

public:
    /**
     * ���������ж�Send BufferCache�Ƿ��������İ�
     * @return int
     */
	virtual int doRequest();

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done) = 0;

    /**
     * ���͵�server����Ϣ������
     *
     */
    void writeToSendBuffer(const string& msg);

    /**
     * ���һ�����͵�����
     * @param req
     */
    void setLastReq(ReqMessagePtr& req);

    /**
     * �ر�����
     */
    void close();

public:
    /**
     * ���緢�ͽӿ�
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag) = 0;

    /**
     * ������սӿ�
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag) = 0;

protected:
    int _fd;

	EndpointInfo _ep;

    ObjectProxy *_objectProxy;

    string _sendBuffer;

    string _recvBuffer;

    ReqMessagePtr _lastReq;
};
//////////////////////////////////////////////////////////
/**
 * TCP ����ʵ��
 */
class TcpTransceiver : public Transceiver
{
public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    TcpTransceiver(ObjectProxy *op, EndpointInfo ep, int fd);

    /**
     * TCP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * TCP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int, =1,��ʾ�����ݾͰ�
     */
	virtual int doResponse(list<ResponsePacket>& done);
};
//////////////////////////////////////////////////////////
/**
 * UDP ����ʵ��
 */
class UdpTransceiver : public Transceiver
{
public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    UdpTransceiver(ObjectProxy *op, EndpointInfo ep, int fd);

    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done);
};
//////////////////////////////////////////////////////////
}
#endif

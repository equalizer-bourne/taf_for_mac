#ifndef __TAF_PROTOCOL_H_
#define __TAF_PROTOCOL_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "jce/RequestF.h"
#include "jce/wup.h"
#include "servant/BaseF.h"
#include "util/tc_epoll_server.h"

using namespace std;
using namespace wup;

namespace taf
{

template<typename T> 
T net2host(T len)
{
    switch(sizeof(T))
    {
    case sizeof(uint8_t): return len;
    case sizeof(uint16_t): return ntohs(len);
    case sizeof(uint32_t): return ntohl(len);
    }
    assert(true);
    return 0;
}
//////////////////////////////////////////////////////////////////////
// Э�����
class AppProtocol
{
public:
    /**
     * ����Э��
     * @param in, Ŀǰ��buffer
     * @param out, һ�������İ�
     *
     * @return int, 0��ʾû�н�����ȫ, 1��ʾ�յ�һ��������
     */
    static int parse(string &in, string &out);

    /**
     * ����Э��
     * @param in, Ŀǰ��buffer
     * @param out, һ�������İ�
     *
     * @return int, 0��ʾû�н�����ȫ, 1��ʾ�յ�һ��������
     */
    static int parseAdmin(string &in, string &out);

    /**
     * 
     * @param T 
     * @param offset 
     * @param netorder 
     * @param in 
     * @param out 
     * @return int 
     */
    template<size_t offset, typename T, bool netorder>
    static int parseStream(string& in, string& out)
    {
        if(in.length() < offset + sizeof(T))
        {       
            return TC_EpollServer::PACKET_LESS;
        }       

        T iHeaderLen = 0;

        ::memcpy(&iHeaderLen, in.c_str() + offset, sizeof(T));

        if (netorder)
        {
            iHeaderLen = net2host<T>(iHeaderLen);
        }

        if (iHeaderLen < (T)(offset + sizeof(T)) || (uint32_t)iHeaderLen > 100000000)
        {       
            return TC_EpollServer::PACKET_ERR;
        }       

        if (in.length() < (uint32_t)iHeaderLen)
        {       
            return TC_EpollServer::PACKET_LESS;
        }       

        out = in.substr(0, iHeaderLen);

        in  = in.substr(iHeaderLen);

        return TC_EpollServer::PACKET_FULL;
    }
};
//////////////////////////////////////////////////////////////////////
typedef TC_Functor<void, TL::TLMaker<const RequestPacket&, string&>::Result> request_protocol;
/**
 * ����Э�鴦��, ����ֵ��ʾ�����˶����ֽ�
 * ��ܲ���Զ��Դ����˰�������
 */
typedef TC_Functor<size_t, TL::TLMaker<const char *, size_t, list<ResponsePacket>&>::Result> response_protocol;

/**
 * �ͻ����Զ���Э������
 */
class ProxyProtocol
{
public:
    /**
     * ����
     */
    ProxyProtocol() : requestFunc(streamRequest) {}

    /**
     * ��ͨ�����������
     * @param request
     * @param buff
     */
    static void streamRequest(const RequestPacket& request, string& buff)
    {
        buff.assign((const char*)(&request.sBuffer[0]), request.sBuffer.size());
    }

    /**
     * ��ͨ����ֹ����taf�����
     * @param request
     * @param buff
     */
    template
    <
        size_t offset, 
        typename T, 
        bool netorder, 
        size_t idOffset, 
        typename K, 
        bool idNetorder
    >
    static size_t streamResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done)
    {
        size_t pos = 0;

        while (pos < length)
        {
            uint32_t len = length - pos;

            if (len < offset + sizeof(T))
            {
                break;
            }

            T iHeaderLen = 0;

            ::memcpy(&iHeaderLen, recvBuffer + pos + offset, sizeof(T));

            if (netorder)
            {
                iHeaderLen = net2host<T>(iHeaderLen);
            }

            //��һ�±���,���ȴ���10M
            if ((uint32_t)iHeaderLen > 10000000 || iHeaderLen == 0)
            {
                throw JceDecodeException("packet length too long or zero,len:" + TC_Common::tostr(iHeaderLen));
            }

            //��û�н���ȫ
            if (len < iHeaderLen)
            {
                break;
            }

            ResponsePacket rsp;

            rsp.sBuffer.resize(iHeaderLen);

            ::memcpy(&rsp.sBuffer[0], recvBuffer + pos, iHeaderLen);

            K requestId;

            ::memcpy(&requestId, recvBuffer + pos + idOffset, sizeof(K));

            if (idNetorder)
            {
                requestId = net2host<K>(requestId);
            }

            rsp.iRequestId = static_cast<uint32_t>(requestId);

            done.push_back(rsp);

            pos += iHeaderLen;
        }

        return pos;
    }

    /**
     * wup��Ӧ��(wup����Ӧ�����ResponsePacket��buffer��)
     * @param request
     * @param buff
     */
    static size_t wupResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done);

public:
    /**
     * taf�����
     * @param request
     * @param buff
     */
    static void tafRequest(const RequestPacket& request, string& buff);

    /**
     * taf��Ӧ������
     * @param recvBuffer
     * @param done
     */
    static size_t tafResponse(const char* recvBuffer, size_t length, list<ResponsePacket>& done);

public:
    request_protocol requestFunc;

    response_protocol responseFunc;
};
//////////////////////////////////////////////////////////////////////
}
#endif

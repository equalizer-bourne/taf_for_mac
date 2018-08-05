#ifndef __TAF_ENDPOINT_INFO_H_
#define __TAF_ENDPOINT_INFO_H_

#include "servant/Global.h"
#include "servant/NetworkUtil.h"

using namespace std;

namespace taf
{
/**
 * ��ַ��ϢIP:Port
 */
class EndpointInfo
{
public:
	enum EType { TCP = 1, UDP = 2, };

    /**
     * ���캯��
     */
	EndpointInfo();

    /**
     * ���캯��
     * @param host
     * @param port
     * @param type
     */
	EndpointInfo(const string& host, uint16_t port, EndpointInfo::EType type, int32_t grid);

    /**
     * ��ַ���ַ�������
     *
     * @return string
     */
	string desc() const;

    /**
     * ��ȡ������
     *
     * @return const string&
     */
	const string& host() const;

    /**
     * ��ȡ�˿ں�
     *
     * @return uint16_t
     */
	uint16_t port() const;

    /**
     * ��ȡ·��״̬
     * @return int32_t
     */
	int32_t grid() const;

    /**
     * ��ȡ������ַ
     *
     * @return const struct sockaddr_in&
     */
	const struct sockaddr_in& addr() const;

    /**
     * ���ض˿�����
     *
     * @return EndpointInfo::EType
     */
	EndpointInfo::EType type() const;

    /**
     * ��ֵ����
     */
    EndpointInfo& operator = (const EndpointInfo& r);

    /**
     * ����
     * @param r
     *
     * @return bool
     */
    bool operator == (const EndpointInfo& r) const;

    /**
     * С��
     * @param r
     *
     * @return bool
     */
    bool operator < (const EndpointInfo& r) const;

private:
    /**
     * ��ַIP
     */
    string _host;

    /**
     * �˿ں�
     */
    uint16_t _port;

    /**
     * ·��״̬
     */
    int32_t _grid;

    /**
     * ����
     */
	EndpointInfo::EType _type;

    /**
     * ��ַ
     */
	struct sockaddr_in _addr;
};
/////////////////////////////////////////////////////////////////////////////
}
#endif

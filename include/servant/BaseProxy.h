#ifndef __TAF_BASE_PROXY_H_
#define __TAF_BASE_PROXY_H_

#include "servant/Global.h"
#include "servant/AppProtocol.h"

namespace taf
{
/**
 * ��ʱһ�����ʺ�����л�
 * ���ó�ʱ������
 * ���㵽ĳ̨�������ĳ�ʱ��, ���������ʱ�������߳�ʱ����������ֵ
 * Ĭ��60s��, ��ʱ���ô���>=2, ��ʱ����0.7����������ʱ����>500,
 * ��ʧЧ
 * ����ʧЧ��, ���󽫾����ܵ��л����������ܵķ�����, ��ÿ��tryTimeInterval����һ��, ����ɹ�����Ϊ�ָ�
 * ���������������ʧЧ, �����ѡ��һ̨����
 * ����ǻҶ�״̬�ķ���, ����ʧЧ����������л�, Ҳֻ��ת������ͬ�Ҷ�״̬�ķ���
 * @uint16_t minTimeoutInvoke, �������С�ĳ�ʱ����, Ĭ��2��(��checkTimeoutIntervalʱ���ڳ�����minTimeoutInvoke, �ż��㳬ʱ)
 * @uint32_t frequenceFailInvoke, ����ʧ�ܴ���
 * @uint32_t checkTimeoutInterval, ͳ��ʱ����, (Ĭ��60s, ����С��30s)
 * @float radio, ��ʱ���� > ��ֵ����Ϊ��ʱ�� ( 0.1<=radio<=1.0 )
 * @uint32_t tryTimeInterval, ����ʱ����
 */
struct CheckTimeoutInfo
{
    uint16_t minTimeoutInvoke;

    uint32_t checkTimeoutInterval;

    uint32_t frequenceFailInvoke;

    float radio;

    uint32_t tryTimeInterval;

    CheckTimeoutInfo() : minTimeoutInvoke(2), checkTimeoutInterval(60), frequenceFailInvoke(10), radio(0.9), tryTimeInterval(30) {}
};
/**
 * Proxy�Ļ���
 */
class BaseProxy : public TC_HandleBase, public TC_ThreadMutex
{
public:
    /**
     * ���캯��
     */
    BaseProxy(Communicator* comm);

    /**
     * ��������
     */
    virtual ~BaseProxy();

    /**
     * ���÷���
     * @param req
     * @return int
     */
    virtual int invoke(ReqMessagePtr& req) = 0;

    /**
     * �������Ӧ
     * @param req
     * @return int
     */
    virtual int finished(ReqMessagePtr& req) = 0;

    /**
     * @return Communicator*
     */
    Communicator* getCommunicator();

    /**
     * �ϱ�ģ������
     * @param req
     */
    void reportToStat(ReqMessagePtr& req, const string& obj, int result);
    
    /**
     * �ϱ�ģ������ ����
     * @param req
     */
    void sampleToStat(ReqMessagePtr& req, const string& obj,const string &ip);
    
protected:
    Communicator* _comm;
};
/////////////////////////////////////////////////////////////////////
}
#endif

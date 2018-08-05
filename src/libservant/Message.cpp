#include "servant/Message.h"
#include "servant/StatReport.h"
#include "servant/Application.h"

namespace taf
{
ReqMessage::ReqMessage()
: type(SYNC_CALL)
, proxy(NULL)
, monitor(NULL)
, callback(NULL)
, hashCode(ThreadPrivateData::INVALID_HASH_CODE)
, fromRpc(false)
{
    begtime.tv_sec = 0;
    begtime.tv_usec = 0;
    endtime.tv_sec = 0;
    endtime.tv_usec = 0;
}

ReqMessage::~ReqMessage() 
{
    //������첽�����һ�û�з���,
    //����һ����ʱ����Ϣ,�����첽�����̶߳���
    if (proxy && callback && response.iVersion == -1)
    {
        if (!proxy->getCommunicator()->isTerminating())
        {
            ReqMessagePtr req = new ReqMessage;

            req->request    = request;
            req->begtime    = begtime;
            req->endtime    = endtime;
            req->endpoint   = endpoint;
            req->callback   = callback;
            req->adapter    = adapter;
            req->type       = type;
            req->proxy      = proxy;
            req->monitor    = monitor;
            req->hashCode   = hashCode;
            req->response.iVersion = -1; 

            proxy->getCommunicator()->asyncProcThread()->put(req);
        }
    }
}
//////////////////////////////////////////////////////////////
}

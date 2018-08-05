#ifndef __TAF_CONNECTOR_H_
#define __TAF_CONNECTOR_H_

#include "servant/EndpointInfo.h"
#include "servant/Transceiver.h"
#include "servant/NetworkUtil.h"
#include "servant/Global.h"

namespace taf
{

class Transceiver;
class ObjectProxy;

/**
 * ������������һ��transceiver
 */
class Connector
{
public:

    /**
     * ��������
     * @param timeout
     *
     * @return TransceiverPtr
     */
    static TransceiverPtr connect(ObjectProxy *op, const EndpointInfo &ep, int timeout);
};
///////////////////////////////////////////////////
}
#endif

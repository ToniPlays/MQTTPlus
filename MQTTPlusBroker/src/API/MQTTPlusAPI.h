#pragma once

#include "MQTTPlusApiFields.h"
#include "MQTT/Broker.h"

namespace MQTTPlus::API
{
    struct MQTTServiceStatus
    {
        Field<int> Port;
        Field<MQTTPlus::BrokerStatus> BrokerStatus;
        Field<uint32_t> ConnectedClients;
    };
}

#pragma once

#include "MQTTPlusApiFields.h"

namespace MQTTPlus::API
{
    struct ClientInfo {
        float none = 0;
    };

    struct MQTTClient
    {
        enum class ConnStatus
        {
            Disconnected = 0,
            Connected = 1,
        };
        
        Field<std::string> ClientID;
        Field<ConnStatus> Status;
        Expandable<std::string, int> ClientInfo; //Id or object if expanded
        Array<std::string> Subscriptions;
    };
}

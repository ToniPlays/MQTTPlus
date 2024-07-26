#pragma once

#include "MQTTPlusApiFields.h"
#include "MQTT/Broker.h"
#include <chrono>

#include <cstddef>

namespace MQTTPlus::API
{
    struct ServiceInfo 
    {
        Field<bool> Running = true;
        Field<std::chrono::time_point<std::chrono::system_clock>> StartupTime;
    };

    struct Service
    {
        Field<std::string> Name;
        Expandable<std::nullptr_t, ServiceInfo> Info;
    };

    struct ServerStatus {
        Field<uint32_t> ServiceCount;
        Array<Service> Services;
    };
}

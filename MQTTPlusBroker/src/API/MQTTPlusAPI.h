#pragma once

#include "MQTTPlusApiFields.h"
#include "MQTT/Broker.h"
#include "Core/Service/ServiceManager.h"
#include <chrono>

#include <cstddef>

namespace MQTTPlus::API
{
    //Endpoint: Server
    struct Service
    {
        Field<std::string> Name;
        Field<bool> Running;
        Field<std::chrono::time_point<std::chrono::system_clock>> StartupTime;

        Service(Ref<MQTTPlus::Service> service)
        {
            Name = service->GetName(),
            Running = service->IsRunning();
            StartupTime = service->GetStartupTime();
        }
    };

    struct ServerStatus 
    {
        Field<std::chrono::time_point<std::chrono::system_clock>> StartupTime;
        Field<uint32_t> ServiceCount;
        Field<uint32_t> RunningServices;
        Expandable<std::nullptr_t, Array<Service>> Services;
        Expandable<std::nullptr_t, MQTTPlus::SystemStatus> Status;
    };

    //Endpoint: Devices
    struct APIDevice
    {
        Field<std::string> PublicID;
        Field<std::string> DeviceName;
        Field<std::string> Nickname;
        Field<uint8_t> Status;
        Field<std::string> LastSeen;
    };
}

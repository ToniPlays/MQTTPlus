#pragma once

#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include "Core/Service/ServiceManager.h"

namespace MQTTPlus::API
{
    class DeviceExpander
    {
    public:
        static Promise<APIDevice> Expand(const std::vector<APIDevice>& devices, const std::vector<std::string>& expandOpts);
    private:
        static Coroutine ExpandElement(JobInfo info, APIDevice device, const std::vector<std::string>& opts);
    };
}
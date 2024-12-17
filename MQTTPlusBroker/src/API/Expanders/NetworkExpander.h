#pragma once

#include "API/MQTTPlusAPI.h"
#include "Core/Service/ServiceManager.h"

namespace MQTTPlus::API
{
    class NetworkExpander
    {
    public:
        static Promise<APINetwork> Expand(const std::vector<APINetwork>& devices, const std::vector<std::string>& expandOpts);
    private:
        static Coroutine ExpandElement(JobInfo info, APINetwork device, const std::vector<std::string>& opts);
    };
}
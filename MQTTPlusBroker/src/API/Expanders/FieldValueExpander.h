#pragma once

#include "Core/Service/ServiceManager.h"
#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"

namespace MQTTPlus::API
{

    class FieldValueExpander
    {
    public:
        static Promise<MQTTFieldValue> Expand(const std::vector<MQTTFieldValue>& devices, const std::vector<std::string>& expandOpts);
    private:
        static Coroutine ExpandElement(JobInfo info, MQTTFieldValue device, const std::vector<std::string>& opts);
    };
}
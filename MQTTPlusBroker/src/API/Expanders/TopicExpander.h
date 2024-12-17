#pragma once

#include "API/MQTTPlusAPI.h"
#include "Core/Service/ServiceManager.h"

namespace MQTTPlus::API
{
    class TopicExpander
    {
    public:
        static Promise<MQTTTopic> Expand(const std::vector<MQTTTopic>& topics, const std::vector<std::string>& expandOpts);
    private:
        static Coroutine ExpandElement(JobInfo info, MQTTTopic topic, const std::vector<std::string>& opts);
    };
}
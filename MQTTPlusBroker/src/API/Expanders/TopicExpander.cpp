#include "TopicExpander.h"

namespace MQTTPlus::API
{
    Promise<MQTTTopic> TopicExpander::Expand(const std::vector<MQTTTopic>& topics, const std::vector<std::string>& expandOpts)
    {
        std::vector<Ref<Job>> jobs;
        jobs.reserve(topics.size());

        for(auto& topic : topics)
        {
            std::string name = topic.PublicID.Value();
            jobs.push_back(Job::Create(name, TopicExpander::ExpandElement, topic, expandOpts));
        }
            

        JobGraphInfo info = {
            .Name = "TopicExpander::Expand",
            .Stages = { { "Expand", 1.0, jobs } },
        };

        return ServiceManager::GetJobSystem()->Submit<MQTTTopic>(Ref<JobGraph>::Create(info));
    }

    Coroutine TopicExpander::ExpandElement(JobInfo info, MQTTTopic topic, const std::vector<std::string>& opts)
    {
        info.Result(topic);
        co_return;
    }
}
#include "NetworkExpander.h"

namespace MQTTPlus::API
{
    Promise<APINetwork> NetworkExpander::Expand(const std::vector<APINetwork>& networks, const std::vector<std::string>& expandOpts)
    {
        std::vector<Ref<Job>> jobs;
        jobs.reserve(networks.size());

        for(auto& network : networks)
        {
            std::string name = network.PublicID.Value();
            jobs.push_back(Job::Create(name, ExpandElement, network, expandOpts));
        }
            
        JobGraphInfo info = {
            .Name = "NetworkExpander::ExpandNetworks",
            .Stages = { { "Expand", 1.0, jobs } },
        };

        return ServiceManager::GetJobSystem()->Submit<APINetwork>(Ref<JobGraph>::Create(info));
    }

    Coroutine NetworkExpander::ExpandElement(JobInfo info, APINetwork network, const std::vector<std::string>& opts)
    {
        info.Result(network);
        co_return;
    }
}
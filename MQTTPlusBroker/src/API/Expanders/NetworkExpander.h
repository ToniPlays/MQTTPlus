#pragma once

#include "API/MQTTPlusAPI.h"
#include "Core/Service/ServiceManager.h"

namespace MQTTPlus::API
{
    static Coroutine ExpandNetworkElement(JobInfo info, APINetwork& network, const std::vector<std::string>& opts)
    {
        info.Result(network);
        co_return;
    }

    static Promise<APINetwork> ExpandNetworks(const std::vector<APINetwork>& networks, const std::vector<std::string>& expandOpts)
    {
        std::vector<Ref<Job>> jobs;
        jobs.reserve(networks.size());

        for(auto& network : networks)
        {
            std::string name = network.PublicID.Value();
            jobs.push_back(Job::Create(name, ExpandNetworkElement, network, expandOpts));
        }
            

        JobGraphInfo info = {
            .Name = "ExpandNetworks",
            .Stages = { { "Expand", 1.0, jobs } },
        };

        return ServiceManager::GetJobSystem()->Submit<APINetwork>(Ref<JobGraph>::Create(info));
    }
}
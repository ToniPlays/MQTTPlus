#pragma once

#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include "Core/Service/ServiceManager.h"

namespace MQTTPlus::API
{
    static Coroutine ExpanDeviceElement(JobInfo info, APIDevice device, const std::vector<std::string>& opts)
    {
        auto expands = GetObjectExpandOpts(opts);

        if(Contains<std::string>(expands, "network"))
        {
            auto id = device.Network.As<std::string>();
            device.Network = (co_await API::GetNetwork(id))[0];
        }

        if(Contains<std::string>(expands, "fields"))
        {
            std::string deviceId = device.PublicID.Value();
            device.Fields = (co_await API::GetFieldValues(deviceId))[0];
        }

        info.Result(device);
    }

    static Promise<APIDevice> ExpandDevices(const std::vector<APIDevice>& devices, const std::vector<std::string>& expandOpts)
    {
        std::vector<Ref<Job>> jobs;
        jobs.reserve(devices.size());
        
        for(auto& device : devices)
        {
            std::string deviceID = device.PublicID.Value();
            jobs.push_back(Job::Create(deviceID, ExpanDeviceElement, device, expandOpts));
        }
            
        JobGraphInfo info = {
            .Name = "ExpandDevices",
            .Stages = { { "Expand", 1.0, jobs } },
        };

        return ServiceManager::GetJobSystem()->Submit<APIDevice>(Ref<JobGraph>::Create(info));
    }
}
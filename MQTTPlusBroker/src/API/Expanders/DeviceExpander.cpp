#include "DeviceExpander.h"
#include "API/QueryTypes/NetworkQueryType.h"
#include "API/QueryTypes/FieldValueQueryType.h"

namespace MQTTPlus::API
{
    Promise<APIDevice> DeviceExpander::Expand(const std::vector<APIDevice>& devices, const std::vector<std::string>& expandOpts)
    {
        std::vector<Ref<Job>> jobs;
        jobs.reserve(devices.size());
        
        for(auto& device : devices)
        {
            std::string deviceID = device.PublicID.Value();
            jobs.push_back(Job::Create(deviceID, DeviceExpander::ExpandElement, device, expandOpts));
        }
            
        JobGraphInfo info = {
            .Name = "DeviceExpander::Expand",
            .Stages = { { "Expand", 1.0, jobs } },
        };

        return ServiceManager::GetJobSystem()->Submit<APIDevice>(Ref<JobGraph>::Create(info));
    }

    Coroutine DeviceExpander::ExpandElement(JobInfo info, APIDevice device, const std::vector<std::string>& opts)
    {
        auto expands = GetObjectExpandOpts(opts);
        auto objectOpts = GetExpandOpts(opts);

        if(Contains<std::string>(objectOpts, "network"))
        {
            auto id = device.Network.As<std::string>();
            if(!id.empty())
            {
                auto network = (co_await NetworkQueryType::Get(id))[0];
                device.Network = network;
            }
        }

        if(Contains<std::string>(objectOpts, "fields"))
        {
            std::string deviceId = device.PublicID.Value();
            device.Fields = (co_await FieldValueQueryType::GetAllForDevice(deviceId, expands))[0];
        }

        info.Result(device);
        co_return;
    }

    
}
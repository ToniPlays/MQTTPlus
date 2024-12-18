#pragma once
#include "Server/HTTPServer.h"
#include "Core/Service/ServiceManager.h"
#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include <nlohmann/json.hpp>
#include "API/JsonConverter.h"

namespace MQTTPlus
{
    static void InitServerEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        server.Post("/server", [](const std::string& message, Ref<HTTPClient> client) mutable -> Coroutine {
            
            json msg = json::parse(message);
            
            uint32_t count = ServiceManager::GetServices().size();
            
            ServerStatus status = {
                .StartupTime = ServiceManager::GetStartupTime(),
                .Threads = ServiceManager::GetJobSystem()->GetThreads().size(),
                .ServiceCount = count,
                .RunningServices = ServiceManager::GetRunningServiceCount(),
                .JobCount = ServiceManager::GetJobSystem()->GetJobCount(),
                .Services = nullptr,
                .Status = nullptr,
            };

            auto expandOpts = ExpandOpts(msg);

            if(Contains<std::string>(expandOpts, "data.services"))
            {
                std::vector<API::Service> s;
                s.reserve(count);
                
                for(auto& service : ServiceManager::GetServices())
                    s.emplace_back(service);
                
                status.Services = s;
            }

            if(Contains<std::string>(expandOpts, "data.status"))
                status.Status = ServiceManager::GetSystemStatus();
            
            json j = {};
            j["type"] = "server";
            j["data"] = status;
            
            client->Send(j.dump());

            co_return;
        });
    }
}

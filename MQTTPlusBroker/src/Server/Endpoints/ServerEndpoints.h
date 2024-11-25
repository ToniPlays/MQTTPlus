#pragma once
#include "Server/HTTPServer.h"
#include "Core/Service/ServiceManager.h"
#include "API/MQTTPlusAPI.h"
#include <nlohmann/json.hpp>
#include "API/JsonConverter.h"

namespace MQTTPlus
{
    static void InitServerEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        server.Post("/server", [](const std::string& message, Ref<HTTPClient> client) mutable {
            
            json msg = json::parse(message);
            
            uint32_t count = ServiceManager::GetServices().size();
            
            ServerStatus status = {
                .StartupTime = ServiceManager::GetStartupTime(),
                .ServiceCount = count,
                .RunningServices = ServiceManager::GetRunningServiceCount(),
                .Services = nullptr,
                .Status = nullptr,
            };

            if(ArrayContains(msg["opts"]["expands"], "data.services"))
            {
                std::vector<API::Service> s;
                s.reserve(count);
                
                for(auto& service : ServiceManager::GetServices())
                    s.emplace_back(service);
                
                status.Services = s;
            }

            if(ArrayContains(msg["opts"]["expands"], "data.status"))
                status.Status = ServiceManager::GetSystemStatus();
            
            json j = {};
            j["type"] = "server";
            j["data"] = status;
            
            client->Send(j.dump());
        });
    }
}

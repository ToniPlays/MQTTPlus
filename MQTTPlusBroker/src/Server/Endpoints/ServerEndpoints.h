#pragma once
#include "Server/HTTPServer.h"
#include "Core/ServiceManager.h"
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
            std::vector<API::Service> services;
            services.reserve(count);
            
            bool expandsServiceInfo = ArrayContains(msg["opts"]["expands"], "services.info");
            
            for(auto& service : ServiceManager::GetServices())
            {
                auto& s = services.emplace_back();
                s.Name = service->GetName();
                
                if(expandsServiceInfo)
                {
                    s.Info = ServiceInfo {
                        .Running = service->IsRunning(),
                        .StartupTime = service->GetStartupTime(),
                    };
                }
            }
            
            ServerStatus status = {
                .StartupTime = ServiceManager::GetStartupTime(),
                .ServiceCount = ServiceManager::GetServices().size(),
                .RunningServices = ServiceManager::GetRunningServiceCount(),
                .Services = services,
                .Status = nullptr,
            };

            if(ArrayContains(msg["opts"]["expands"], "status"))
                status.Status = ServiceManager::GetSystemStatus();
            
            json j = {};
            j["endpoint"] = "/server";
            j["data"] = status;
            
            client->Send(j.dump());
        });
    }
}

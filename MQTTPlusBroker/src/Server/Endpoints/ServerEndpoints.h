#pragma once
#include "Server/HTTPServer.h"
#include "Server/ServiceManager.h"
#include "API/MQTTPlusAPI.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitServerEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        server.Post("/server", [](const std::string& message, void*) mutable {
            
            json msg = json::parse(message);
            
            uint32_t count = ServiceManager::GetRunningServiceCount();
            std::vector<API::Service> services;
            services.reserve(count);
            
            for(auto& service : ServiceManager::GetServices())
            {
                auto& s = services.emplace_back();
                s.Name = service->GetName();
                
                if(ArrayContains(msg["opts"]["expands"], "services.info"))
                {
                    s.Info = ServiceInfo {
                        .Running = true,
                        .StartupTime = service->GetStartupTime(),
                    };
                }
            }
            
            
            ServerStatus status = {
                .ServiceCount = count,
                .Services = services,
            };
            
            json j = {};
            j["endpoint"] = "/server";
            j["data"] = status;
            
            return j.dump();
        });
    }
}

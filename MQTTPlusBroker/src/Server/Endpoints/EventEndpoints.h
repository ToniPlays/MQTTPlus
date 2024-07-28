#pragma once
#include "Server/HTTPServer.h"
#include "Core/ServiceManager.h"
#include "API/MQTTPlusAPI.h"
#include <nlohmann/json.hpp>
#include "API/JsonConverter.h"

namespace MQTTPlus
{
    static void InitEventEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        
        server.Post("/events", [](const std::string& message, HTTPClient& client) mutable {
            std::unordered_map<std::string, std::function<void(HTTPClient&)>> timedEvents;
            timedEvents["server.status"] = [](HTTPClient& client) {
                const SystemStatus& status = ServiceManager::GetSystemStatus();
                
                json j;
                j["endpoint"] = "/events";
                j["data"] = json({
                    { "type", "server.status" },
                    { "event_data", status }
                });
                client.Send(j.dump());
            };
            
            
            
            json msg = json::parse(message);
            
            for(auto& obj : msg["opts"]["listen"])
            {
                const std::string& type = obj["type"];
                float interval = obj["interval"];
                auto func = timedEvents[type];
                client.SetTimedFunction(interval, [&client, func]() { func(client); });
            }
        });
    }
}

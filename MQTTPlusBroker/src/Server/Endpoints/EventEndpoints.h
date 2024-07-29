#pragma once
#include "Server/HTTPServer.h"
#include "Core/ServiceManager.h"
#include "MQTT/Events.h"
#include "API/MQTTPlusAPI.h"
#include <nlohmann/json.hpp>
#include "API/JsonConverter.h"

namespace MQTTPlus
{

    static void InitEventEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        server.Post("/events", [](const std::string& message, Ref<HTTPClient> client) mutable {
            std::unordered_map<std::string, std::function<void(HTTPClient&)>> timedEvents = { {"server.status", [](HTTPClient& client) {
                const SystemStatus& status = ServiceManager::GetSystemStatus();
                
                json j;
                j["endpoint"] = "/events";
                j["data"] = json({
                    { "type", "server.status" },
                    { "event_data", status }
                });
                client.Send(j.dump());
            }}};

            std::unordered_map<std::string, std::string> eventMap = {
                { "mqtt.client_connection_status_changed", MQTTClientEvent::GetStaticName(), },
            };
            
            client->GetTimer().Clear(); //Clear timed functions

            json msg = json::parse(message);
            
            Ref<FrontendService> service = ServiceManager::GetService<FrontendService>();
            
            for(auto& obj : msg["opts"]["listen"])
            {
                const std::string& type = obj["type"];
                if(timedEvents.find(type) != timedEvents.end())
                {
                    auto func = timedEvents[type];
                    float interval = obj["interval"];
                    client->SetTimedFunction(interval, [client, func]() mutable { func(*client.Raw()); });
                }
                else if(eventMap.find(type) != eventMap.end())
                {
                    std::string ev = eventMap[type];
                    service->SetEventListener(client, ev);
                } else MQP_ERROR("Tried to subscribe to invalid event {}", type);
            }
        });
    }
}

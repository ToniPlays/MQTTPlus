#pragma once
#include "Server/HTTPServer.h"
#include "Core/Service/ServiceManager.h"
#include "MQTT/Events.h"
#include "API/MQTTPlusAPI.h"
#include <nlohmann/json.hpp>
#include "API/JsonConverter.h"

namespace MQTTPlus
{
    using namespace API;

    static void ServerStatusTimer(HTTPClient& client)
    {
        auto& status = ServiceManager::GetSystemStatus();

        json j = {};
        j["event"] = "server.status";
        j["data"] = status;

        client.Send(j.dump());
    }

    static void InitEventEndpoints(HTTPServer& server)
    {
        server.Post("/event", [](const std::string& message, Ref<HTTPClient> client) mutable {
            
            json msg = json::parse(message);
            json j;

            client->GetSubscribedEvents().clear();
            client->GetTimer().Clear();

            if(ArrayContains(msg["listen"], "server.status"))
                client->SetTimedFunction(2.5f, ServerStatusTimer);

            for(auto& e : msg["listen"])
                client->SubscribeToEvent(e);

            client->Send(j.dump());
        });
    }
}

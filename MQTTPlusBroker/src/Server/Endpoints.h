#pragma once

#include "HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"
#include "Server/ServiceManager.h"
#include "Server/Services/MQTTClientService.h"

#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitializeApiEndpoints(HTTPServer& server)
    {
        using namespace MQTTPlus::API;
        server.Post("/mqtt", [](const std::string& message, void* userData) {
            auto json = nlohmann::json::parse(message);
            ServiceManager* manager = (ServiceManager*)userData;
            
            auto mqtt = manager->GetService<MQTTClientService>();
            auto& broker = mqtt->GetBroker();
            auto socket = broker.GetWebSocket();
            
            MQTTServiceStatus status = {
                .Port = (socket ? socket->GetPort() : -1),
                .BrokerStatus = broker.GetStatus(),
                .ConnectedClients = broker.GetConnectedClientCount(),
            };
            
            nlohmann::json response;
            response["endpoint"] = "/mqtt";
            response["data"] = status;
            
            return response.dump();
        });
    }
}

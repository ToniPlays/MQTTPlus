#pragma once

#include "Client/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitializeClientServerEndpoints(HTTPServer& server)
    {
        using namespace MQTTPlus::API;
        server.Post("/clients", [](const std::string& message) {
            auto json = nlohmann::json::parse(message);
            
            nlohmann::json response;
            response["endpoint"] = "/clients";
            response["object"] = MQTTClient {
                .ClientID = std::string("This is some field"),
                .Status = MQTTClient::ConnStatus::Connected,
                .ClientInfo = std::string("gffryinbcmaphy")
            };
            
            return response.dump();
        });
    }
}

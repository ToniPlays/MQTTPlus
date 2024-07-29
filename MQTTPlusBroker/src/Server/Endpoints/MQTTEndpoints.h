#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include <nlohmann/json.hpp>
#include "API/JsonConverter.h"

namespace MQTTPlus
{
    static void InitMQTTEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        server.Post("/mqtt/devices", [](const std::string& message, Ref<HTTPClient> client) mutable {
            
            json msg = json::parse(message);
            
        });
    }
}

#pragma once

#include "MQTTPlusApiFields.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus::API
{
    using json = nlohmann::json;

    static void to_json(json& j, const MQTTClient& client)
    {
        j = json {
            { "client_id", client.ClientID },
            { "status", client.Status },
            { "client_info", client.ClientInfo },
        };
    }
    
}

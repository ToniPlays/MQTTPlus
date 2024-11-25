#pragma once
#include "Server/HTTPServer.h"
#include "Core/Service/ServiceManager.h"
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
    }
}

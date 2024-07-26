#pragma once

#include "HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"
#include "Server/ServiceManager.h"
#include "Server/Services/MQTTClientService.h"

#include "Endpoints/ServerEndpoints.h"

#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitializeApiEndpoints(HTTPServer& server)
    {
        InitServerEndpoints(server);
    }
}

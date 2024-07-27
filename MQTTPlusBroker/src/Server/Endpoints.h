#pragma once

#include "HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"

#include "Endpoints/ServerEndpoints.h"

namespace MQTTPlus
{
    static void InitializeApiEndpoints(HTTPServer& server)
    {
        InitServerEndpoints(server);
    }
}

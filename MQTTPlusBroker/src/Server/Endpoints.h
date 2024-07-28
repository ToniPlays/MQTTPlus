#pragma once

#include "HTTPServer.h"
#include "API/MQTTPlusAPI.h"

#include "Endpoints/ServerEndpoints.h"
#include "Endpoints/EventEndpoints.h"

namespace MQTTPlus
{
    static void InitializeApiEndpoints(HTTPServer& server)
    {
        InitServerEndpoints(server);
        InitEventEndpoints(server);
    }
}

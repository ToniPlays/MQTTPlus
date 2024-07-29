#pragma once

#include "HTTPServer.h"
#include "API/MQTTPlusAPI.h"

#include "Endpoints/ServerEndpoints.h"
#include "Endpoints/MQTTEndpoints.h"
#include "Endpoints/EventEndpoints.h"

namespace MQTTPlus
{
    static void InitializeApiEndpoints(HTTPServer& server)
    {
        InitServerEndpoints(server);
        InitMQTTEndpoints(server);
        InitEventEndpoints(server);
    }
}

#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
#include "API/QueryTypes/DeviceQueryType.h"
#include "Core/Coroutine.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitMQTTEndpoints(HTTPServer &server)
    {
        using namespace nlohmann;
        using namespace API;

        server.Post("/devices", [](const std::string &message, Ref<HTTPClient> client) mutable -> Coroutine
        {
            json msg = json::parse(message);
            json j = {};
            j["type"] = "devices";
            j["data"] = NULL;

            auto expandOpts = ExpandOpts(msg);

            j["data"] = (co_await DeviceQueryType::GetAll(expandOpts))[0];
            client->Send(j.dump());
        });


        server.Post("/device", [](const std::string &message, Ref<HTTPClient> client) mutable -> Coroutine {
            json msg = json::parse(message);
            json j = {};

            j["type"] = "device";
            j["data"] = NULL;
            
            if(msg["id"].is_null())
            {
                client->Send(j.dump());
                co_return;
            }

            std::string id = msg["id"];

            std::vector<std::string> expandOpts = ExpandOpts(msg);

            j["data"] = (co_await DeviceQueryType::Get(id, expandOpts))[0];
            client->Send(j.dump());
        });
    }
}

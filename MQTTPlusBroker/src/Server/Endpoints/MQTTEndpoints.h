#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
#include "API/Expanders/DeviceExpander.h"
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

            auto devices = co_await API::GetDevices();
            if(devices.size() > 0)
                j["data"] = co_await API::ExpandDevices(devices[0], expandOpts);
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
            auto results = co_await API::GetDevice(id);

            j["data"] = (co_await API::ExpandDevices(results, GetExpandOpts(expandOpts)))[0];
            client->Send(j.dump()); 
        });
    }
}

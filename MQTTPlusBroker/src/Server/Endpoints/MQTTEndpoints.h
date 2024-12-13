#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
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

            std::vector<std::string> expandOpts;
            if(!msg["opts"]["expands"].is_null())
                expandOpts = msg["opts"]["expands"].get<std::vector<std::string>>();

            const auto results = co_await API::GetDevices();
            const auto devices = API::RowsToDevices(results[0]);

            if(Contains<std::string>(expandOpts, "data.network"))
            {
                
            }

            j["data"] = devices;
            client->Send(j.dump());
        });


        server.Post("/device", [](const std::string &message, Ref<HTTPClient> client) mutable -> Coroutine {
            json msg = json::parse(message);
            if(msg["id"].is_null())
            {
                json j = {};
                j["type"] = "device";
                j["data"] = NULL;
                client->Send(j.dump());
                co_return;
            }

            std::string id = msg["id"];

            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();

            std::vector<std::string> expandOpts;
            if(!msg["opts"]["expands"].is_null())
                expandOpts = msg["opts"]["expands"].get<std::vector<std::string>>();

            json j = {};
            j["type"] = "device";
            /*if(devices.size() > 0)
                j["data"] = devices[0];*/
            j["data"] = NULL;

        client->Send(j.dump()); 
            co_return;
        });
    }
}

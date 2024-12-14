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

            auto results = co_await API::GetDevices();
            auto devices = API::RowsToDevices(results[0]);

            if(Contains<std::string>(expandOpts, "data.network"))
            {
                for(auto& device : devices)
                {
                    auto network = co_await API::GetNetwork(device.Network.GetValueAs<std::string>());
                    device.Network = API::RowToNetwork(network[0]);
                }
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

            std::vector<std::string> expandOpts;
            if(!msg["opts"]["expands"].is_null())
                expandOpts = msg["opts"]["expands"].get<std::vector<std::string>>();
        
            auto results = co_await API::GetDevice(id);
            
            json j = {};
            j["type"] = "device";
            if(results[0]->Rows() > 0)
                j["data"] = API::RowToDevice(results[0]);
            else j["data"] = NULL;

            client->Send(j.dump()); 
        });
    }
}

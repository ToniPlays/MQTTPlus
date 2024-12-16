#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/DataGetters.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
#include "API/Expanders/NetworkExpander.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitNetworkEndpoints(HTTPServer &server)
    {
        using namespace nlohmann;
        using namespace API;

        server.Post("/networks", [](const std::string &message, Ref<HTTPClient> client) mutable -> Coroutine
        {
            json msg = json::parse(message);
            json j = {};
            j["type"] = "networks";
            j["data"] = NULL;

            std::vector<std::string> expandOpts = ExpandOpts(msg);

            auto networks = co_await API::GetNetworks();
            if(networks.size() > 0)
                j["data"] = co_await API::ExpandNetworks(networks[0], expandOpts);
            client->Send(j.dump());
        });

        server.Post("/network", [](const std::string &message, Ref<HTTPClient> client) mutable -> Coroutine
        {
            json msg = json::parse(message);

            json j = {};
            j["type"] = "network";
            j["data"] = NULL;

            if(msg["id"].is_null())
            {
                client->Send(j.dump());
                co_return;
            }

            std::vector<std::string> expandOpts = ExpandOpts(msg);

            auto results = co_await API::GetNetwork(msg["id"]);
            if(results.size() > 0)
                j["data"] = results[0];
                
            client->Send(j.dump());
        });
    }
}
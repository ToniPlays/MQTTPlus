#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static API::APIDevice RowToDevice(const SQLQueryResult& result)
    {
        return API::APIDevice {
            .PublicID = result.Get<std::string>("devicePublicID"),
            .DeviceName = result.Get<std::string>("deviceName"),
            .Nickname = result.Get<std::string>("nickname"),
            .Status = result.Get<uint32_t>("deviceStatus"),
            .LastSeen = result.Get<std::string>("lastSeen"),
            .Network = result.Get<std::string>("networkPublicID"),
        };
    }

    static API::APINetwork RowToNetwork(const SQLQueryResult& result)
    {
        return API::APINetwork {
            .PublicID = result.Get<std::string>("networkPublicID"),
            .NetworkName = result.Get<std::string>("networkName"),
        };
    }

    static void GetApiDevices(const SQLQuery& query, const std::vector<std::string>& expand, const std::function<void(const std::vector<API::APIDevice>)>& cb)
    {
        Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();
        db->Transaction(query, [cb, expand](const SQLQueryResult& result) mutable {
            std::vector<API::APIDevice> devices;

            bool expandNetwork = Contains<std::string>(expand, "data.network");

            if(result.Rows() > 0)
            {
                while(result.Results->next())
                {
                    API::APIDevice device = RowToDevice(result);
                    if(expandNetwork)
                        device.Network = RowToNetwork(result);
                    devices.push_back(device);
                }
            }
            cb(devices);
        });
    }

    static void InitMQTTEndpoints(HTTPServer &server)
    {
        using namespace nlohmann;
        using namespace API;

        server.Post("/devices", [](const std::string &message, Ref<HTTPClient> client) mutable
        {
            json msg = json::parse(message);
            json j = {};
            j["type"] = "devices";

            std::vector<std::string> expandOpts;
            if(!msg["opts"]["expands"].is_null())
                expandOpts = msg["opts"]["expands"].get<std::vector<std::string>>();

            
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "devices.publicID", "devicePublicID" }, "deviceName", "nickname", { "devices.status", "deviceStatus" }, "lastSeen" },
                .Table = "devices"
            };

            if(Contains<std::string>(expandOpts, "data.network"))
            {
                query.Joins.push_back({ "networks", "networks.publicID", "devices.networkID", SQLJoinType::Left });
                query.Fields.emplace_back("networks.publicID", "networkPublicID");
                query.Fields.emplace_back("networks.networkName");
            }
            else query.Fields.push_back({ "devices.networkID", "networkPublicId" });

            GetApiDevices(query, expandOpts, [client, j](const auto& devices) mutable {
                j["data"] = devices;
                client->Send(j.dump());
            }); 
        });



        server.Post("/device", [](const std::string &message, Ref<HTTPClient> client) mutable {
            json msg = json::parse(message);
            if(msg["id"].is_null())
            {
                json j = {};
                j["type"] = "device";
                j["data"] = NULL;
                client->Send(j.dump());
                return;
            }

            std::string id = msg["id"];

            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();

            std::string sql = fmt::format("SELECT publicID, deviceName, nickname, status, lastSeen FROM devices WHERE publicID = '{0}'", id);

            db->Transaction(sql, [client](const SQLQueryResult& result) mutable { 
                json j = {};
                j["type"] = "device";

                if(!result.Results)
                {
                    j["data"] = nullptr;
                    client->Send(j.dump());
                    return;
                }

                result.Results->next();
                j["data"] = RowToDevice(result);

                client->Send(j.dump()); 
            });
        });
    }
}

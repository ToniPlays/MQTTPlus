#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus
{
    static void InitMQTTEndpoints(HTTPServer &server)
    {
        using namespace nlohmann;
        using namespace API;

        server.Post("/devices", [](const std::string &message, Ref<HTTPClient> client) mutable
                    {
            json msg = json::parse(message);
            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();

            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID" },
                .Table = "devices"
            };

            bool expand = ArrayContains(msg["opts"]["expands"], "data.devices");

            if(expand)
            {
                query.Fields.emplace_back("deviceName");
                query.Fields.emplace_back("nickname");
                query.Fields.emplace_back("status");
                query.Fields.emplace_back("lastSeen");
            }

            db->Transaction(query, [client, expand](sql::ResultSet* result) mutable {
                json j = {};
                j["type"] = "devices";

                if(!result)
                {
                    j["data"] = Array<std::string>();
                    client->Send(j.dump());
                    return;
                };

                std::vector<Expandable<APIDevice, std::string>> devices;
                devices.reserve(result->rowsCount());
            
                while(result->next())
                {
                    if(!expand)
                    {
                        devices.emplace_back(Expandable<APIDevice, std::string>(result->getString("publicID").c_str()));
                        continue;
                    }

                    auto d = APIDevice();
                    d.PublicID = result->getString("publicID").c_str();
                    d.DeviceName = result->getString("deviceName").c_str();
                    d.Nickname = result->getString("nickname").c_str();
                    d.Status = result->getUInt("status");
                    d.LastSeen = result->getString("lastSeen").c_str();

                    devices.emplace_back(d);
                }

                j["data"] = Array(devices);
                client->Send(j.dump());
            }); 
        });

        server.Post("/device", [](const std::string &message, Ref<HTTPClient> client) mutable {
            json msg = json::parse(message);
            if(msg["public_id"].is_null())
            {
                json j = {};
                j["type"] = "device";
                j["data"] = NULL;
                client->Send(j.dump());
                return;
            }

            std::string id = msg["public_id"];

            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();

            std::string sql = fmt::format("SELECT publicID, deviceName, nickname, status, lastSeen FROM devices WHERE publicID = '{0}'", id);

            db->Transaction(sql, [client](sql::ResultSet* result) mutable { 
                if(!result)
                {
                    json j = {};
                    j["type"] = "device";
                    j["data"] = nullptr;
                    client->Send(j.dump());
                    return;
                }

                result->next();

                APIDeviceInfo info = {};
                info.PublicID = result->getString("publicID").c_str();
                info.DeviceName = result->getString("deviceName").c_str();
                info.Nickname = result->getString("nickname").c_str();
                info.Status = result->getUInt("status");
                info.LastSeen = result->getString("lastSeen").c_str();

                json j = {};
                j["type"] = "device";
                j["data"] = info;

                client->Send(j.dump()); 
            });
        });  
    }
}

#pragma once

#include "Server/HTTPServer.h"
#include "API/MQTTPlusAPI.h"
#include "API/JsonConverter.h"
#include "Database/DatabaseService.h"
#include <nlohmann/json.hpp>


namespace MQTTPlus
{
    static void InitMQTTEndpoints(HTTPServer& server)
    {
        using namespace nlohmann;
        using namespace API;
        
        server.Post("/devices", [](const std::string& message, Ref<HTTPClient> client) mutable {
            
            json msg = json::parse(message);

            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();
            db->Transaction("SELECT publicID, deviceName, nickname, status, lastSeen FROM devices", [client](sql::ResultSet* result) mutable {
                if(!result)
                {
                    client->Send(""); //TODO:
                    return;
                };

                std::vector<APIDevice> devices;
                devices.reserve(result->rowsCount());

                while(result->next())
                {
                    auto& d = devices.emplace_back();
                    d.PublicID = result->getString("publicId").c_str();
                    d.DeviceName = result->getString("deviceName").c_str();
                    d.Nickname = result->getString("nickname").c_str();
                    d.Status = result->getUInt("status");
                    d.LastSeen = result->getString("lastSeen").c_str();
                }

                auto data = json::array();
                for(uint32_t i = 0; i < devices.size(); i++)
                    data.push_back(devices[i]);

                json j = {};
                j["endpoint"] = "/devices";
                j["data"] = data;

                client->Send(j.dump());
            });
        });
    }
}

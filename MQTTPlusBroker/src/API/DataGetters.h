#pragma once

#include "MQTTPlusAPI.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    static APIDevice RowToDevice(Ref<SQLQueryResult> result)
    {
        return APIDevice {
            .PublicID = result->Get<std::string>("publicID"),
            .DeviceName = result->Get<std::string>("deviceName"),
            .Nickname = result->Get<std::string>("nickname"),
            .Status = result->Get<uint32_t>("status"),
            .Network = result->Get<std::string>("networkID"),
        };
    }

    static std::vector<APIDevice> RowsToDevices(Ref<SQLQueryResult> result)
    {
        std::vector<APIDevice> devices;
        devices.reserve(result->Rows());
        if(result->Rows() > 0)
        {
            while(result->Results->next())
            {
                MQP_INFO("Test");
                devices.push_back(RowToDevice(result));
            }
        }
        MQP_INFO("Finished");
        return devices;
    }

    static Promise<Ref<SQLQueryResult>> GetDevice(const std::string& publicID)
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Table = "devices",
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    static Promise<Ref<SQLQueryResult>> GetDevices()
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
            .Table = "devices",
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    static Promise<Ref<SQLQueryResult>> GetNetwork(const std::string& publicID)
    {

    }

    static Promise<Ref<SQLQueryResult>> GetNetworks()
    {

    }
}
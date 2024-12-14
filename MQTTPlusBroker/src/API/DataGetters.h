#pragma once

#include "MQTTPlusAPI.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    static APIDevice RowToDevice(Ref<SQLQueryResult> result)
    {
        if(result->Rows() == 0) return {};
        result->Results->next();

        return APIDevice {
            .PublicID = result->Get<std::string>("publicID"),
            .DeviceName = result->Get<std::string>("deviceName"),
            .Nickname = result->Get<std::string>("nickname"),
            .Status = result->Get<uint32_t>("status"),
            .LastSeen = result->Get<std::string>("lastSeen"),
            .Network = result->Get<std::string>("networkID"),
        };
    }

    static std::vector<APIDevice> RowsToDevices(Ref<SQLQueryResult> result)
    {
        std::vector<APIDevice> devices;
        devices.reserve(result->Rows());
        if(result->Rows() > 0)
        {
            while(result->Results->getRow() != result->Rows())
                devices.push_back(RowToDevice(result));
        }
        return devices;
    }

    static Promise<Ref<SQLQueryResult>> GetDevice(const std::string& publicID)
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
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

    static APINetwork RowToNetwork(Ref<SQLQueryResult> result)
    {
        if(result->Rows() == 0) return {};
        result->Results->next();

        return APINetwork {
            .PublicID = result->Get<std::string>("publicID"),
            .NetworkName = result->Get<std::string>("networkName"),
            .NetworkType = 0,
            .ActiveDevices = 0,
            .TotalDevices = 0,
        };
    }

    static std::vector<APINetwork> RowsToNetworks(Ref<SQLQueryResult> result)
    {
        std::vector<APINetwork> networks;
        networks.reserve(result->Rows());
        if(result->Rows() > 0)
        {
            while(result->Results->getRow() != result->Rows())
                networks.push_back(RowToNetwork(result));
        }
        return networks;
    }

    static Promise<Ref<SQLQueryResult>> GetNetwork(const std::string& publicID)
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { { "publicID", "networkName", "description", "username", "password", "status" } },
            .Table = "networks",
            .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    static Promise<Ref<SQLQueryResult>> GetNetworks()
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { { "publicID", "networkName", "description", "username", "password", "status" } },
            .Table = "networks"
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }
}
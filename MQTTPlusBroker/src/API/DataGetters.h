#pragma once

#include "MQTTPlusAPI.h"
#include "Core/StringUtility.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    static std::vector<std::string> GetObjectExpandOpts(const std::vector<std::string> opts)
    {
        std::vector<std::string> results;
        for(auto& opt : opts)
        {
            if(opt.find_first_of('.') == std::string::npos)
            {
                if(opt.length() > 1)
                    results.emplace_back(opt);
            }
            else 
            {
                results.emplace_back(opt.substr(opt.find_first_of('.') + 1));
            }
        }
        return results;
    }

    static std::vector<std::string> GetExpandOpts(const std::vector<std::string>& opts)
    {
        std::vector<std::string> results;

        for(auto& opt : opts)
        {
            std::string string = opt.substr(0, opt.find_first_of('.'));
            if(string.length() == 0) continue;
            results.push_back(opt.substr(string.length() + 1));
        }

        return results;
    }


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
        
        if(result->Rows() > 0)
        {
            devices.reserve(result->Rows());
            while(result->Results->getRow() != result->Rows())
                devices.push_back(RowToDevice(result));
        }
        return devices;
    }
    static Promise<APIDevice> GetDevice(const std::string& publicID)
    {
        Ref<Job> job = Job::Lambda("GetDevice", [publicID](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
            .Table = "devices",
            .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
        };

            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(RowToDevice(results[0]));
        });

        return ServiceManager::GetJobSystem()->Submit<APIDevice>(job);
    }
    static Promise<std::vector<APIDevice>> GetDevices()
    {
        Ref<Job> job = Job::Lambda("GetDevices", [](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
                .Table = "devices",
            };
    
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(RowsToDevices(results[0]));
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<APIDevice>>(job);
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
        if(result->Rows() > 0)
        {
            networks.reserve(result->Rows());
            while(result->Results->getRow() != result->Rows())
                networks.push_back(RowToNetwork(result));
        }
        return networks;
    }
    static Promise<APINetwork> GetNetwork(const std::string& publicID)
    {
        Ref<Job> job = Job::Lambda("GetNetwork", [publicID](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "networkName", "description", "username", "password", "status" } },
                .Table = "networks",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
            };

            Ref<SQLQueryResult> results = (co_await ServiceManager::GetService<DatabaseService>()->Run(query))[0];
            info.Result(RowToNetwork(results));
        });

        return ServiceManager::GetJobSystem()->Submit<APINetwork>(job);
    }
    static Promise<std::vector<APINetwork>> GetNetworks(const std::vector<std::string> expandOpts = {})
    {
        Ref<Job> job = Job::Lambda("GetNetworks", [expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "networkName", "description", "username", "password", "status" } },
                .Table = "networks"
            };
            
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(RowsToNetworks(results[0]));
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<APINetwork>>(job);
    }

    static MQTTFieldValue RowToFieldValue(Ref<SQLQueryResult> result)
    {
        if(result->Rows() == 0) return {};
        result->Results->next();

        return MQTTFieldValue {
            .PublicID = result->Get<std::string>("publicID"),
            .DeviceID = result->Get<std::string>("deviceID"),
            .Topic = result->Get<std::string>("topicID"),
            .RawValue = result->Get<std::string>("rawValue"),
            .Formatter = result->Get<std::string>("formatter"),
            .DisplayValue = result->Get<std::string>("displayValue"),
            .LastUpdated = result->Get<std::string>("lastUpdated"),
        };
    }
    static std::vector<MQTTFieldValue> RowsToFieldValues(Ref<SQLQueryResult> result)
    {
        std::vector<MQTTFieldValue> networks;
        if(result->Rows() > 0)
        {
            networks.reserve(result->Rows());
            while(result->Results->getRow() != result->Rows())
                networks.push_back(RowToFieldValue(result));
        }
        return networks;
    }
    static Promise<std::vector<MQTTFieldValue>> GetFieldValues(const std::string& deviceId)
    {
        Ref<Job> job = Job::Lambda("GetFieldValues", [deviceId](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "topicID", "deviceID", "rawValue", "formatter", "displayValue", "lastUpdated" } },
                .Table = "topic_values",
                .Filters = { { "deviceID", SQLFieldFilterType::Equal, deviceId } }
            };
            
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(RowsToFieldValues(results[0]));
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<MQTTFieldValue>>(job);
    }
    static Promise<MQTTFieldValue> GetFieldValue(const std::string& fieldId)
    {
        Ref<Job> job = Job::Lambda("GetFieldValue", [fieldId](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "topicID", "deviceID", "rawValue", "formatter", "displayValue", "lastUpdated" } },
                .Table = "topic_values",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, fieldId } }
            };
            
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(RowToFieldValue(results[0]));
        });

        return ServiceManager::GetJobSystem()->Submit<MQTTFieldValue>(job);
    }

    static std::vector<std::string> ExpandOpts(const nlohmann::json& json)
    {
        if(!json.contains("opts")) return {};
        if(!json["opts"].contains("expands")) return {};

        if(!json["opts"]["expands"].is_null())
            return json["opts"]["expands"].get<std::vector<std::string>>();
        return {};
    }
}
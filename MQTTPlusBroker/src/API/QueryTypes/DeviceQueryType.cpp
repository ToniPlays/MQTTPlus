#include "DeviceQueryType.h"
#include "API/Expanders/DeviceExpander.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    Promise<APIDevice> DeviceQueryType::Get(const std::string& publicID, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("DeviceQueryType::Get", [publicID, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
                .Table = "devices",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
            };

            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto devices = Convert(results[0]);
            if(devices.size() == 0) co_return;

            devices = co_await DeviceExpander::Expand(devices, expandOpts);
            info.Result(devices[0]);
        });

        return ServiceManager::GetJobSystem()->Submit<APIDevice>(job);
    }

    Promise<std::vector<APIDevice>> DeviceQueryType::GetAll(const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("DeviceQueryType::GetAll", [expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
                .Table = "devices",
            };
    
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto devices = Convert(results[0]);
            if(devices.size() == 0) co_return;
            
            devices = co_await DeviceExpander::Expand(devices, expandOpts);
            info.Result(devices);
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<APIDevice>>(job);
    }

    Promise<APIDevice> DeviceQueryType::GetFromName(const std::string& deviceName, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("DeviceQueryType::GetAll", [deviceName, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID", "deviceName", "nickname", "status", "lastSeen", "networkID" },
                .Table = "devices",
                .Filters = { { "deviceName", SQLFieldFilterType::Equal, deviceName } }
            };
    
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto devices = DeviceQueryType::Convert(results[0]);
            if(devices.size() == 0) co_return;

            devices = co_await DeviceExpander::Expand(devices, expandOpts);
            info.Result(devices[0]);
        });

        return ServiceManager::GetJobSystem()->Submit<APIDevice>(job);
    }


    Promise<bool> DeviceQueryType::CreateDevice(const std::string& id, const std::string& deviceName, bool connected)
    {
        Ref<Job> job = Job::Lambda("DeviceQueryType::GetAll", [id, deviceName, connected](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Insert,
                .Fields = { { "publicID" }, { "deviceName" }, { "status" }, { "lastSeen" } },
                .Values = {  id , { deviceName }, { connected ? 1 : 0 }, { "NOW()", false } },
                .Table = "devices",
            };

            co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(true);
        });

        return ServiceManager::GetJobSystem()->Submit<bool>(job);
    }

    Promise<bool> DeviceQueryType::UpdateDevice(const std::string& id, const std::vector<SQLQueryField>& fields, const std::vector<SQLQueryFieldValue>& values)
    {
        Ref<Job> job = Job::Lambda("DeviceQueryType::GetAll", [id, fields, values](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Update,
                .Fields = fields,
                .Values = values,
                .Table = "devices",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, id }}
            };

            co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(true);
        });
        return ServiceManager::GetJobSystem()->Submit<bool>(job);
    }




    APIDevice DeviceQueryType::ConvertRow(Ref<SQLQueryResult> result)
    {
        if(result->Rows == 0) return {};
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
    std::vector<APIDevice> DeviceQueryType::Convert(Ref<SQLQueryResult> result)
    {
        std::vector<APIDevice> devices;
        
        if(result->Rows > 0)
        {
            devices.reserve(result->Rows);
            while(result->Results->getRow() != result->Rows)
                devices.push_back(ConvertRow(result));
        }
        return devices;
    }

}

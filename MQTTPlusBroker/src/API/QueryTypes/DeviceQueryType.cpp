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
            devices = co_await DeviceExpander::Expand(devices, expandOpts);
            info.Result(devices);
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<APIDevice>>(job);
    }

     APIDevice DeviceQueryType::ConvertRow(Ref<SQLQueryResult> result)
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
    std::vector<APIDevice> DeviceQueryType::Convert(Ref<SQLQueryResult> result)
    {
        std::vector<APIDevice> devices;
        
        if(result->Rows() > 0)
        {
            devices.reserve(result->Rows());
            while(result->Results->getRow() != result->Rows())
                devices.push_back(ConvertRow(result));
        }
        return devices;
    }

}

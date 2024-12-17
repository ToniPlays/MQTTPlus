#include "NetworkQueryType.h"
#include "API/Expanders/NetworkExpander.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    Promise<APINetwork> NetworkQueryType::Get(const std::string& publicID, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("NetworkQueryType::Get", [publicID, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "networkName", "description", "username", "password", "status" } },
                .Table = "networks",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
            };

            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto networks = Convert(results[0]);
            if(networks.size() == 0) co_return;

            auto network = (co_await NetworkExpander::Expand(networks, expandOpts))[0];
            info.Result(network);
        });

        return ServiceManager::GetJobSystem()->Submit<APINetwork>(job);
    }

    Promise<std::vector<APINetwork>> NetworkQueryType::GetAll(const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("NetworkQueryType::GetAll", [expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "networkName", "description", "username", "password", "status" } },
                .Table = "networks"
            };
    
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto networks = Convert(results[0]);
            if(networks.size() == 0) co_return;
            
            networks = co_await NetworkExpander::Expand(networks, expandOpts);
            info.Result(networks);
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<APINetwork>>(job);
    }

    APINetwork NetworkQueryType::ConvertRow(Ref<SQLQueryResult> result)
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
    std::vector<APINetwork> NetworkQueryType::Convert(Ref<SQLQueryResult> result)
    {
        std::vector<APINetwork> devices;
        
        if(result->Rows() > 0)
        {
            devices.reserve(result->Rows());
            while(result->Results->getRow() != result->Rows())
                devices.push_back(ConvertRow(result));
        }
        return devices;
    }

}

#pragma once

#include "API/MQTTPlusAPI.h"
#include "Database/SQLQuery.h"

namespace MQTTPlus::API 
{
    class NetworkQueryType
    {
    public:
        static Promise<APINetwork> Get(const std::string& publicID, const std::vector<std::string>& expandOpts = {});
        static Promise<std::vector<APINetwork>> GetAll(const std::vector<std::string>& expandOpts = {});

     private:
        static APINetwork ConvertRow(Ref<SQLQueryResult> result);
        static std::vector<APINetwork> Convert(Ref<SQLQueryResult> result);
    };
}
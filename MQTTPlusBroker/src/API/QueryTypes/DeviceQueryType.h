#pragma once

#include "API/MQTTPlusAPI.h"
#include "Database/SQLQuery.h"

namespace MQTTPlus::API 
{
    class DeviceQueryType
    {
    public:
        static Promise<APIDevice> Get(const std::string& publicID, const std::vector<std::string>& expandOpts = {});
        static Promise<std::vector<APIDevice>> GetAll(const std::vector<std::string>& expandOpts = {});

     private:
        static APIDevice ConvertRow(Ref<SQLQueryResult> result);
        static std::vector<APIDevice> Convert(Ref<SQLQueryResult> result);
    };
}
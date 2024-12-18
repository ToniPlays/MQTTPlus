#pragma once

#include "API/MQTTPlusAPI.h"
#include "Database/SQLQuery.h"

namespace MQTTPlus::API 
{
    class FieldValueQueryType
    {
    public:
        static Promise<MQTTFieldValue> Get(const std::string& publicID, const std::vector<std::string>& expandOpts = {});
        static Promise<std::vector<MQTTFieldValue>> GetAllForDevice(const std::string& deviceId, const std::vector<std::string>& expandOpts = {});

     private:
        static MQTTFieldValue ConvertRow(Ref<SQLQueryResult> result);
        static std::vector<MQTTFieldValue> Convert(Ref<SQLQueryResult> result);
    };
}
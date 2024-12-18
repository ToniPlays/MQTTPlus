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

        static Promise<APIDevice> GetFromName(const std::string& deviceName, const std::vector<std::string>& expandOpts = {});

        static Promise<bool> CreateDevice(const std::string& id, const std::string& deviceName, bool connected);

        static Promise<bool> UpdateDevice(const std::string& id, const std::vector<SQLQueryField>& fields, const std::vector<SQLQueryFieldValue>& values);

        static APIDevice ConvertRow(Ref<SQLQueryResult> result);
        static std::vector<APIDevice> Convert(Ref<SQLQueryResult> result);
    };
}
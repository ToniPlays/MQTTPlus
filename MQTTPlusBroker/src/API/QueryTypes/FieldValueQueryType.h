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

        static Promise<MQTTFieldValue> GetFieldFromDeviceAndTopic(const std::string& deviceId, const std::string& topicId, const std::vector<std::string>& expandOpts = {});
        static Promise<bool> Create(const std::string& publicID, const std::string& deviceId, const std::string& topicId, const std::string& value);
        static Promise<bool> Update(const std::string& publicID, const std::vector<SQLQueryField>& fields, const std::vector<SQLQueryFieldValue>& values);
        //static Promise<MQTTFieldValue> GetFieldFromDeviceAndTopic(const std::string& deviceId, const std::string& topicId, const std::vector<std::string>& expandOpts = {});


     private:
        static MQTTFieldValue ConvertRow(Ref<SQLQueryResult> result);
        static std::vector<MQTTFieldValue> Convert(Ref<SQLQueryResult> result);
    };
}
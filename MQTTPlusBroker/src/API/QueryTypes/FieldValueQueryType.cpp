#include "FieldValueQueryType.h"
#include "API/Expanders/DeviceExpander.h"
#include "API/Expanders/FieldValueExpander.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    Promise<MQTTFieldValue> FieldValueQueryType::Get(const std::string& publicID, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("FieldValueQueryType::Get", [publicID, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "deviceID", "topicID", "rawValue", "formatter", "displayValue", "lastUpdated" } },
                .Table = "topic_values",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
            };
            
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto fields = Convert(results[0]);
            fields = co_await FieldValueExpander::Expand(fields, expandOpts);
            info.Result(fields);
        });

        return ServiceManager::GetJobSystem()->Submit<MQTTFieldValue>(job);
    }

    Promise<std::vector<MQTTFieldValue>> FieldValueQueryType::GetAllForDevice(const std::string& deviceId, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("FieldValueQueryType::GetAll", [deviceId, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "deviceID", "topicID", "rawValue", "formatter", "displayValue", "lastUpdated" } },
                .Table = "topic_values",
                .Filters = { { "deviceID", SQLFieldFilterType::Equal, deviceId } }
            };
            
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto fields = Convert(results[0]);
            fields = co_await FieldValueExpander::Expand(fields, expandOpts);
            info.Result(fields);
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<MQTTFieldValue>>(job);
    }

    MQTTFieldValue FieldValueQueryType::ConvertRow(Ref<SQLQueryResult> result)
    {
        if(result->Rows == 0) return {};
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
    std::vector<MQTTFieldValue> FieldValueQueryType::Convert(Ref<SQLQueryResult> result)
    {
        std::vector<MQTTFieldValue> fields;
        if(result->Rows > 0)
        {
            fields.reserve(result->Rows);
            while(result->Results->getRow() != result->Rows)
                fields.push_back(ConvertRow(result));
        }
        return fields;
    }
}

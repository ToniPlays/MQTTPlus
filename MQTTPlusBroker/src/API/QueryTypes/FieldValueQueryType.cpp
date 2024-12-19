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
            if(results[0]->Rows == 0) co_return;

            auto fields = Convert(results[0]);
            if(fields.size() > 0)
                fields = co_await FieldValueExpander::Expand(fields, expandOpts);
            info.Result(fields[0]);
        });

        return ServiceManager::GetJobSystem()->Submit<MQTTFieldValue>(job);
    }

    Promise<std::vector<MQTTFieldValue>> FieldValueQueryType::GetAllForDevice(const std::string& deviceId, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("FieldValueQueryType::GetAllForDevice", [deviceId, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "deviceID", "topicID", "rawValue", "formatter", "displayValue", "lastUpdated" } },
                .Table = "topic_values",
                .Filters = { { "deviceID", SQLFieldFilterType::Equal, deviceId } }
            };
            
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            if(results[0]->Rows == 0) co_return;

            auto fields = Convert(results[0]);
            if(fields.size() > 0)
                fields = co_await FieldValueExpander::Expand(fields, expandOpts);

            info.Result(fields);
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<MQTTFieldValue>>(job);
    }

    Promise<MQTTFieldValue> FieldValueQueryType::GetFieldFromDeviceAndTopic(const std::string& deviceId, const std::string& topicId, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("FieldValueQueryType::GetFieldFromDeviceAndTopic", [deviceId, topicId, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { { "publicID", "deviceID", "topicID", "rawValue", "formatter", "displayValue", "lastUpdated" } },
                .Table = "topic_values",
                .Filters = { { "deviceId", SQLFieldFilterType::Equal, deviceId }, { "topicId", SQLFieldFilterType::Equal, topicId } }
            };

            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            if(results[0]->Rows == 0) co_return;
            auto field = Convert(results[0]);

            //if(field.size() >= 0)
            //    fields = co_await FieldValueQueryType::Expand(field);
            info.Result(field[0]);
        });

        return ServiceManager::GetJobSystem()->Submit<MQTTFieldValue>(job);
    }

    Promise<bool> FieldValueQueryType::Create(const std::string& publicID, const std::string& deviceId, const std::string& topicId, const std::string& value)
    {
        Ref<Job> job = Job::Lambda("FieldValueQueryType::GetFieldFromDeviceAndTopic", [publicID, deviceId, topicId, value](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Insert,
                .Fields = { "publicID", "topicID", "deviceID", "rawValue", "displayValue", "lastUpdated" },
                .Values = { publicID, topicId, deviceId, value, value, { "NOW()", false } },
                .Table = "topic_values",
            };

            co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(true);
        });

        return ServiceManager::GetJobSystem()->Submit<bool>(job);
    }

    Promise<bool> FieldValueQueryType::Update(const std::string& publicID, const std::vector<SQLQueryField>& fields, const std::vector<SQLQueryFieldValue>& values)
    {
        Ref<Job> job = Job::Lambda("FieldValueQueryType::Update", [publicID, fields, values](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Update,
                .Fields = fields,
                .Values = values,
                .Table = "topic_values",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
            };

            co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            info.Result(true);
        });

        return ServiceManager::GetJobSystem()->Submit<bool>(job);
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

#include "TopicQueryType.h"
#include "API/Expanders/TopicExpander.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    Promise<MQTTTopic> TopicQueryType::Get(const std::string& publicID, const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("TopicQueryType::Get", [publicID, expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID", "topicName", "displayName" },
                .Table = "topics",
                .Filters = { { "publicID", SQLFieldFilterType::Equal, publicID } }
            };

            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto topics = Convert(results[0]);
            auto topic = (co_await TopicExpander::Expand(topics, expandOpts))[0];
            info.Result(topic);
        });

        return ServiceManager::GetJobSystem()->Submit<MQTTTopic>(job);
    }

    Promise<std::vector<MQTTTopic>> TopicQueryType::GetAll(const std::vector<std::string>& expandOpts)
    {
        Ref<Job> job = Job::Lambda("TopicQueryType::GetAll", [expandOpts](JobInfo info) mutable -> Coroutine {
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicID", "topicName", "displayName" },
                .Table = "topics",
            };
    
            auto results = co_await ServiceManager::GetService<DatabaseService>()->Run(query);
            auto topics = Convert(results[0]);
            topics = co_await TopicExpander::Expand(topics, expandOpts);
            info.Result(topics);
        });

        return ServiceManager::GetJobSystem()->Submit<std::vector<MQTTTopic>>(job);
    }

    MQTTTopic TopicQueryType::ConvertRow(Ref<SQLQueryResult> result)
    {
        if(result->Rows() == 0) return {};
        result->Results->next();

        return MQTTTopic {
            .PublicID = result->Get<std::string>("publicID"),
            .TopicName = result->Get<std::string>("topicName"),
            .DisplayName = result->Get<std::string>("displayName"),
        };
    }
    std::vector<MQTTTopic> TopicQueryType::Convert(Ref<SQLQueryResult> result)
    {
        std::vector<MQTTTopic> topics;
        
        if(result->Rows() > 0)
        {
            topics.reserve(result->Rows());
            while(result->Results->getRow() != result->Rows())
                topics.push_back(ConvertRow(result));
        }
        return topics;
    }

}

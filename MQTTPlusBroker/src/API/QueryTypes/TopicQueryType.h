#pragma once

#include "API/MQTTPlusAPI.h"
#include "Database/SQLQuery.h"

namespace MQTTPlus::API 
{
    class TopicQueryType
    {
    public:
        static Promise<MQTTTopic> Get(const std::string& publicID, const std::vector<std::string>& expandOpts = {});
        static Promise<MQTTTopic> GetFromNameAndNetwork(const std::string& topicName, const std::string& networkID, const std::vector<std::string>& expandOpts = {});
        static Promise<std::vector<MQTTTopic>> GetAll(const std::vector<std::string>& expandOpts = {});
        
        static Promise<bool> Create(const std::string& id, const std::string& topicName, const std::string& networkID, uint32_t type = 0);

     private:
        static MQTTTopic ConvertRow(Ref<SQLQueryResult> result);
        static std::vector<MQTTTopic> Convert(Ref<SQLQueryResult> result);
    };
}
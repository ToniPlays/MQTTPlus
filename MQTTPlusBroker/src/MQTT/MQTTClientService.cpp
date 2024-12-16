#include "MQTTClientService.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"
#include "Events.h"
#include "Core/Logger.h"
#include "Core/StringUtility.h"

#include <spdlog/fmt/fmt.h>

namespace MQTTPlus
{
    MQTTClientService::MQTTClientService(uint32_t port)
    {
        BrokerCreateSettings settings = {
            .Port = port,
            .UseSSL = false,
        };
        
        m_Broker = new Broker(settings);
    }

    void MQTTClientService::Start() {
        
        m_StartupTime = std::chrono::system_clock::now();
        
        m_Broker->SetOnClientChange([this](Ref<MQTTClient> client, bool connected, int reason) {
            MQTTClientEvent e(client, connected);
            OnMQTTClientEvent(e);

            MQP_INFO("{} MQTT client {}", connected ? "Connected" : "Disconnected", client->GetAuth().ClientID);
        });

        m_Broker->SetOnPublished([this](Ref<MQTTClient> client, const std::string& topic, const std::string& message) {
            HandleValuePublish(client, topic, message);
        });
        
        m_Broker->SetOnSubscribed([this](Ref<MQTTClient> client, const MQTT::SubscribeMessage::Topic& topic) {
            MQP_WARN("{} subscribed to {} (QoS: {})", client->GetAuth().ClientID, topic.Topic, topic.QOS);
        });

        m_Broker->Listen();
    }

    void MQTTClientService::Stop() 
    {
        
    }
    
    void MQTTClientService::OnEvent(Event& e)
    {

    }

    void MQTTClientService::OnMQTTClientEvent(MQTTClientEvent& e)
    {
        if(e.GetClient().GetAuth().ClientID.empty()) return;

        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "publicId" },
            .Table = "devices",
            .Filters = { { "deviceName", SQLFieldFilterType::Equal, e.GetClient().GetAuth().ClientID } }
        };

        Ref<Job> job = Job::Lambda("Update", [query, e](JobInfo info) mutable -> Coroutine {
            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();
            auto deviceName = e.GetClient().GetAuth().ClientID;

            auto results = co_await db->Run(query);
            auto result = results[0];
            
            if(result->Rows() == 0)
            {
                std::string id = "de_" + StringUtility::Hex16();
                SQLQuery insert = {
                    .Type = SQLQueryType::Insert,
                    .Fields = { { "publicID" }, { "deviceName" }, { "status" }, { "lastSeen" } },
                    .Values = { { id }, { deviceName }, { e.IsConnected() ? 1 : 0 }, { "NOW()", false } },
                    .Table = "devices",
                };

                e.GetClient().SetPublicID(id);
                co_await db->Run(insert);
            }
            else 
            {
                result->Results->next();
                e.GetClient().SetPublicID(result->Get<std::string>("publicId"));
                
                SQLQuery update = {
                    .Type = SQLQueryType::Update,
                    .Fields = { { "status" }, { "lastSeen" } },
                    .Values = { { e.IsConnected() ? 1 : 0 }, { "NOW()", false } },
                    .Table = "devices",
                    .Filters = { { "deviceName", SQLFieldFilterType::Equal, deviceName }}
                };
                
                co_await db->Run(update);
            }
            
            ServiceManager::OnEvent(e);
        });

        JobGraphInfo info = {
            .Name = "Client change",
            .Stages = { { "Run", 1.0f, { job } } }
        };

        ServiceManager::GetJobSystem()->Submit<bool>(Ref<JobGraph>::Create(info));
    }

    void MQTTClientService::HandleValuePublish(Ref<MQTTClient> client, const std::string& topic, const std::string& message)
    {
        Ref<Job> job = Job::Lambda("PublishUpdate", [client, topic, message](JobInfo info) mutable -> Coroutine {
            const auto& auth = client->GetAuth();

            MQP_TRACE("{} published {} to {}", auth.ClientID, message, topic);
            auto result = co_await GetTopic(topic, auth.NetworkID);

            std::string topicPublicId = "to_" + StringUtility::Hex16();

            if(result[0]->Rows() == 0) 
            {
                co_await CreateTopic(topicPublicId, 0, topic, auth.NetworkID);
            }
            else
            {
                result[0]->Results->next();
                topicPublicId = result[0]->Get<std::string>("topicID");
            }

            auto field = (co_await GetTopicFieldIdForDevice(auth.PublicId, topicPublicId))[0];
            if(field->Rows() == 0)
            {
                std::string fieldId = "fi_" + StringUtility::Hex16();
                co_await CreateTopicField(fieldId, client, topicPublicId, message);
            }
            else 
            {
                field->Results->next();
                std::string fieldId = field->Get<std::string>("publicID");
                co_await UpdateTopicField(fieldId, message, field->Get<std::string>("formatter"));
            }
        });

        JobGraphInfo info = {
            .Name = "Value publish",
            .Stages = { { "Run", 1.0f, { job } } }
        };

        ServiceManager::GetJobSystem()->Submit<bool>(Ref<JobGraph>::Create(info));
    }

    Promise<Ref<SQLQueryResult>> MQTTClientService::GetTopicFieldIdForDevice(const std::string& deviceId, const std::string& topicId)
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "publicID", "formatter" },
            .Table = "topic_values",
            .Filters = { { "deviceId", SQLFieldFilterType::Equal, deviceId }, { "topicId", SQLFieldFilterType::Equal, topicId } }
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    Promise<Ref<SQLQueryResult>> MQTTClientService::CreateTopicField(const std::string& fieldId, Ref<MQTTClient> client, const std::string& topicId, const std::string& value)
    {
        SQLQuery query = {
            .Type = SQLQueryType::Insert,
            .Fields = { "publicID", "topicID", "deviceID", "rawValue", "displayValue", "lastUpdated" },
            .Values = { fieldId, topicId, client->GetAuth().PublicId, value, value, { "NOW()", false } },
            .Table = "topic_values",
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    Promise<Ref<SQLQueryResult>> MQTTClientService::UpdateTopicField(const std::string& fieldId, const std::string& value, const std::string& formatter)
    {
        std::string formattedValue = value;
        if(!formatter.empty())
        {
            MQP_INFO("Run data formatter {} for value: {}", formatter, value);
            formattedValue = value;
        }

        SQLQuery query = {
            .Type = SQLQueryType::Update,
            .Fields = { "rawValue", "displayValue", "lastUpdated" },
            .Values = { value, formattedValue, { "NOW()", false } },
            .Table = "topic_values",
            .Filters = { { "publicID", SQLFieldFilterType::Equal, fieldId } }
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    Promise<Ref<SQLQueryResult>> MQTTClientService::GetTopic(const std::string& topic, const std::string& networkId)
    {
        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { { "topics.publicId", "topicId" }, "topicName", "topicType", { "topic_values.publicId", "fieldPublicId" } },
            .Table = "topics",
            .Filters = { { "topics.topicName", SQLFieldFilterType::Equal, topic },
                         { "topics.networkID", SQLFieldFilterType::Equal, networkId },
                       },
            .Joins = { { "topic_values", "topics.publicId", "topic_values.topicId", SQLJoinType::Left } },
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }

    Promise<Ref<SQLQueryResult>> MQTTClientService::CreateTopic(const std::string& id, uint32_t type, const std::string& name, const std::string& networkId)
    {
        MQP_INFO("Creating new topic {}", id);

        SQLQuery query = {
            .Type = SQLQueryType::Insert,
            .Fields = { "publicID", "topicName", "topicType", "createdAt", "networkID" },
            .Values = { id, name, type, { "NOW()", false, }, networkId },
            .Table = "topics"
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }
}

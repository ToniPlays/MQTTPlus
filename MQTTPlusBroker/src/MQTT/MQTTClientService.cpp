#include "MQTTClientService.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"
#include "Events.h"
#include "Core/Logger.h"
#include "Core/StringUtility.h"

#include "API/QueryTypes/DeviceQueryType.h"
#include "API/QueryTypes/TopicQueryType.h"

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


        Ref<Job> job = Job::Lambda("Update", [e](JobInfo info) mutable -> Coroutine {
            
            SQLQuery query = {
                .Type = SQLQueryType::Select,
                .Fields = { "publicId" },
                .Table = "devices",
                .Filters = { { "deviceName", SQLFieldFilterType::Equal, e.GetClient().GetAuth().ClientID } }
            };

            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();
            auto deviceName = e.GetClient().GetAuth().ClientID;

            auto result = (co_await API::DeviceQueryType::GetFromName(deviceName))[0];
            
            std::string deviceID = result.PublicID;

            if(deviceID.empty())
            {
                deviceID = "de_" + StringUtility::Hex16();
                co_await API::DeviceQueryType::CreateDevice(deviceID, deviceName, e.IsConnected());
            }
            else 
                co_await API::DeviceQueryType::UpdateDevice(deviceID, { { "status" }, { "lastSeen" } }, { { e.IsConnected() ? 1 : 0 }, { "NOW()", false } } );

            e.GetClient().SetPublicID(deviceID);
            ServiceManager::OnEvent(e);
        });

        ServiceManager::GetJobSystem()->Submit<bool>(job);
    }

    void MQTTClientService::HandleValuePublish(Ref<MQTTClient> client, const std::string& topic, const std::string& message)
    {
        Ref<Job> job = Job::Lambda("Value publish", [client, topic, message](JobInfo info) mutable -> Coroutine {
            const auto& auth = client->GetAuth();

            MQP_TRACE("{} published {} to {}", auth.ClientID, message, topic);
            auto result = (co_await API::TopicQueryType::GetFromNameAndNetwork(topic, auth.NetworkID))[0];

            std::string topicPublicId = result.PublicID;

            if(topicPublicId.empty()) 
            {
                topicPublicId = "to_" + StringUtility::Hex16();
                co_await API::TopicQueryType::Create(topicPublicId, topic, auth.NetworkID, 0);
            }

            auto field = (co_await GetTopicFieldIdForDevice(auth.PublicId, topicPublicId))[0];
            if(field->Rows == 0)
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

            MQTTPublishEvent e(client);
        });

        ServiceManager::GetJobSystem()->Submit<bool>(job);
    }


    //Deprecate these
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
            formattedValue = fmt::format(fmt::runtime(formatter), value);

        SQLQuery query = {
            .Type = SQLQueryType::Update,
            .Fields = { "rawValue", "displayValue", "lastUpdated" },
            .Values = { value, formattedValue, { "NOW()", false } },
            .Table = "topic_values",
            .Filters = { { "publicID", SQLFieldFilterType::Equal, fieldId } }
        };

        return ServiceManager::GetService<DatabaseService>()->Run(query);
    }
}

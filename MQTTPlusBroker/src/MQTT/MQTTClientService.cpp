#include "MQTTClientService.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"
#include "Events.h"
#include "Core/Logger.h"
#include "Core/StringUtility.h"

#include "API/QueryTypes/DeviceQueryType.h"
#include "API/QueryTypes/TopicQueryType.h"
#include "API/QueryTypes/FieldValueQueryType.h"

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

            if(result.PublicID.Value().empty()) 
            {
                result.PublicID = "to_" + StringUtility::Hex16();
                co_await API::TopicQueryType::Create(result.PublicID, topic, auth.NetworkID, 0);
            }

            auto field = (co_await API::FieldValueQueryType::GetFieldFromDeviceAndTopic(auth.PublicId, result.PublicID))[0];
            if(field.PublicID.Value().empty())
            {
                field.PublicID = "fi_" + StringUtility::Hex16();
                co_await API::FieldValueQueryType::Create(field.PublicID, client->GetAuth().PublicId, result.PublicID, message);
            }
            else 
            {
                std::string formattedValue = message;
                if(!field.Formatter.Value().empty())
                    formattedValue = fmt::format(fmt::runtime(field.Formatter.Value()), message);

                std::vector<SQLQueryField> fields = { "rawValue", "displayValue", "lastUpdated" };
                std::vector<SQLQueryFieldValue> values = { message, formattedValue, { "NOW()", false } };
                co_await API::FieldValueQueryType::Update(field.PublicID, fields, values);
            }

            MQTTPublishEvent e(client);
            ServiceManager::OnEvent(e);
        });

        ServiceManager::GetJobSystem()->Submit<bool>(job);
    }
}

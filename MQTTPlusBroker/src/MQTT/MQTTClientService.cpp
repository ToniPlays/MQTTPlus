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
            MQP_WARN("{} published {} to {}", client->GetAuth().ClientID, message, topic);
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

        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "*" },
            .Table = "devices",
            .Filters = { { "deviceName", SQLFieldFilterType::Equal, e.GetClient().GetAuth().ClientID } }
        };

        Ref<Job> job = Job::Lambda("Update", [query, e](JobInfo& info) mutable -> Coroutine {
            Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();
            auto deviceName = e.GetClient().GetAuth().ClientID;

            auto result = (co_await db->Run(query))[0];
            if(result->Rows() == 0)
            {
                SQLQuery insert = {
                    .Type = SQLQueryType::Insert,
                    .Fields = { { "publicID" }, { "deviceName" }, { "status" }, { "lastSeen" } },
                    .Values = { { "de_" + StringUtility::Hex16() }, { deviceName }, { e.IsConnected() ? 1 : 0 }, { "NOW()", false } },
                    .Table = "devices",
                };
                co_await db->Run(insert);
            }
            else 
            {
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
}

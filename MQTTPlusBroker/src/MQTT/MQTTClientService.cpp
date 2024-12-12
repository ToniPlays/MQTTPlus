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
        Ref<DatabaseService> db = ServiceManager::GetService<DatabaseService>();
        MQP_ASSERT(db, "Database service nullptr");

        SQLQuery query = {
            .Type = SQLQueryType::Select,
            .Fields = { "*" },
            .Table = "devices",
            .Filters = { { "deviceName", SQLFieldFilterType::Equal, e.GetClient().GetAuth().ClientID } }
        };

        db->Transaction(query, [db, e](const SQLQueryResult& result) mutable {
            if(!result.Results) return;

            auto deviceName = e.GetClient().GetAuth().ClientID;
            
            if(result.Results->rowsCount() == 0)
            {
                SQLQuery query = {
                    .Type = SQLQueryType::Insert,
                    .Fields = { { "publicID" }, { "deviceName" }, { "status" }, { "lastSeen" } },
                    .Values = { { "de_" + StringUtility::Hex16() }, { deviceName }, { e.IsConnected() ? 1 : 0 }, { "NOW()", false } },
                    .Table = "devices",
                };
                
                db->Transaction(query);
            }
            else 
            {
                SQLQuery query = {
                    .Type = SQLQueryType::Update,
                    .Fields = { { "status" }, { "lastSeen" } },
                    .Values = { { e.IsConnected() ? 1 : 0 }, { "NOW()", false } },
                    .Table = "devices",
                    .Filters = { { "deviceName", SQLFieldFilterType::Equal, deviceName }}
                };
                db->Transaction(query);
            }
            
            ServiceManager::OnEvent(e);
        });
    }
}

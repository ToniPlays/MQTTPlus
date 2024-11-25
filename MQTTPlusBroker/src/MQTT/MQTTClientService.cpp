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

        throw std::exception();
        
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
        std::string sql = fmt::format("SELECT * FROM devices WHERE deviceName = '{0}'", e.GetClient().GetAuth().ClientID);

        db->Transaction(sql, [db, e](sql::ResultSet* result) mutable {
            if(!result) return;

            auto deviceName = e.GetClient().GetAuth().ClientID;
            
            if(result->rowsCount() == 0)
            {
                //Insert new
                auto hash = StringUtility::Hex16();
                db->Transaction(fmt::format("INSERT INTO devices (publicID, deviceName, status, lastSeen) VALUES ('{0}', '{1}', {2}, NOW())", hash, deviceName, e.IsConnected() ? 1 : 0));
            }
            else 
            {
                //Update entry
                db->Transaction(fmt::format("UPDATE devices SET status = {1}, lastSeen = NOW() WHERE deviceName = '{0}'", deviceName, e.IsConnected() ? 1 : 0));
            }

            ServiceManager::OnEvent(e);
        });
    }
}

#include "MQTTClientService.h"
#include "Core/Service/ServiceManager.h"
#include "Events.h"
#include "Core/Logger.h"

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
        
        m_Broker->SetOnClientConnected([this](Ref<MQTTClient> client) {
            MQTTClientEvent e(client, true);
            ServiceManager::OnEvent(e);
        });
        
        m_Broker->SetOnClientDisconnected([this](Ref<MQTTClient> client, int code) {
            MQTTClientEvent e(client, false);
            ServiceManager::OnEvent(e);
        });

        m_Broker->Listen();
    }

    void MQTTClientService::Stop() 
    {
        
    }
    void MQTTClientService::OnEvent(Event& e)
    {
        
    }
}

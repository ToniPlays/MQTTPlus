#include "MQTTClientService.h"
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
           
        });
        
        m_Broker->SetOnClientDisconnected([this](Ref<MQTTClient> client, int code) {
        });
        
        m_Thread = Ref<Thread>::Create(std::thread([this]() {
            m_Broker->Listen();
        }));
    }

    void MQTTClientService::Stop() {
        
    }
}

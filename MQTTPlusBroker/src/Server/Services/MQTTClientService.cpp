#include "MQTTClientService.h"

namespace MQTTPlus
{
    MQTTClientService::MQTTClientService(uint32_t port)
    {
        BrokerCreateSettings settings = {
            .Port = port,
            .UseSSL = false,
        };
        
        m_Broker = new Broker(settings);
        std::cout << fmt::format("MQTT broker created") << std::endl;
    }

    void MQTTClientService::Start(ServiceManager* manager) {
        
        m_Broker->SetOnClientConnected([this](Ref<MQTTClient> client) {
            std::cout << "Total clients: " << m_Broker->GetConnectedClientCount() << std::endl;
        });
        
        m_Broker->SetOnClientDisconnected([this](Ref<MQTTClient> client, int code) {
            std::cout << "Remaining clients: " << m_Broker->GetConnectedClientCount() << std::endl;
        });
        
        m_Broker->Listen();
    }

    void MQTTClientService::Stop() {
        
    }
}

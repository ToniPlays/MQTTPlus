#include "MQTTClientService.h"

namespace MQTTPlus
{
    MQTTClientService::MQTTClientService(uint32_t port)
    {
        BrokerCreateSettings settings = {
            .Port = 8883,
            .UseSSL = false,
        };
        
        m_Broker = std::make_unique<Broker>(settings);
    }

    void MQTTClientService::Start(ServiceManager* manager) {
        
        m_Broker->SetOnClientConnected([](Ref<MQTTClient> client) {
            std::cout << "Connected" << std::endl;
        });
        
        m_Broker->SetOnClientDisconnected([](Ref<MQTTClient> client, int code) {
            std::cout << "Disconnected" << std::endl;
        });
        
        m_Broker->Listen();
    }

    void MQTTClientService::Stop() {
        
    }
}

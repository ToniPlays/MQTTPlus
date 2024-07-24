#include "MQTTClientService.h"

namespace MQTTPlus
{
    void MQTTClientService::Start(ServiceManager* manager) {
        BrokerCreateSettings settings = {
            .Port = 8883,
            .UseSSL = false,
        };
        
        m_Broker = std::make_unique<Broker>(settings);
        
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

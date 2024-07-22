#include "MQTTPlusBroker.h"
#include "Core/Broker.h"
#include <iostream>

int main(int argc, char* argv[])
{
    auto decoder = MQTTPlus::MQTTMessageDecoder(0);
    
    const auto settings = MQTTPlus::BrokerCreateSettings {
        .Port = 8886,
        .UseSSL = false,
    };
    
    MQTTPlus::Broker broker(settings);
    broker.SetOnClientConnected([broker](Ref<MQTTPlus::Client> client) mutable {
        std::cout << std::format("Client {} connected", client->GetAuth().ClientID) << std::endl;
        
        std::vector<Ref<MQTTPlus::Client>> clients = std::vector<Ref<MQTTPlus::Client>>();
        clients.push_back(client);
    });
    
    broker.OnPublish("/test", [](Ref<MQTTPlus::Client> client, Buffer payload) {
        std::cout << "To data to /test" << std::endl;
    });
    
    
    broker.Listen();
    
    return 0;
}

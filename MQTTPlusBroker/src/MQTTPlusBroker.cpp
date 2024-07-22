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
    broker.Listen();
    
    return 0;
}

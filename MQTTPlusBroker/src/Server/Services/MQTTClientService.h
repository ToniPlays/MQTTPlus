#pragma once

#include "Service.h"
#include "MQTT/Broker.h"
#include <iostream>

namespace MQTTPlus
{
    class MQTTClientService : public Service {
    public:
        MQTTClientService(uint32_t port);
        ~MQTTClientService() = default;
        
        const Broker& GetBroker() { return *m_Broker; }

        void Start(ServiceManager* manager) override;
        void Stop() override;

        
        std::string GetName() const override { return "MQTTClientService"; };
        
    private:
        Broker* m_Broker;
    };
}

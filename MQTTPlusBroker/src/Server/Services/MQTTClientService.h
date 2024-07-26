#pragma once

#include "Service.h"
#include "MQTT/Broker.h"
#include "Core/Threading/Thread.h"
#include <iostream>

namespace MQTTPlus
{
    class MQTTClientService : public Service {
    public:
        MQTTClientService(uint32_t port);
        ~MQTTClientService() = default;
        
        Broker& GetBroker() { return *m_Broker; }

        void Start() override;
        void Stop() override;

        void Check();
        
        std::string GetName() const override { return "MQTTClientService"; };
        
    private:
        Broker* m_Broker;
        Ref<Thread> m_Thread;
    };
}

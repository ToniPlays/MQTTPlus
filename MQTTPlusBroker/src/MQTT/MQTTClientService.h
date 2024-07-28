#pragma once

#include "Core/Service.h"
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
        bool IsRunning() const override { return m_Thread; };

        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
        
        std::string GetName() const override { return "MQTTClientService"; };
        
    private:
        Broker* m_Broker;
        Ref<Thread> m_Thread;
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
    };
}
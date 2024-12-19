#pragma once

#include "Core/Service/Service.h"
#include "MQTT/Broker.h"
#include "Core/Threading/JobSystem.h"
#include "Database/SQLQuery.h"
#include "Events.h"
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
        void OnEvent(Event& e) override;

        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
        
        std::string GetName() const override { return "MQTTClientService"; };
        
    private:
        void OnMQTTClientEvent(MQTTClientEvent& e);
        void HandleValuePublish(Ref<MQTTClient> client, const std::string& topic, const std::string& message);

        static Promise<Ref<SQLQueryResult>> UpdateTopicField(const std::string& fieldId, const std::string& value, const std::string& formatter);
    private:
        Broker* m_Broker;
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
    };
}

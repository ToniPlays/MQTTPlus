#pragma once

#include "Core/Service/Service.h"
#include "Server/HTTPServer.h"
#include "Core/Event/EventListener.h"

#include <iostream>

namespace MQTTPlus
{
    class FrontendService : public Service {
    public:
        FrontendService(uint32_t port);
        ~FrontendService() = default;

        void Start() override;
        void Stop() override;
        void OnEvent(Event& e) override;

        void SetEventListener(Ref<HTTPClient> client, const std::string& type);
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
        
        std::string GetName() const override { return "FrontendService"; }

        private:
        static void ProcessMQTTChangeEvent(Ref<HTTPClient> client, Event& e);

    private:
        HTTPServer* m_Server;
        
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
        std::unordered_map<HTTPClient*, EventListener> m_Listeners;
    };
}

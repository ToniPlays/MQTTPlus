#pragma once

#include "Core/Service.h"
#include "Server/HTTPServer.h"
#include "Core/EventListener.h"
#include "Core/Threading/Thread.h"

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
        bool IsRunning() const override { return m_Thread; };

        void SetEventListener(Ref<HTTPClient> client, const std::string& type);
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
        
        std::string GetName() const override { return "FrontendService"; }

        private:
        static void ProcessMQTTChangeEvent(Ref<HTTPClient> client, Event& e);

    private:
        HTTPServer* m_Server;
        Ref<Thread> m_Thread;
        
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
        std::unordered_map<HTTPClient*, EventListener> m_Listeners;
    };
}

#pragma once

#include "Core/Service.h"
#include "Server/HTTPServer.h"
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
        bool IsRunning() const override { return m_Thread; };
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
        
        std::string GetName() const override { return "FrontendService"; }
    private:
        HTTPServer* m_Server;
        Ref<Thread> m_Thread;
        
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
    };
}

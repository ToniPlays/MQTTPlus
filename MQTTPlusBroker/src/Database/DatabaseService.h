#pragma once

#include "Core/Service.h"
#include "Core/Threading/Thread.h"

#include <iostream>

namespace MQTTPlus
{
    class DatabaseService : public Service {
    public:
        DatabaseService();
        ~DatabaseService() = default;

        void Start() override;
        void Stop() override;
        void OnEvent(Event& e) override {}
        bool IsRunning() const override { return m_Thread; };
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
            
        std::string GetName() const override { return "DatabaseService"; }
    private:
        Ref<Thread> m_Thread;
    
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
    };
}

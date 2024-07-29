#pragma once

#include "Core/Service.h"
#include "Core/Threading/Thread.h"

#include <iostream>
#include <mariadb/conncpp.hpp>

namespace MQTTPlus
{
    class DatabaseService : public Service {
    public:
        DatabaseService();
        ~DatabaseService() = default;

        void Start() override;
        void Stop() override;
        void OnEvent(Event& e) override {}
        bool IsRunning() const override { return m_Connection != nullptr; };
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
            
        std::string GetName() const override { return "DatabaseService"; }
    private:
        Ref<Thread> m_Thread;
        sql::Driver* m_Driver;
        std::unique_ptr<sql::Connection> m_Connection;
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
    };
}

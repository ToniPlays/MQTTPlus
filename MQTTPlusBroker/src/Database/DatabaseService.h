#pragma once

#include "Core/Service/Service.h"

#include <queue>
#include <iostream>
#include <mariadb/conncpp.hpp>

namespace MQTTPlus
{
    struct DatabaseTransaction
    {
        std::string SQL;
        std::function<void(sql::ResultSet*)> Callback;
    };

    class DatabaseService : public Service {
    public:
        DatabaseService();
        ~DatabaseService() = default;

        void Start() override;
        void Stop() override;
        void OnEvent(Event& e) override {}
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
            
        std::string GetName() const override { return "DatabaseService"; }

        void Transaction(const std::string& sql, const std::function<void(sql::ResultSet*)> callback = nullptr);

    private:
        void Reconnect();
        void ValidateSchema();
        bool RunTransaction(const DatabaseTransaction& transaction);

    private:
        sql::Driver* m_Driver;
        std::unique_ptr<sql::Connection> m_Connection;
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
        std::recursive_mutex m_TransactionMutex;
        std::queue<DatabaseTransaction> m_Transactions;
        std::atomic_uint32_t m_TransactionCount = 0;
    };
}

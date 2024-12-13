#pragma once

#include "Core/Service/Service.h"
#include "SQLQuery.h"
#include "Core/Threading/JobGraph.h"

#include <queue>
#include <iostream>
#include <mariadb/conncpp.hpp>

namespace MQTTPlus
{
    struct DatabaseTransaction
    {
        std::string SQL;
        SQLQuery Query;
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

        Promise<Ref<SQLQueryResult>> Run(const SQLQuery& query);
        Promise<Ref<SQLQueryResult>> Run(const std::string& sql);

    private:
        void Reconnect();
        void ValidateSchema();
        static Coroutine RunTransaction(JobInfo& info, DatabaseService* service, DatabaseTransaction& transaction);

    private:
        sql::Driver* m_Driver;
        std::unique_ptr<sql::Connection> m_Connection;
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
        std::recursive_mutex m_TransactionMutex;

        JobSystem m_System = JobSystem(1);
    };
}

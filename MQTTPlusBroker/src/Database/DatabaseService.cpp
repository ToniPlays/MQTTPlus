#include "DatabaseService.h"
#include "Core/Logger.h"
#include "Core/CommandLineArgs.h"

#include "SQLQueryBuilder.h"

namespace MQTTPlus {

    DatabaseService::DatabaseService()
    {
        m_Driver = sql::mariadb::get_driver_instance();
    }

    void DatabaseService::Start() 
    {
        m_StartupTime = std::chrono::system_clock::now();
        m_TransactionMutex.lock();
        Reconnect();
        ValidateSchema();

        m_System.WaitForJobsToFinish();
        m_TransactionMutex.unlock();

        while(m_Thread->GetStatus() != ThreadStatus::Terminated)
            m_Thread->Wait(m_Thread->GetStatus());
    }

    void DatabaseService::Stop() 
    {
        m_System.Terminate();
    }

    Promise<Ref<SQLQueryResult>> DatabaseService::Run(const SQLQuery& query)
    {
        m_TransactionMutex.lock();
        SQLQueryBuilder builder(query);
        std::string sql = builder.CreateQuery();

        DatabaseTransaction trx = { sql, query };
        Ref<Job> runJob = Job::Create("Query", RunTransaction, this, trx);

        JobGraphInfo info = {
            .Name = "Transaction",
            .Stages = { { "Run", 1.0f, { runJob } } },
        };
        m_TransactionMutex.unlock();

        auto promise = m_System.Submit<Ref<SQLQueryResult>>(Ref<JobGraph>::Create(info));
        promise.ContinueWith([sql](const auto&) {
            MQP_TRACE("Finished {}", sql);
        });
        return promise;
    }

    Promise<Ref<SQLQueryResult>> DatabaseService::Run(const std::string& sql)
    {
        m_TransactionMutex.lock();
        DatabaseTransaction trx = { sql };
        Ref<Job> runJob = Job::Create("Query", RunTransaction, this, trx);

        JobGraphInfo info = {
            .Name = "Transaction",
            .Stages = { { "RunSQL", 1.0f, { runJob } } },
        };
        
        m_TransactionMutex.unlock();

        return m_System.Submit<Ref<SQLQueryResult>>(Ref<JobGraph>::Create(info));
    }

    void DatabaseService::Reconnect() 
    {
        if(m_Connection)
            if(!m_Connection->isClosed()) 
                return;

        sql::SQLString url("localhost");

        std::string user = CommandLineArgs::Get<std::string>("db_user");
        std::string password = CommandLineArgs::Get<std::string>("db_pass");

        m_Connection = std::unique_ptr<sql::Connection>(m_Driver->connect(url, user, password));
        if(!m_Connection)
        {
            MQP_FATAL("Could not connect to MariaDB server");
            return;
        }
        
        MQP_INFO("Connection established to MariaDB {}", m_Connection->isValid() ? "Valid" : "Not valid");
    }

    void DatabaseService::ValidateSchema()
    {
        std::string file = File::ReadFile("res/database/schema.txt");
        const char* type = "#sql";
		size_t endPos = 0;

		while (endPos != std::string::npos)
		{
            std::string f = StringUtility::GetPreprocessor(type, file, endPos, &endPos);
            if (endPos == std::string::npos) continue;

            uint64_t nextTokenPos = file.find(type, endPos);
            std::string src = nextTokenPos == std::string::npos ? file.substr(endPos) : file.substr(endPos, nextTokenPos - endPos);
            Run(f + src);
        }
        
        MQP_INFO("Schema validated from res/database/schema.txt");
    }

    Coroutine DatabaseService::RunTransaction(JobInfo& info, DatabaseService* service, DatabaseTransaction& transaction)
    {
        try 
        {
            service->Reconnect();
            //MQP_TRACE(transaction.SQL);
            sql::PreparedStatement* stmt = service->m_Connection->prepareStatement(transaction.SQL);
            
            auto result = stmt->executeQuery();
            Ref<SQLQueryResult> queryResult = Ref<SQLQueryResult>::Create(stmt, transaction.Query, result);
            info.Result(queryResult);
            co_return;
            
        } catch(sql::SQLSyntaxErrorException e)
        {
            throw JobException(fmt::format("Failed running transaction: {0}\n{1}", transaction.SQL, e.what()));
        } catch(sql::SQLException e)
        {
            throw JobException(fmt::format("Database error {}\nQuery {}", e.getMessage().c_str(), transaction.SQL));
        } catch(std::exception e)
        {
            throw JobException(fmt::format("Database error: std::exception\nQuery {}", transaction.SQL));
        }
        throw JobException("Failed running SQL query");
    }
}

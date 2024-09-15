#include "DatabaseService.h"
#include "Core/Logger.h"
#include "Core/CommandLineArgs.h"

namespace MQTTPlus {

    DatabaseService::DatabaseService()
    {
        m_Driver = sql::mariadb::get_driver_instance();
    }

    void DatabaseService::Start() 
    {
        m_StartupTime = std::chrono::system_clock::now();

        Reconnect();
        ValidateSchema();

        while(true)
        {
            m_TransactionCount.wait(0);
            m_TransactionMutex.lock();
            DatabaseTransaction transaction = m_Transactions.front();

            m_Transactions.pop();
            m_TransactionCount = m_Transactions.size();

            RunTransaction(transaction);
            m_TransactionMutex.unlock();
        }
    }

    void DatabaseService::Stop() 
    {

    }

    void DatabaseService::Transaction(const std::string& sql, const std::function<void(sql::ResultSet*)> callback)
    {
        m_TransactionMutex.lock();
        m_Transactions.push({ sql, callback });
        m_TransactionCount = m_Transactions.size();
        m_TransactionMutex.unlock();
        m_TransactionCount.notify_all();
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

            RunTransaction({ f + src, nullptr });
        }
        MQP_INFO("Schema validated from res/database/schema.txt");
    }

    bool DatabaseService::RunTransaction(const DatabaseTransaction& transaction)
    {
        
        try 
        {
            Reconnect();
            //MQP_TRACE(transaction.SQL);
            std::unique_ptr<sql::PreparedStatement> stmt(m_Connection->prepareStatement(transaction.SQL));
            if(transaction.Callback)
            {
                auto result = std::unique_ptr<sql::ResultSet>(stmt->executeQuery());
                transaction.Callback(result.get());
            }
            else
                stmt->executeUpdate();
                
            return true;
            
        } catch(sql::SQLSyntaxErrorException e)
        {
            MQP_ERROR("Failed running transaction: {0}\n{1}", transaction.SQL, e.what());
        } catch(sql::SQLException e)
        {
            MQP_ERROR("Database error {}\nQuery {}", e.getMessage().c_str(), transaction.SQL);
        } catch(std::exception e)
        {
            MQP_ERROR("Database error: std::exception\nQuery {}", transaction.SQL);
        }
        if(transaction.Callback)
                transaction.Callback(nullptr);
            return false;
    }
}

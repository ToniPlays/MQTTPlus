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
        ValidateSchema();

        while(true)
        {
            m_TransactionCount.wait(0);
            auto& transaction = m_Transactions[0];
            RunTransaction(transaction);     
        }
    }

    void DatabaseService::Stop() 
    {

    }

    void DatabaseService::ValidateSchema()
    {
        std::string file = File::ReadFile("res/database/schema.txt");
        const char* type = "#sql";
		size_t endPos = 0;

		while (endPos != std::string::npos)
		{
            std::string sql = StringUtility::GetPreprocessor(type, file, endPos, &endPos);
            if (endPos == std::string::npos) continue;

            size_t pos = StringUtility::OffsetOf(file, type, endPos + 1);
            std::string src = sql + file.substr(endPos, pos != std::string::npos ? pos : file.length() - endPos);
            RunTransaction({ src, nullptr });
        }
    }

    bool DatabaseService::RunTransaction(const DatabaseTransaction& transaction)
    {
        MQP_TRACE(transaction.SQL);
        try 
        {
            std::unique_ptr<sql::PreparedStatement> stmt(m_Connection->prepareStatement(transaction.SQL));
            int32_t result = stmt->executeUpdate();

            if(transaction.Callback)
                transaction.Callback(nullptr);
            return true;
            
        } catch(sql::SQLSyntaxErrorException e)
        {
            MQP_ERROR("Failed running transaction: {0}\n{1}", transaction.SQL, e.what());
            if(transaction.Callback)
                transaction.Callback(nullptr);
            return false;
        }
    }
}

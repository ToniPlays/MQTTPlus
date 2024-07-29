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
    }

    void DatabaseService::Stop() 
    {

    }
}

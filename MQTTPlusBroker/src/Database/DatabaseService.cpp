#include "DatabaseService.h"
#include "Core/Logger.h"

namespace MQTTPlus {

    DatabaseService::DatabaseService()
    {
        MQP_INFO("Database connection init");
    }

    void DatabaseService::Start() 
    {
        m_StartupTime = std::chrono::system_clock::now();
    }

    void DatabaseService::Stop() 
    {

    }
}

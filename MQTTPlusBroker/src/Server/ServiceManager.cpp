#include "ServiceManager.h"
#include "Core/Logger.h"
#include <spdlog/fmt/fmt.h>

namespace MQTTPlus 
{
    void ServiceManager::Start()
    {
        m_Running = true;
        for(auto& service : m_Services)
        {
            MQP_INFO("Starting service: {}", service->GetName());
            service->Start(this);
        }
        MQP_INFO("All services started");
    }
    void ServiceManager::Stop()
    {
        m_Running = false;
        m_Running.notify_all();
        m_Services.clear();
    }
}

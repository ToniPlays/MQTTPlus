#include "ServiceManager.h"
#include "Core/Logger.h"
#include <spdlog/fmt/fmt.h>

namespace MQTTPlus 
{
    void ServiceManager::Start()
    {
        s_Running = true;
        for(auto& service : s_Services)
        {
            MQP_INFO("Starting service: {}", service->GetName());
            service->Start();
        }
        MQP_INFO("All services started");
    }
    void ServiceManager::Stop()
    {
        s_Running = false;
        s_Running.notify_all();
        s_Services.clear();
    }
}

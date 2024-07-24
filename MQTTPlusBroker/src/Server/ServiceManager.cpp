#include "ServiceManager.h"
#include <spdlog/fmt/fmt.h>

namespace MQTTPlus 
{
    void ServiceManager::Start()
    {
        m_Running = true;
        for(auto& service : m_Services)
        {
            std::cout << fmt::format("Starting service: {}", service->GetName()) << std::endl;
            service->Start(this);
        }
    }
    void ServiceManager::Stop()
    {
        m_Running.wait(true);
        m_Running = false;
    }
}

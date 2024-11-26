#include "ServiceManager.h"
#include "Core/Logger.h"
#include "Core/Timer.h"
#include <spdlog/fmt/fmt.h>

#include "Core/RaspberryPi/LinuxSystemStat.h"
#include "Core/MacOS/MacOSSystemStat.h"

namespace MQTTPlus 
{
    void ServiceManager::Start()
    {
        s_Status.TotalMemory = GetAvailableSystemMemory();
        UpdateSystemStatus();
        s_StartupTime = std::chrono::system_clock::now();
        s_JobSystem = new JobSystem();
    
        for(auto& service : s_Services) {
            s_JobSystem->Queue([&service](Ref<Thread> thread) mutable { 
                try {
                    MQP_INFO("Starting service: {} on {}", service->GetName(), thread->GetDebugName());
                    service->RunService(thread);
                    MQP_WARN("Stopped service: {} on {}", service->GetName(), thread->GetDebugName());
                } catch(std::exception e)
                {
                    MQP_ERROR("Service {} crashed", service->GetName());
                }
            });
        }
        
        MQP_INFO("All services started");
        s_Running = true;
    }
    
    void ServiceManager::Stop()
    {
        s_Running.wait(true);
        s_Running = false;
        s_Running.notify_all();

        MQP_WARN("Closing");
        s_Services.clear();
    }

    void ServiceManager::OnEvent(Event& e)
    {
        for(auto& service : s_Services)
            service->OnEvent(e);
    }

    const SystemStatus& ServiceManager::GetSystemStatus() 
    {
        static Timer timer;
        if(timer.ElapsedMillis() > 500)
        {
            UpdateSystemStatus();
            timer.Reset();
        }
        
        return s_Status;
    }
    void ServiceManager::UpdateSystemStatus()
    {
        auto diskUsage = GetSystemDiskUsage();
        s_Status.UpdatedAt = std::chrono::system_clock::now();
        s_Status.UsageCPU = GetCPULoad();
        s_Status.AvailableMemory = GetSystemMemoryAvailable();
            
        s_Status.DiskTotalSpace = diskUsage[0];
        s_Status.DiskSpaceUsed = diskUsage[1];
    }
}

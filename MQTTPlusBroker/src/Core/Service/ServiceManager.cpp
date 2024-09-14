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
        s_StartupTime = std::chrono::system_clock::now();
        s_JobSystem = new JobSystem();
    
        for(auto& service : s_Services) {
            s_JobSystem->Queue([&service](Ref<Thread> thread) mutable { 
                MQP_INFO("Starting service: {} on {}", service->GetName(), thread->GetDebugName());
                service->RunService(thread);
                MQP_INFO("Stopped service: {} on {}", service->GetName(), thread->GetDebugName());
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
            auto diskUsage = GetSystemDiskUsage();
            s_Status.UpdatedAt = std::chrono::system_clock::now();
            s_Status.UsageCPU = GetCPULoad();
            s_Status.AvailableMemory = GetSystemMemoryAvailable();
            
            s_Status.DiskTotalSpace = diskUsage[0];
            s_Status.DiskSpaceUsed = diskUsage[1];
            timer.Reset();
        }
        
        return s_Status;
    }
}

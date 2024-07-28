#include "ServiceManager.h"
#include "Core/Logger.h"
#include "Timer.h"
#include <spdlog/fmt/fmt.h>

#include "LinuxSystemStat.h"
#include "MacOSSystemStat.h"

namespace MQTTPlus 
{
    void ServiceManager::Start()
    {
        s_StartupTime = std::chrono::system_clock::now();
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

    const SystemStatus& ServiceManager::GetSystemStatus() {
        static Timer timer;
        if(timer.ElapsedMillis() > 500)
        {
            auto diskUsage = GetSystemDiskUsage();
            s_Status.UpdatedAt = std::chrono::system_clock::now();
            s_Status.UsageCPU = GetCPULoad();
            s_Status.MemoryUsage = GetSystemMemoryUsagePercentage();
            s_Status.DiskTotalSpace = diskUsage[0];
            s_Status.DiskSpaceUsed = diskUsage[1];
            timer.Reset();
        }
        
        return s_Status;
    }
}

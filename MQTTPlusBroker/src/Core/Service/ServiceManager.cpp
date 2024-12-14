#include "ServiceManager.h"
#include "Core/Logger.h"
#include "Core/Timer.h"
#include "Core/MQTTPlusException.h"
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
        s_JobSystem = new JobSystem(4);

        s_JobSystem->Hook([](Ref<Thread> thread, ThreadStatus status) {
            if(status == ThreadStatus::Executing)
                MQP_TRACE("Thread {} running {}", thread->GetThreadID(), thread->GetCurrentJob()->GetName());
        });

        for(auto& service : s_Services) {
            auto job = Job::Lambda(service->GetName(), [service](JobInfo& info) mutable -> Coroutine {
                try {
                    MQP_INFO("Starting service: {} on {}", service->GetName(), info.Thread->GetThreadID());
                    service->RunService(info.Thread);
                    MQP_WARN("Stopped service: {} on {}", service->GetName(), info.Thread->GetThreadID());
                } catch(MQTTPlusException e)
                {
                    MQP_ERROR("Service {} crashed with {}", service->GetName(), e.what());
                }
                co_return;
            });

            JobGraphInfo info = {
                .Name = "Service",
                .Stages = { { "Run", 1.0f, { job }}}
            };
            s_JobSystem->Submit<bool>(Ref<JobGraph>::Create(info));
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

#pragma once

#include "Service.h"
#include "Core/Event/Event.h"
#include "Core/Threading/JobSystem.h"
#include <vector>
#include <chrono>
#include <atomic>
#include <queue>

namespace MQTTPlus {

    struct SystemStatus 
    {
        std::chrono::time_point<std::chrono::system_clock> UpdatedAt;
        float UsageCPU;
        uint64_t TotalMemory;
        uint64_t AvailableMemory;
        uint64_t DiskTotalSpace;
        uint64_t DiskSpaceUsed;
        uint64_t JobCount;
    };

    class ServiceManager {
    public:
        ServiceManager() = default;
        ~ServiceManager() = default;
        
        static void Start();
        static void Stop();
        static void OnEvent(Event& e);
        
        template<typename T, typename... Args>
        static Ref<T> AddService(Args&&... args)
        {
            Ref<T> service = Ref<T>::Create(args...);
            s_Services.push_back(service);
            return service;
        }
        
        template<typename T>
        static Ref<T> GetService()
        {
            for(auto& service : s_Services)
            {
                if(dynamic_cast<T*>(service.Raw()))
                    return service.As<T>();
            }
            return nullptr;
        }
        
        static uint32_t GetRunningServiceCount() {
            uint32_t count = 0;
            for(auto& service : s_Services)
                count += service->IsRunning();
            return count;
        }
        
        static const SystemStatus& GetSystemStatus();
        static JobSystem* GetJobSystem() { return s_JobSystem; }
        
        static std::vector<Ref<Service>> GetServices() 
        {
            return s_Services;
        }
        static const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() { return s_StartupTime; }
        
    private:
        static void UpdateSystemStatus();

        static void ThreadFunc(Ref<Thread> thread);

    private:
        inline static std::vector<Ref<Service>> s_Services;
        inline static std::chrono::time_point<std::chrono::system_clock> s_StartupTime;
        inline static SystemStatus s_Status;
        inline static JobSystem* s_JobSystem;
        inline static std::atomic_bool s_Running = false;
    };
}

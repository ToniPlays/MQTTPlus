#pragma once

#include "Service.h"
#include <vector>
#include <atomic>

namespace MQTTPlus {
    class ServiceManager {
    public:
        ServiceManager() = default;
        ~ServiceManager() = default;
        
        static void Start();
        static void Stop();
        
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
            return (uint32_t)s_Services.size();
        }
        static std::vector<Ref<Service>> GetServices() { return s_Services; }
        
    private:
        inline static std::vector<Ref<Service>> s_Services;
        inline static std::atomic_bool s_Running = false;
    };
}

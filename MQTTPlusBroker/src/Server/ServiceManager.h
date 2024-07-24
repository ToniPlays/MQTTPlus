#pragma once

#include "Services/Service.h"
#include <vector>
#include <atomic>

namespace MQTTPlus {
    class ServiceManager {
    public:
        ServiceManager() = default;
        ~ServiceManager() = default;
        
        void Start();
        void Stop();
        
        template<typename T, typename... Args>
        void AddService(Args&&... args)
        {
            m_Services.push_back(Ref<T>::Create(args...));
        }
        
        template<typename T>
        Ref<T> GetService()
        {
            for(auto& service : m_Services)
            {
                if(dynamic_cast<T*>(service.Raw()))
                    return service.As<T>();
            }
            return nullptr;
        }
        
    private:
        std::vector<Ref<Service>> m_Services;
        std::atomic_bool m_Running = false;
    };
}

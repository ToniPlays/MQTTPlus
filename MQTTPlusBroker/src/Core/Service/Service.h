#pragma once

#include "Ref.h"
#include "Core/Event/Event.h"
#include "Core/Threading/JobSystem.h"
#include <chrono>

namespace MQTTPlus {
    class ServiceManager;

    class Service : public RefCount
    {
    public:
        virtual void RunService(Ref<Thread> thread)
        {
            m_Thread = thread;
            Start();
        };
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void OnEvent(Event& e) = 0;
        
        virtual bool IsRunning() const { return m_Thread->GetStatus() == ThreadStatus::Executing; };
        
        virtual const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const = 0;
        
        virtual std::string GetName() const = 0;
    private:
        Ref<Thread> m_Thread;
    };
}

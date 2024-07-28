#pragma once

#include "Ref.h"
#include <chrono>

namespace MQTTPlus {

    class Service : public RefCount
    {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;
        
        virtual bool IsRunning() const = 0;
        
        virtual const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const = 0;
        
        virtual std::string GetName() const = 0;
    };
}

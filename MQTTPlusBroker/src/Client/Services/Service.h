#pragma once

#include "Ref.h"

namespace MQTTPlus {

class ServiceManager;

    class Service : public RefCount
    {
    public:
        virtual void Start(ServiceManager* manager) = 0;
        virtual void Stop() = 0;
        
        virtual std::string GetName() const = 0;
    };
}

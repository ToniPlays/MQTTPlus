#pragma once

#include "Ref.h"

namespace MQTTPlus {

    class Service : public RefCount
    {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;
        
        virtual std::string GetName() const = 0;
    };
}

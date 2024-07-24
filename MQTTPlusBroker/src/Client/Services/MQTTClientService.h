#pragma once

#include "Service.h"
#include "Core/Broker.h"
#include <iostream>

namespace MQTTPlus
{
    class MQTTClientService : public Service {
    public:
        MQTTClientService() = default;
        ~MQTTClientService() = default;
        
        void Start(ServiceManager* manager);
        void Stop();
        
        std::string GetName() const { return "MQTTClientService"; };
        
    private:
        std::unique_ptr<Broker> m_Broker;
    };
}

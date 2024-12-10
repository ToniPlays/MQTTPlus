#pragma once

#include "Core/Service/Service.h"
#include "Server/HTTPServer.h"
#include "Core/Event/EventListener.h"

#include <iostream>

namespace MQTTPlus
{
    class FrontendService : public MQTTPlus::Service 
    {
    public:
        FrontendService(uint32_t port);
        ~FrontendService() = default;

        void Start() override;
        void Stop() override;
        void OnEvent(Event& e) override;
        
        const std::chrono::time_point<std::chrono::system_clock>& GetStartupTime() const override { return m_StartupTime; };
        
        std::string GetName() const override { return "FrontendService"; }

    private:
    
    private:
        HTTPServer* m_Server;
        std::chrono::time_point<std::chrono::system_clock> m_StartupTime;
    };
}

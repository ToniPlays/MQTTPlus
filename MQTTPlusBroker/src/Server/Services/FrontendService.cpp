#include "FrontendService.h"
#include "Server/Endpoints.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus {

    FrontendService::FrontendService(uint32_t port)
    {
        m_Server = std::make_unique<HTTPServer>(port);
    }

    void FrontendService::Start(ServiceManager* manager) 
    {
        
        m_Server->SetMessageResolver([](const char* endpoint, const std::string& message) {
            try {
                auto json = nlohmann::json::parse(message);
                std::string requestEndpoint = json["endpoint"];
                if(endpoint == requestEndpoint)
                    return true;

                return false;
            } catch(std::exception& e)
            {
                return false;
            }
        });
        InitializeApiEndpoints(*m_Server.get());
        m_Server->Listen();
    }

    void FrontendService::Stop() 
    {

    }
}
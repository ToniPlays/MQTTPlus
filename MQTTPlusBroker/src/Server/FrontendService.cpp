#include "FrontendService.h"
#include "Endpoints.h"
#include "Events.h"
#include "Core/Service/ServiceManager.h"
#include "MQTT/Events.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus {

    FrontendService::FrontendService(uint32_t port)
    {
        m_Server = new HTTPServer(port);
    }

    void FrontendService::Start() 
    {
        m_StartupTime = std::chrono::system_clock::now();
        
        m_Server->SetOnClientChanged([](Ref<HTTPClient> client, bool connected) {
            HTTPClientEvent e(client, connected);
            ServiceManager::OnEvent(e);
        });
        m_Server->SetUserData((void*)nullptr);
        m_Server->SetMessageResolver([](const char* endpoint, const std::string& message) {
            try 
            {
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

        InitializeApiEndpoints(*m_Server);
        m_Server->Listen(); 
    }

    void FrontendService::Stop() 
    {

    }

    void FrontendService::OnEvent(Event& e)
    {
        for(auto& [con, client] : m_Server->GetClients())
        {
            client->OnEvent(e);
        }
    }
}

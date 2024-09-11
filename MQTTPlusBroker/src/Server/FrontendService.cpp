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

        InitializeApiEndpoints(*m_Server);
        m_Server->Listen(); 
    }

    void FrontendService::Stop() 
    {

    }

    void FrontendService::OnEvent(Event& e)
    {
        if(Event::Is<HTTPClientEvent>(e))
        {
            auto& ev = (HTTPClientEvent&)e;
            if(!ev.IsConnected())
            {
                auto it = m_Listeners.find(ev.GetClient().Raw());
                if(it != m_Listeners.end())
                    m_Listeners.erase(it);
            }
            return;
        }

        for(auto& [key, listener] : m_Listeners)
        {
            listener.Invoke(e);
        }
    }
    void FrontendService::SetEventListener(Ref<HTTPClient> client, const std::string& type)
    {
        std::unordered_map<std::string, ListenCallback> listeners = { 
            { MQTTClientEvent::GetStaticName(), [client](Event& e) { ProcessMQTTChangeEvent(client, e); } }
            };

        m_Listeners[client.Raw()].AddListener(type, listeners[type]);
    }


    void FrontendService::ProcessMQTTChangeEvent(Ref<HTTPClient> client, Event& e)
    {
        using namespace nlohmann;

        MQTTClientEvent& ev = (MQTTClientEvent&)e;

        json j;
        j["endpoint"] = "/events";
        j["data"] = json({
            { "type", "mqtt.client_connection_status_changed" },
            { "event_data", {
                { "client_id", ev.GetClient().GetAuth().ClientID },
                { "is_connected", ev.IsConnected() }
            } }
        });
        
        client->Send(j.dump());
    }
}

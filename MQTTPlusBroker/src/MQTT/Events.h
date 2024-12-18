#pragma once

#include "Core/Event/Event.h"
#include "MQTTClient.h"
#include "API/JsonConverter.h"
#include "nlohmann/json.hpp"

namespace MQTTPlus
{
    class MQTTClientEvent : public Event 
    {
    public:
        MQTTClientEvent(Ref<MQTTClient> client, bool connected) : m_Client(client), m_Connected(connected), m_LastSeen(std::chrono::system_clock::now()) {}
        ~MQTTClientEvent() = default;

        virtual std::string ToString() const override {
            return fmt::format("Client: {} {}", m_Client->GetAuth().ClientID, (m_Connected ? "Connected" : "Disconnected"));
        }
        
        MQTTClient& GetClient() { return *m_Client.Raw(); }
        const MQTTClient& GetClient() const { return *m_Client.Raw(); }
        bool IsConnected() const { return m_Connected; }

        const std::chrono::time_point<std::chrono::system_clock>& LastSeen() const { return m_LastSeen; } 

        EVENT_CLASS_TYPE(MQTTClientEvent);

    private:
        Ref<MQTTClient> m_Client;
        std::chrono::time_point<std::chrono::system_clock> m_LastSeen;
        bool m_Connected;
    };


    static void to_json(nlohmann::json& j, const MQTTClientEvent& event)
    {
        auto& auth = event.GetClient().GetAuth();
        j = nlohmann::json {
            { "client_id", auth.PublicId, },
            { "name", auth.ClientID, },
            { "status", event.IsConnected() ? 1 : 0 },
            { "last_seen", FormatTime(event.LastSeen()) },
        };
    }

    class MQTTPublishEvent : public Event 
    {
    public:
        MQTTPublishEvent(Ref<MQTTClient> client) : m_Client(client) {}
        ~MQTTPublishEvent() = default;

        virtual std::string ToString() const override {
            return fmt::format("Client published: {}", m_Client->GetAuth().ClientID);
        }
        
        MQTTClient& GetClient() { return *m_Client.Raw(); }
        const MQTTClient& GetClient() const { return *m_Client.Raw(); }

        EVENT_CLASS_TYPE(MQTTPublishEvent);

    private:
        Ref<MQTTClient> m_Client;
    };


    static void to_json(nlohmann::json& j, const MQTTPublishEvent& event)
    {
        auto& auth = event.GetClient().GetAuth();
        j = nlohmann::json {
            { "client_id", event.GetName(), },
        };
    }

}
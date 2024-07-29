#pragma once

#include "Core/Event.h"
#include "MQTTClient.h"

namespace MQTTPlus
{
    class MQTTClientEvent : public Event {
    public:
        MQTTClientEvent(Ref<MQTTClient> client, bool connected) : m_Client(client), m_Connected(connected) {}
        ~MQTTClientEvent() = default;

        virtual std::string ToString() const override {
            return fmt::format("Client: {} {}", m_Client->GetAuth().ClientID, (m_Connected ? "Connected" : "Disconnected"));
        }

        MQTTClient& GetClient() { return *m_Client.Raw(); }
        bool IsConnected() const { return m_Connected; }
        EVENT_CLASS_TYPE(MQTTClientEvent);

    private:
        Ref<MQTTClient> m_Client;
        bool m_Connected;
    };
}
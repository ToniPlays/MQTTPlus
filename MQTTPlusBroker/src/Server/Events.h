#pragma once

#include "Core/Event//Event.h"
#include "HTTPClient.h"

namespace MQTTPlus
{
    class HTTPClientEvent : public Event {
    public:
        HTTPClientEvent(Ref<HTTPClient> client, bool connected) : m_Client(client), m_Connected(connected) {}
        ~HTTPClientEvent() = default;

        
        virtual std::string ToString() const override {
            return fmt::format("HTTPClient");
        }

        Ref<HTTPClient> GetClient() { return m_Client; }
        bool IsConnected() const { return m_Connected; }

        EVENT_CLASS_TYPE(HTTPClientEvent);
    private:
        Ref<HTTPClient> m_Client;
        bool m_Connected;
    };
}
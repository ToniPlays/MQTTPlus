
#include "HTTPClient.h"
#include "HTTPServer.h"
#include "MQTT/Events.h"
#include "API/MQTTPlusApiFields.h"
#include "API/JsonConverter.h"

#include <websocketpp/server.hpp>

namespace MQTTPlus
{
    void HTTPClient::SetTimedFunction(float timeout, const std::function<void(HTTPClient &)> &callback)
    {
        callback(*this);
        m_Timer.AddCallback(timeout, [this, callback]() mutable
                            { callback(*this); });
    }

    void HTTPClient::SubscribeToEvent(const std::string &eventType)
    {
        m_SubscribedEvents.emplace_back(eventType);
    }

    bool HTTPClient::HasSubscribedToChangeOf(const std::string &publicId) const
    {
        return std::find(m_ChangeEventSubscribes.begin(), m_ChangeEventSubscribes.end(), publicId) != m_ChangeEventSubscribes.end();
    }

    void HTTPClient::Send(const std::string &message)
    {
        try
        {
            m_Server->m_Server.send(m_ConnPtr->get_handle(), message, websocketpp::frame::opcode::TEXT);
        }
        catch (std::exception e)
        {
        }
    }

    void HTTPClient::OnEvent(Event &e)
    {
        using namespace API;

        if (Event::Is<MQTTClientEvent>(e))
        {
            if (!Contains<std::string>(m_SubscribedEvents, "mqtt.client_connection_change"))
                return;

            nlohmann::json j = {};
            j["event"] = "mqtt.client_connection_change";
            j["data"] = (MQTTClientEvent &)e;

            Send(j.dump());
            return;
        }
        else if (Event::Is<MQTTPublishEvent>(e))
        {
            if (!HasSubscribedToChangeOf(""))
                return;

            nlohmann::json j = {};
            j["event"] = "mqtt.field_update";
            j["data"] = (MQTTPublishEvent &)e;

            Send(j.dump());
            return;
        }
    }
}

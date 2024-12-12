
#include "HTTPClient.h"
#include "HTTPServer.h"
#include "MQTT/Events.h"
#include "API/MQTTPlusApiFields.h"
#include "API/JsonConverter.h"

#include <websocketpp/server.hpp>

namespace MQTTPlus 
{
    void HTTPClient::SetTimedFunction(float timeout, const std::function<void (HTTPClient&)>& callback)
    {
        callback(*this);
        m_Timer.AddCallback(timeout, [this, callback]() mutable {
            callback(*this);
        });
    }

    void HTTPClient::SubscribeToEvent(const std::string& eventType)
    {
        m_SubscribedEvents.emplace_back(eventType);
    }

    void HTTPClient::Send(const std::string& message)
    {
        try 
        {
            m_Server->m_Server.send(m_ConnPtr->get_handle(), message, websocketpp::frame::opcode::TEXT);
        } catch(std::exception e)
        {
            
        }
    }

    void HTTPClient::OnEvent(Event& e)
    {
         using namespace API;

        if(Event::Is<MQTTClientEvent>(e) && Contains<std::string>(m_SubscribedEvents, "mqtt.client_connection_change"))
        {
            MQTTClientEvent ev = (MQTTClientEvent&)e;
            nlohmann::json j = {};
            j["event"] = "mqtt.client_connection_change";
            j["data"] = ev;

            Send(j.dump());
            return;
        }
    }
}

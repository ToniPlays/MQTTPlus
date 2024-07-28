
#include "HTTPClient.h"
#include "HTTPServer.h"
#include <websocketpp/server.hpp>

namespace MQTTPlus 
{
    void HTTPClient::SetTimedFunction(float timeout, const std::function<void ()>& callback)
    {
        MQP_INFO("Adding new callback with interval of {}", timeout);
        m_Timer.AddCallback(timeout, callback);
    }
    void HTTPClient::Send(const std::string& message)
    {
        m_Server->m_Server.send(m_ConnPtr->get_handle(), message, websocketpp::frame::opcode::TEXT);
    }
}
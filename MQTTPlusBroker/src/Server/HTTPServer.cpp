#include "HTTPServer.h"

#include "Core/Logger.h"

namespace MQTTPlus
{
    HTTPServer::HTTPServer(uint32_t port, void* userData) : m_Port(port), m_UserData(userData)
    {
        m_Server.set_reuse_addr(true);
        m_Server.set_access_channels(websocketpp::log::alevel::none);
        m_Server.set_error_channels(websocketpp::log::alevel::frame_payload);
        
        m_Server.init_asio();
        
        auto messageHandlerFunc = [this](Server* s, websocketpp::connection_hdl hdl, Server::message_ptr msg) {
            MessageHandlerFunc(hdl, msg);
        };

        
        m_Server.set_message_handler(bind(messageHandlerFunc, &m_Server, _1, _2));
    }

    void HTTPServer::Listen()
    {
        MQP_WARN("HTTP Server on port {}", m_Port);
        m_Server.listen(m_Port);
        m_Server.start_accept();
        m_Server.run();
    }

    void HTTPServer::Post(const char* type, const PostMessageCallback&& callback)
    {
        m_PostCallbacks[type] = callback;
        MQP_TRACE("Register POST \"{}\"", type);
    }

    void HTTPServer::SetMessageResolver(const MessageResolverCallback&& callback)
    {
        m_ResolverCallback = callback;
    }

    void HTTPServer::MessageHandlerFunc(websocketpp::connection_hdl hdl, MessagePtr msg)
    {
        std::string payload = msg->get_payload();
        for(auto& [key, func] : m_PostCallbacks)
        {
            if(!m_ResolverCallback(key.c_str(), payload)) continue;
            
            try 
            {
                std::string result = func(payload, m_UserData);
                m_Server.send(hdl, result, msg->get_opcode());
                return;
            } catch(std::exception& e)
            {
                MQP_ERROR("Message endpoint not found {}", payload);
            }
        }
    }
}

#include "HTTPServer.h"

#include "Core/Logger.h"

namespace MQTTPlus
{
    HTTPServer::HTTPServer(uint32_t port, void* userData) : m_Port(port), m_UserData(userData)
    {
        using namespace websocketpp;

        m_ChangeCallback = [](Ref<HTTPClient> client, bool connected) {};

        m_Server.set_reuse_addr(true);
        m_Server.set_access_channels(log::alevel::none);
        m_Server.set_error_channels(log::alevel::frame_payload);
        
        m_Server.init_asio();
        
        auto messageHandlerFunc = [this](Server* s, connection_hdl hdl, Server::message_ptr msg) {
            MessageHandlerFunc(hdl, msg);
        };
        open_handler openHandler = [this](connection_hdl hdl) {
            Server::connection_ptr con = m_Server.get_con_from_hdl(hdl);
            auto client = Ref<HTTPClient>::Create(this, con);
            m_ConnectedClients[con] = client;
            m_ChangeCallback(client, true);
        };
        
        close_handler closeHandler = [this](connection_hdl hdl) {
            Server::connection_ptr con = m_Server.get_con_from_hdl(hdl);
            auto it = m_ConnectedClients.find(con);
            m_ChangeCallback(m_ConnectedClients[con], false);

            if(it != m_ConnectedClients.end())
                m_ConnectedClients.erase(it);

            MQP_INFO("Socket closed");
        };
        
        m_Server.set_message_handler(bind(messageHandlerFunc, &m_Server, _1, _2));
        m_Server.set_open_handler(openHandler);
        m_Server.set_close_handler(closeHandler);
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
        //MQP_TRACE("Register POST \"{}\"", type);
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
            MQP_TRACE("Running endpoint {}", key);
            try 
            {
                auto conn = m_Server.get_con_from_hdl(hdl);
                func(payload, m_ConnectedClients[conn]);  
                return;
            } catch(std::exception& e)
            {
                MQP_ERROR("Message failed for {} {}", e.what(), payload);
            }
        }
        MQP_ERROR("Endpoint not found for message {}", payload);
    }
}

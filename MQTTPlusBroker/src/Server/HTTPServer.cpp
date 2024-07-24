#include "HTTPServer.h"

#include <spdlog/fmt/fmt.h>

namespace MQTTPlus
{
    HTTPServer::HTTPServer(uint32_t port, void* userData) : m_Port(port), m_UserData(userData)
    {
        m_Server.set_reuse_addr(true);
        m_Server.set_access_channels(websocketpp::log::alevel::all);
        m_Server.set_error_channels(websocketpp::log::alevel::frame_payload);
        
        m_Server.init_asio();
        
        auto messageHandlerFunc = [this](Server* s, websocketpp::connection_hdl hdl, Server::message_ptr msg) {
            MessageHandlerFunc(hdl, msg);
        };

        
        m_Server.set_message_handler(bind(messageHandlerFunc, &m_Server, _1, _2));

    }

    void HTTPServer::Listen()
    {
        std::cout << fmt::format("HTTP listening at {}", m_Port) << std::endl;
        m_Server.listen(m_Port);
        m_Server.start_accept();
        m_Server.run();
    }

    void HTTPServer::Post(const char* type, const PostMessageCallback&& callback)
    {
        m_PostCallbacks[type] = callback;
        std::cout << fmt::format("Register POST \"{}\"", type) << std::endl;
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
                std::cout << result << std::endl;
                m_Server.send(hdl, result, msg->get_opcode());
                return;
            } catch(std::exception& e)
            {
                std::cout << "Message failure: " << e.what() << std::endl;
            }
        }
    }
}

#pragma once

#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace MQTTPlus {
    
    using Server = websocketpp::server<websocketpp::config::asio>;
    using MessagePtr = Server::message_ptr;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;
    using PostMessageCallback = std::function<std::string(const std::string&, void*)>;
    using MessageResolverCallback = std::function<bool(const char*, const std::string&)>;

    class HTTPServer
    {
    public:
        HTTPServer() = default;
        HTTPServer(uint32_t port, void* userData = nullptr);
        
        void Listen();
        void Post(const char* type, const PostMessageCallback&& callback);

        void SetUserData(void* userData) { 
            m_UserData = userData; 
        }
        void SetMessageResolver(const MessageResolverCallback&& callback);
        
    private:
        void MessageHandlerFunc(websocketpp::connection_hdl hdl, MessagePtr msg);
        
    private:
        uint32_t m_Port = 0;
        Server m_Server;
        void* m_UserData = nullptr;
        
        MessageResolverCallback m_ResolverCallback;
        std::unordered_map<std::string, PostMessageCallback> m_PostCallbacks;
    };
}

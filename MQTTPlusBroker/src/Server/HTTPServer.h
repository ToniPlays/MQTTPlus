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
    using PostMessageCallback = std::function<std::string(const std::string&)>;
    using MessageResolverCallback = std::function<bool(const char*, const std::string&)>;

    class HTTPServer
    {
    public:
        HTTPServer() = default;
        HTTPServer(uint32_t port);
        
        void Listen();
        void Post(const char* type, const PostMessageCallback&& callback);

        void SetMessageResolver(const MessageResolverCallback&& callback);
        
    private:
        void MessageHandlerFunc(HTTPServer* server, websocketpp::connection_hdl hdl, MessagePtr msg);
        
    private:
        uint32_t m_Port = 0;
        Server m_Server;
        
        MessageResolverCallback m_ResolverCallback;
        std::unordered_map<std::string, PostMessageCallback> m_PostCallbacks;
    };
}

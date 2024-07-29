#pragma once

#include "HTTPClient.h"
#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace MQTTPlus {
    
    using Server = websocketpp::server<websocketpp::config::asio>;
    using MessagePtr = Server::message_ptr;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;
    using ClientChangeCallback = std::function<void(Ref<HTTPClient>, bool)>;
    using PostMessageCallback = std::function<void(const std::string&, Ref<HTTPClient>)>;
    using MessageResolverCallback = std::function<bool(const char*, const std::string&)>;

    class HTTPServer
    {
        friend class HTTPClient;
    public:
        HTTPServer() = default;
        HTTPServer(uint32_t port, void* userData = nullptr);
        
        void Listen();
        void Post(const char* type, const PostMessageCallback&& callback);

        void SetUserData(void* userData) { 
            m_UserData = userData; 
        }
        void SetOnClientChanged(const ClientChangeCallback callback)
        {
            m_ChangeCallback = callback;
        }
        void SetMessageResolver(const MessageResolverCallback&& callback);
        
    private:
        void MessageHandlerFunc(websocketpp::connection_hdl hdl, MessagePtr msg);
        
    private:
        uint32_t m_Port = 0;
        Server m_Server;
        void* m_UserData = nullptr;
        
        ClientChangeCallback m_ChangeCallback;
        MessageResolverCallback m_ResolverCallback;
        std::unordered_map<std::string, PostMessageCallback> m_PostCallbacks;
        
        std::unordered_map<Server::connection_ptr, Ref<HTTPClient>> m_ConnectedClients;
    };
}

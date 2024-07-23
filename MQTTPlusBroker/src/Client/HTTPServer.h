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

    class HTTPServer
    {
    public:
        HTTPServer(uint32_t port);
        
        void Listen();
        
    private:
        static void MessageHandlerFunc(Server* server, websocketpp::connection_hdl hdl, MessagePtr msg);
        
    private:
        uint32_t m_Port = 0;
        Server m_Server;
    };
}

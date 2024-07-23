#include "HTTPServer.h"

#include <spdlog/fmt/fmt.h>

namespace MQTTPlus 
{
    HTTPServer::HTTPServer(uint32_t port) : m_Port(port)
    {
        m_Server.set_reuse_addr(true);
        m_Server.set_access_channels(websocketpp::log::alevel::all);
        m_Server.set_error_channels(websocketpp::log::alevel::frame_payload);
        
        m_Server.init_asio();
        m_Server.set_message_handler(bind(&MessageHandlerFunc, &m_Server, _1, _2));
    
    }

    void HTTPServer::Listen()
    {
        std::cout << fmt::format("HTTP listening at {}", m_Port) << std::endl;
        m_Server.listen(m_Port);
        m_Server.start_accept();
        m_Server.run();
    }

    void  HTTPServer::MessageHandlerFunc(Server* server, websocketpp::connection_hdl hdl, MessagePtr msg)
    {
        
    }
}

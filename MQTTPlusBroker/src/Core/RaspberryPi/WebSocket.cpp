#if MQP_LINUX

#include "WebSocket.h"
#include "MQTTPlusException.h"
#include "spdlog/fmt/fmt.h"

namespace MQTTPlus
{
    WebSocket::WebSocket(uint32_t port, bool ssl) : m_Port(port), m_SSL(ssl)
    {
        std::cout << fmt::format("Created websocket for port {}", m_Port) << std::endl;
    }

    void WebSocket::Listen()
    {
        
    }
    void WebSocket::SetSocketTimeout(void* socket, uint32_t timeout)
    {
        
    }

    void WebSocket::Write(void* socket, std::vector<uint8_t> bytes)
    {
        us_socket_write(m_SSL, (us_socket_t*)socket, (char*)bytes.data(), (uint32_t)bytes.size(), 0);
    }
}

#endif

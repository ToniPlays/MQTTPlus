#include "WebSocket.h"

#include "Core/MacOS/WebSocketImpl.h"
#include "Core/RaspberryPi/WebSocketImpl.h"

namespace MQTTPlus
{
    Ref<WebSocket> WebSocket::Create(uint32_t port, bool ssl)
    {
    #ifndef MQP_WINDOWS
        return Ref<WebSocketImpl>::Create(port, ssl);
    #else
        return nullptr;
    #endif
    }
}

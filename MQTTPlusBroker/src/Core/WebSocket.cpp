#include "WebSocket.h"

#include "Core/MacOS/WebSocketImpl.h"
#include "Core/RaspberryPi/WebSocketImpl.h"

namespace MQTTPlus
{
    Ref<WebSocket> WebSocket::Create(uint32_t port, bool ssl)
    {
    #if 0   //TODO: Fix
        return Ref<WebSocketImpl>::Create(port, ssl);
    #endif
        return nullptr;
    }
}

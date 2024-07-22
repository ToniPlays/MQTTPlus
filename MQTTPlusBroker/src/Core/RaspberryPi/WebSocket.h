#pragma once

#include <libusockets.h>
#include <iostream>
#include <functional>

namespace MQTTPlus
{
    using SocketConnected = std::function<void(void*)>;
    using SocketDisconnected = std::function<void(void*, int)>;
    using SocketDataReceive = std::function<void(void*, char*, int)>;

    class WebSocket
    {
    public:
        WebSocket() = default;
        WebSocket(uint32_t port, bool ssl = false);
        
        void Listen();
        void SetSocketTimeout(void* socket, uint32_t timeout);
        
        void Write(void* socket, std::vector<uint8_t> bytes);
        
        void SetOnSocketConnected(SocketConnected&& callback)
        {
            m_OnSocketConnected = callback;
        }
        void SetOnSocketDisconnected(SocketDisconnected&& callback)
        {
            m_OnSocketDisconnected = callback;
        }
        
        void SetOnSocketDataReceived(SocketDataReceive&& callback)
        {
            m_OnSocketDataReceived = callback;
        }
        
    private:
        uint32_t m_Port = 0;
        bool m_SSL = false;
        
        SocketConnected m_OnSocketConnected;
        SocketDisconnected m_OnSocketDisconnected;
        SocketDataReceive m_OnSocketDataReceived;
        
    };
}


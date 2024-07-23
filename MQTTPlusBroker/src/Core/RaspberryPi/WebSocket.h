#pragma once
#ifdef MQP_LINUX

#include "Ref.h"
#include <iostream>
#include <functional>
#include <sys/socket.h>
#include "Core/Threading/Thread.h"

namespace MQTTPlus
{
    using SocketConnected = std::function<void(void*)>;
    using SocketDisconnected = std::function<void(void*, int)>;
    using SocketDataReceive = std::function<void(void*, char*, int)>;

    class WebSocket : public RefCount
    {
            friend class SocketClient;
    public:
        WebSocket() = default;
        WebSocket(uint32_t port, bool ssl = false);
        ~WebSocket();
        
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
        static void SocketListenThread(WebSocket* socket);
        
    private:
        uint32_t m_Port = 0;
        bool m_SSL = false;
        
        int m_SocketDesc = 0;
        Ref<Thread> m_ListenerThread;
        
        SocketConnected m_OnSocketConnected;
        SocketDisconnected m_OnSocketDisconnected;
        SocketDataReceive m_OnSocketDataReceived;
        
    };
}

#endif

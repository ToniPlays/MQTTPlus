#pragma once
#ifdef MQP_LINUX

#include "Core/WebSocket.h"
#include <iostream>
#include <functional>
#include <sys/socket.h>
#include "Core/Threading/JobSystem.h"
#include "SocketClient.h"

#include <unordered_map>

namespace MQTTPlus
{

    class WebSocketImpl : public WebSocket
    {
            friend class SocketClient;
    public:
        WebSocketImpl() = default;
        WebSocketImpl(uint32_t port, bool ssl = false);
        ~WebSocketImpl();

        uint32_t GetPort() const override { return m_Port; };
        
        void Listen() override;
        void DisconnectClient(void* socket) override;
        void SetSocketTimeout(void* socket, uint32_t timeout) override;
        
        void Write(void* socket, std::vector<uint8_t> bytes) override;
        void Write(void* socket, const std::string& message) override;
        
        
        void SetOnSocketConnected(SocketConnected&& callback) override
        {
            m_OnSocketConnected = callback;
        }
        void SetOnSocketDisconnected(SocketDisconnected&& callback) override
        {
            m_OnSocketDisconnected = callback;
        }
        
        void SetOnSocketDataReceived(SocketDataReceive&& callback) override
        {
            m_OnSocketDataReceived = callback;
        }

        bool IsListening() const override { return m_ListenerThread; }
        
    private:
        static void SocketListenThread(WebSocketImpl* socket);
        
    private:
        uint32_t m_Port = 0;
        bool m_SSL = false;
        
        std::atomic_bool m_Running = false;
        Ref<Thread> m_ListenerThread;
        std::mutex m_ClientMutex;
        std::unordered_map<uint32_t, Ref<SocketClient>> m_ConnectedClients;
        
        SocketConnected m_OnSocketConnected;
        SocketDisconnected m_OnSocketDisconnected;
        SocketDataReceive m_OnSocketDataReceived;
        
    };
}

#endif

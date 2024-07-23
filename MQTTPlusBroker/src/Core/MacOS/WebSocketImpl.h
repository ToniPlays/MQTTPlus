#pragma once
#ifdef MQP_MACOS

#include <libusockets.h>
#include <iostream>
#include <functional>
#include "Ref.h"
#include "Core/WebSocket.h"


namespace MQTTPlus 
{
    class WebSocketImpl : public WebSocket
    {
    public:
        WebSocketImpl() = default;
        WebSocketImpl(uint32_t port, bool ssl = false);
        
        void Listen() override;
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
        
    private:
        static void OnWakeup(us_loop_t* loop);
        static void OnPre(us_loop_t* loop);
        static void OnPost(us_loop_t* loop);
        
        static us_socket_t* OnConnectionOpened(us_socket_t* socket, int isClient, char* ip, int ipLength);
        static us_socket_t* OnConnectionClosed(us_socket_t* socket, int code, void* reason);
        static us_socket_t* OnSocketTimeout(us_socket_t* socket);
        static us_socket_t* OnEnd(us_socket_t* socket);
        
        static us_socket_t* OnDataReceive(us_socket_t* socket, char* data, int length);
        static us_socket_t* OnWritable(us_socket_t* socket);
        
    private:
        uint32_t m_Port = 0;
        bool m_SSL = false;
        
        us_loop_t* m_Loop = nullptr;
        us_socket_context_t* m_SocketContext = nullptr;
        us_socket_t* m_Socket = nullptr;
        
        SocketConnected m_OnSocketConnected;
        SocketDisconnected m_OnSocketDisconnected;
        SocketDataReceive m_OnSocketDataReceived;
        
        struct WebSocketEXT
        {
            WebSocketImpl* Socket;
        };
    };
}
#endif

#pragma once

#include <libusockets.h>
#include <iostream>
#include <functional>

namespace MQTTPlus 
{
    using SocketConnected = std::function<void(us_socket_t*)>;
    using SocketDisconnected = std::function<void(us_socket_t*, int)>;
    using SocketDataReceive = std::function<void(us_socket_t*, char*, int)>;

    class WebSocket
    {
    public:
        WebSocket() = default;
        WebSocket(uint32_t port, bool ssl = false);
        
        void Listen();
        void SetSocketTimeout(us_socket_t* socket, uint32_t timeout);
        
        void Write(us_socket_t* socket, std::vector<uint8_t> bytes);
        
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
            WebSocket* Socket;
        };
    };
}

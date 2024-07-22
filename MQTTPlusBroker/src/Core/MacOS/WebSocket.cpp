#include "WebSocket.h"
#include "MQTTPlusException.h"
#include <loop.c>
#include "spdlog/fmt/fmt.h"

namespace MQTTPlus 
{
    WebSocket::WebSocket(uint32_t port, bool ssl) : m_Port(port), m_SSL(ssl)
    {
        m_Loop = us_create_loop(nullptr, OnWakeup, OnPre, OnPost, 0);
        
        us_socket_context_options_t options = {};
        m_SocketContext = us_create_socket_context(ssl, m_Loop, sizeof(WebSocketEXT), options);
        
        us_socket_context_on_open(ssl, m_SocketContext, OnConnectionOpened);
        us_socket_context_on_close(ssl, m_SocketContext, OnConnectionClosed);
        us_socket_context_on_timeout(ssl, m_SocketContext, OnSocketTimeout);
        
        us_socket_context_on_end(ssl, m_SocketContext, OnEnd);
        us_socket_context_on_data(ssl, m_SocketContext, OnDataReceive);
        us_socket_context_on_writable(ssl, m_SocketContext, OnWritable);
        
        std::cout << fmt::format("Created websocket for port {}", m_Port) << std::endl;
    }

    void WebSocket::Listen()
    {
        us_listen_socket_t* listenSocket = us_socket_context_listen(m_SSL ? 1 : 0, m_SocketContext, 0, m_Port, 0, 0);
        if(!listenSocket)
            throw MQTTPlusException(fmt::format("Could not listen to port: {}", m_Port));
        
        
        std::cout << fmt::format("Listening to port: {}", m_Port) << std::endl;
        WebSocketEXT& s = *(WebSocketEXT*)us_socket_context_ext(m_SSL, m_SocketContext);
        s.Socket = this;
        us_loop_run(m_Loop);
    }
    void WebSocket::SetSocketTimeout(void* socket, uint32_t timeout)
    {
        us_socket_timeout(m_SSL, (us_socket_t*)socket, timeout);
    }

    void WebSocket::Write(void* socket, std::vector<uint8_t> bytes)
    {
        us_socket_write(m_SSL, (us_socket_t*)socket, (char*)bytes.data(), (uint32_t)bytes.size(), 0);
    }

    void WebSocket::OnWakeup(us_loop_t* loop)
    {
        
    }

    void WebSocket::OnPre(us_loop_t* loop)
    {
        
    }

    void WebSocket::OnPost(us_loop_t* loop)
    {
        
    }

    us_socket_t* WebSocket::OnConnectionOpened(us_socket_t* socket, int isClient, char* ip, int ipLength)
    {
        WebSocketEXT& ext = *(WebSocketEXT*)us_socket_context_ext(false, socket->context);
        if(ext.Socket->m_OnSocketConnected)
            ext.Socket->m_OnSocketConnected(socket);
        
        
        us_socket_timeout(0, socket, 30);
        return socket;
    }

    us_socket_t* WebSocket::OnConnectionClosed(us_socket_t* socket, int code, void* reason)
    {
        
        WebSocketEXT& ext = *(WebSocketEXT*)us_socket_context_ext(false, socket->context);
        if(ext.Socket->m_OnSocketDisconnected)
            ext.Socket->m_OnSocketDisconnected(socket, code);
        return socket;
    }

    us_socket_t* WebSocket::OnSocketTimeout(us_socket_t* socket)
    {
        
        WebSocketEXT& ext = *(WebSocketEXT*)us_socket_context_ext(false, socket->context);
        if(ext.Socket->m_OnSocketDisconnected)
            ext.Socket->m_OnSocketDisconnected(socket, -1);
        return socket;
    }

    us_socket_t* WebSocket::OnEnd(us_socket_t* socket)
    {
        std::cout << "On End\n";
        return socket;
    }

    us_socket_t* WebSocket::OnDataReceive(us_socket_t* socket, char* data, int length)
    {
        WebSocketEXT& ext = *(WebSocketEXT*)us_socket_context_ext(false, socket->context);
        if(ext.Socket->m_OnSocketDataReceived)
            ext.Socket->m_OnSocketDataReceived(socket, data, length);
        return socket;
    }

    us_socket_t* WebSocket::OnWritable(us_socket_t* socket)
    {
        std::cout << "OnWriteable\n";
        return socket;
    }
}

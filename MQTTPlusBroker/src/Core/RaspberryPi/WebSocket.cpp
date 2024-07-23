#ifdef MQP_LINUX

#include "WebSocket.h"
#include "Core/MQTTPlusException.h"
#include "SocketClient.h"
#include "spdlog/fmt/fmt.h"
#include <chrono>

#include <netinet/in.h>
#include <arpa/inet.h>

namespace MQTTPlus
{
    WebSocket::WebSocket(uint32_t port, bool ssl) : m_Port(port), m_SSL(ssl)
    {   
        m_SocketDesc = socket(AF_INET, SOCK_STREAM, 0);
        
        if(!m_SocketDesc)
            throw MQTTPlusException("Failed to create socket");

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        
        if(bind(m_SocketDesc, (sockaddr*)&addr, sizeof(sockaddr_in)) != 0)
            throw MQTTPlusException("Failed to bind socket");
        
        std::cout << fmt::format("Created websocket for port {}", m_Port) << std::endl;
    }
    
    WebSocket::~WebSocket() {
        close(m_SocketDesc);
        if(m_ListenerThread)
            m_ListenerThread->Join();
    }

    void WebSocket::Listen()
    {
        m_ListenerThread = Ref<Thread>::Create(std::thread(&WebSocket::SocketListenThread, this));
    }
    
    void WebSocket::SetSocketTimeout(void* socket, uint32_t timeout)
    {
        int client = ((SocketClient*)socket)->GetClientID();

        timeval val = {};
        val.tv_sec = timeout;
        val.tv_usec = 0;
                
        if(setsockopt (client, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, sizeof(timeval)))
            std::cout << fmt::format("setsockopt RCV for {}: fail\n", client);
        if(setsockopt (client, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, sizeof(timeval)))
            std::cout << fmt::format("setsockopt SND for {}: fail\n", client);
    }

    void WebSocket::Write(void* socket, std::vector<uint8_t> bytes)
    {
        int client = ((SocketClient*)socket)->GetClientID();
        write(client, bytes.data(), bytes.size());
    }
    
    void WebSocket::SocketListenThread(WebSocket* socket)
    {
        std::cout << fmt::format("Listening on port {}", socket->m_Port) << std::endl;
        listen(socket->m_SocketDesc, 10);
        while(true)
        {            
            int client = accept(socket->m_SocketDesc, NULL, NULL);
            
            Ref<SocketClient> socketClient = Ref<SocketClient>::Create(socket, client);
            socketClient->IncRefCount();
            
            int pid = fork();
            
            if(pid == 0)
            {
                socket->SetSocketTimeout((void*)socketClient.Raw(), 30);
                socket->m_OnSocketConnected((void*)socketClient.Raw());
                socketClient->Start();
            }
            else if(pid == -1)
            {
                
            }
            else 
            {
                close(client);
            }
        }
        std::cout << fmt::format("Listening ended") << std::endl;
    }
}

#endif

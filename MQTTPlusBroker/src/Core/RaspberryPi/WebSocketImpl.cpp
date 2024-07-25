#ifdef MQP_LINUX

#include "WebSocketImpl.h"
#include "Core/MQTTPlusException.h"
#include "Core/Logger.h"
#include "SocketClient.h"
#include "spdlog/fmt/fmt.h"
#include "Core/Timer.h"
#include <chrono>

#include <netinet/in.h>
#include <arpa/inet.h>

namespace MQTTPlus
{
    WebSocketImpl::WebSocketImpl(uint32_t port, bool ssl) : m_Port(port), m_SSL(ssl)
    {
        m_SocketDesc = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        
        if(!m_SocketDesc)
            throw MQTTPlusException("Failed to create socket");

		const int enable = 1;
		if(setsockopt(m_SocketDesc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			throw MQTTPlusException("Failed to set socketop");

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        
        if(bind(m_SocketDesc, (sockaddr*)&addr, sizeof(sockaddr_in)) != 0)
            throw MQTTPlusException("Failed to bind socket");
        
        m_OnSocketConnected = [](void* socket) {};
        m_OnSocketDisconnected = [](void* socket, int code) {};
        m_OnSocketDataReceived = [](void* socket, char* data, int length) {};
        
        MQP_WARN("Created WebSocket for port {} (Linux native)", m_Port);
    }
    
    WebSocketImpl::~WebSocketImpl() 
    {
        m_Running = false;
        close(m_SocketDesc);
        if(m_ListenerThread)
            m_ListenerThread->Join();
    }

    void WebSocketImpl::Listen()
    {
        m_Running = true;
        m_ListenerThread = Ref<Thread>::Create(std::thread(&WebSocketImpl::SocketListenThread, this));
    }
    
    void WebSocketImpl::SetSocketTimeout(void* socket, uint32_t timeout)
    {
        int client = ((SocketClient*)socket)->GetClientID();

        timeval val = {};
        val.tv_sec = timeout;
        val.tv_usec = 0;
                
        if(setsockopt (client, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, sizeof(timeval)))
            MQP_FATAL("setsockopt RCV for {}: fail\n", client);
        if(setsockopt (client, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, sizeof(timeval)))
            MQP_FATAL("setsockopt SND for {}: fail\n", client);
   
    }

    void WebSocketImpl::Write(void* socket, std::vector<uint8_t> bytes)
    {
        int client = ((SocketClient*)socket)->GetClientID();
        write(client, bytes.data(), bytes.size());
    }

    void WebSocketImpl::Write(void* socket, const std::string& message)
    {
        int client = ((SocketClient*)socket)->GetClientID();
        std::cout << message << std::endl;
        write(client, message.data(), message.length());
    }
    
    void WebSocketImpl::SocketListenThread(WebSocketImpl* socket)
    {
        MQP_INFO("Listening on port {}", socket->m_Port);
        listen(socket->m_SocketDesc, 10);

        float timeout = 100;

        while(socket->m_Running)
        {   
            Timer timer;

            int client = accept(socket->m_SocketDesc, NULL, NULL);
            if(client != -1)
            {                
                Ref<SocketClient> socketClient = Ref<SocketClient>::Create(socket, client);
                MQP_WARN("New connection on port: {}", socket->m_Port);
                
                
                int pid = fork();
                
                if(pid == 0)
                {
                    socket->SetSocketTimeout((void*)socketClient.Raw(), 30);
                    socket->m_OnSocketConnected((void*)socketClient.Raw());
                    socket->m_ConnectedClients[client] = socketClient;
                }
                else if(pid == -1)
                {
                    
                }
                else 
                {
                    close(client);
                }
                
            }
            socket->m_ClientMutex.lock();
            auto clients = socket->m_ConnectedClients;
            socket->m_ClientMutex.unlock();

            for(auto& [id, client] : clients) {
                if(!client->Read())
                {
                    socket->m_OnSocketDisconnected((void*)client.Raw(), -1);
                    socket->m_ClientMutex.lock();
                    auto it = socket->m_ConnectedClients.find(id);
                    if(it != socket->m_ConnectedClients.end())
                        socket->m_ConnectedClients.erase(it);
                    socket->m_ClientMutex.unlock();
                }
            }

            
            int sleepFor = timeout - timer.ElapsedMillis();
            if(sleepFor > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
            
        }
        MQP_ERROR("Stopped listening on port {}", socket->m_Port);
    }
}

#endif

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
        m_OnSocketConnected = [](void* socket) {};
        m_OnSocketDisconnected = [](void* socket, int code) {};
        m_OnSocketDataReceived = [](void* socket, char* data, int length) {};

        MQP_WARN("Created WebSocket for port {} (Linux native)", m_Port);
    }
    
    WebSocketImpl::~WebSocketImpl() 
    {
        m_Running = false;
        
        if(m_ListenerThread)
            m_ListenerThread->Join();
    }

    void WebSocketImpl::Listen()
    {
        m_Running = true;
        WebSocketImpl::SocketListenThread(this);
    }
    void WebSocketImpl::DisconnectClient(void* socket)
    {
        SocketClient* client = (SocketClient*)socket;
        close(client->GetClientID()); 
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
        write(client, message.data(), message.length());
    }
    
    void WebSocketImpl::SocketListenThread(WebSocketImpl* socketImpl)
    {
        thread_local static fd_set m_MasterSet;
        thread_local static int m_MaxFD = 0;
        thread_local static int m_SocketDesc = 0;

        m_SocketDesc = socket(AF_INET, SOCK_STREAM, 0);
        if (m_SocketDesc < 0) {
            throw MQTTPlusException("Failed to create socket");
        }

        // Set socket to non-blocking
        fcntl(m_SocketDesc, F_SETFL, O_NONBLOCK);

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(socketImpl->m_Port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(m_SocketDesc, (sockaddr*)&addr, sizeof(sockaddr_in)) < 0) {
            close(m_SocketDesc);
            throw MQTTPlusException("Failed to bind socket");
        }

        // Start listening on the socket
        listen(m_SocketDesc, SOMAXCONN);
        MQP_INFO("Listening on port {}", socketImpl->m_Port);

        FD_ZERO(&m_MasterSet);
        FD_SET(m_SocketDesc, &m_MasterSet);
        m_MaxFD = m_SocketDesc;

        MQP_WARN("MaxFD: {}", m_MaxFD);

        while (socketImpl->m_Running) {

            fd_set readSet = m_MasterSet;
            // Wait for activity on any of the sockets
            int activity = select(m_MaxFD + 1, &readSet, NULL, NULL, NULL);

            if (activity < 0 && errno != EINTR) {
                MQP_ERROR("Select error: {}, activity {}", strerror(errno), activity);
                break;
            }

            if(activity == 0) continue;

            for (int i = 0; i <= m_MaxFD; ++i) {
                if (FD_ISSET(i, &readSet)) {
                    if (i == m_SocketDesc) {
                        // New client connection
                        int client = accept(m_SocketDesc, NULL, NULL);
                        if (client < 0) continue;  // skip if accept failed

                        // Set client socket to non-blocking
                        fcntl(client, F_SETFL, O_NONBLOCK);

                        // Add the client to the master set
                        FD_SET(client, &m_MasterSet);
                        if (client > m_MaxFD) {
                            m_MaxFD = client;
                        }

                        Ref<SocketClient> socketClient = Ref<SocketClient>::Create(socketImpl, client);
                        MQP_WARN("New connection on port: {} {}", socketImpl->m_Port, client);

                        socketImpl->SetSocketTimeout((void*)socketClient.Raw(), 30);
                        socketImpl->m_OnSocketConnected((void*)socketClient.Raw());

                        socketImpl->m_ClientMutex.lock();
                        socketImpl->m_ConnectedClients[client] = socketClient;
                        socketImpl->m_ClientMutex.unlock();
                    }
                    else 
                    {
                        // Handle existing client activity
                        Ref<SocketClient> socketClient = socketImpl->m_ConnectedClients[i];

                        if (socketClient->Read()) {
                            continue;  // Continue reading
                        }

                        socketImpl->m_OnSocketDisconnected((void*)socketClient.Raw(), -1);

                        // Lock and clean up client
                        socketImpl->m_ClientMutex.lock();
                        MQP_WARN("Client disconnected {}", i);
                        FD_CLR(i, &m_MasterSet);  // Remove from select set
                        close(i);  // Close client socket
                        socketImpl->m_ConnectedClients.erase(i);
                        socketImpl->m_ClientMutex.unlock();
                    }
                }
            }
        }

        // Close the server socket
        FD_CLR(m_SocketDesc, &m_MasterSet);  // Remove from select set
        close(m_SocketDesc);
        MQP_WARN("Stopped listening on port {}", socketImpl->m_Port);
    }
}

#endif

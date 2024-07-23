#ifdef MQP_LINUX

#include "SocketClient.h"
#include <sys/socket.h>
#include <spdlog/fmt/fmt.h>
#include <chrono>

#include "Core/Buffer/Buffer.h"
#include "Core/WebSocket.h"

namespace MQTTPlus 
{
	SocketClient::SocketClient(WebSocket* socket, int socketId) : m_WebSocket(socket), m_SocketId(socketId)
	{
		
	}
	
	void SocketClient::Start() {
		m_Thread = Ref<Thread>::Create(std::thread(&SocketClient::EventFunc, this));
	}
	
	void SocketClient::EventFunc(SocketClient* client)
	{		
		while(true)
		{
			char buffer[8192];
			int read = recv(client->GetClientID(), buffer, 8192, 0);
			if(read < 0)
				continue;

			client->m_WebSocket->m_OnSocketDataReceived((void*)client, buffer, read);
		}
	}
}
#endif

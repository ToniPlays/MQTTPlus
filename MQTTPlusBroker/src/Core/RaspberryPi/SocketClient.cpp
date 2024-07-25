#ifdef MQP_LINUX

#include "SocketClient.h"
#include "Core/Logger.h"
#include <sys/socket.h>
#include <spdlog/fmt/fmt.h>
#include <chrono>

#include "Core/Buffer/Buffer.h"
#include "WebSocketImpl.h"

namespace MQTTPlus 
{
	SocketClient::SocketClient(WebSocketImpl* socket, int socketId) : m_SocketId(socketId), m_WebSocket(socket)
	{
		
	}
	
	bool SocketClient::Read()
	{	
		fd_set rfds = {};
		timeval tv = {
			.tv_sec = 0,
			.tv_usec = 1000,
		};

		int result = select(1, &rfds, NULL, NULL, &tv);
		if(result == - 1)
			return true;

		char buffer[8192];
		int read = recv(GetClientID(), buffer, 8192, 0);

		if(read <= 0)
		{
			return false;
		}
		
		m_WebSocket->m_OnSocketDataReceived((void*)this, buffer, read);
		return true;
	}
}
#endif

#ifdef MQP_LINUX

#include "SocketClient.h"
#include "Core/Logger.h"
#include "Core/MQTTPlusException.h"
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
		char buffer[8192];
		int read = recv(GetClientID(), buffer, 8192, 0);

		if(read <= 0)
			return false;		
		try 
		{
			m_WebSocket->m_OnSocketDataReceived((void*)this, buffer, read);
		} catch(MQTTPlusException e)
		{
			MQP_WARN("Socket data receive failure: {}", e.what());
		}
		catch(std::exception e)
		{
			MQP_ERROR("Unknown error");
		}
		return true;
	}
}
#endif

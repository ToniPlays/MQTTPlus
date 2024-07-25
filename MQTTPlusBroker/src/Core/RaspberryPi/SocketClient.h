#pragma once
#ifdef MQP_LINUX

#include "Ref.h"
#include "Core/Threading/Thread.h"


namespace MQTTPlus {
	class WebSocketImpl;
	
	class SocketClient : public RefCount {
	public:
		SocketClient(WebSocketImpl* socket, int socketId);
		
		int GetClientID() const { return m_SocketId; }
		
		bool Read();
		
	private:
		int m_SocketId = 0;
		WebSocketImpl* m_WebSocket;
	};
}
#endif

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
		
		void Start();
		
	private:
		static void EventFunc(SocketClient* client);
		
	private:
		WebSocketImpl* m_WebSocket = nullptr;
		int m_SocketId = 0;
		
		Ref<Thread> m_Thread;
	};
}
#endif

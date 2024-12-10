#pragma once

#include "Ref.h"
#include "Core/Event/Event.h"
#include "Core/CallbackTimer.h"
#include "Core/Threading/JobSystem.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace MQTTPlus {
    class HTTPServer;

    using Server = websocketpp::server<websocketpp::config::asio>;
    
    class HTTPClient : public RefCount 
    {
    public:
        HTTPClient(HTTPServer* server, Server::connection_ptr ptr) : m_Server(server), m_ConnPtr(ptr) {
            m_Timer.Start();
        };
        ~HTTPClient() {
            m_Timer.Clear();
        };
        
        void SetTimedFunction(float timeout, const std::function<void(HTTPClient&)>& callback);
        void SubscribeToEvent(const std::string& type);
        std::vector<std::string> GetSubscribedEvents() { return m_SubscribedEvents; }
        void Send(const std::string& message);
        
        CallbackTimer& GetTimer() { return m_Timer; }

        void OnEvent(Event& e);
        
    private:
        HTTPServer* m_Server = nullptr;
        Server::connection_ptr m_ConnPtr;
        CallbackTimer m_Timer; 
        std::vector<std::string> m_SubscribedEvents;
    };
}

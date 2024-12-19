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

        void ClearEvents()
        {
            m_SubscribedEvents.clear();
            m_ChangeEventSubscribes.clear();
        }
        
        void SetTimedFunction(float timeout, const std::function<void(HTTPClient&)>& callback);
        void SubscribeToEvent(const std::string& type);
        const std::vector<std::string>& GetSubscribedEvents() const { return m_SubscribedEvents; }
        const std::vector<std::string>& GetSubscribedChangeEvents() { return m_ChangeEventSubscribes; }
        
        bool HasSubscribedToChangeOf(const std::string& publicId) const ;

        void Send(const std::string& message);
        
        CallbackTimer& GetTimer() { return m_Timer; }

        void OnEvent(Event& e);
        
    private:
        HTTPServer* m_Server = nullptr;
        Server::connection_ptr m_ConnPtr;
        CallbackTimer m_Timer; 
        std::vector<std::string> m_SubscribedEvents;
        std::vector<std::string> m_ChangeEventSubscribes;
    };
}

#include "Broker.h"
#include "MQTTPlusException.h"


namespace MQTTPlus
{
    Broker::Broker(const BrokerCreateSettings& settings) : m_Settings(settings) {
        try 
        {
            m_WebSocket = WebSocket::Create(settings.Port, settings.UseSSL);
        } catch(MQTTPlusException e)
        {
            m_WebSocket = nullptr;
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    void Broker::Listen()
    {
        if(!m_WebSocket) return;
        
        m_WebSocket->SetOnSocketConnected([this](void* client) {
            Ref<Client> c = Ref<Client>::Create(this, client);
            
            c->SetSendMessageCallback([socket = m_WebSocket, client](Ref<MQTT::Message> message) mutable {
                socket->Write(client, message->GetBytes());
            });
            
            std::scoped_lock lock(m_ClientMutex);
            m_ConnectedClients[client] = c;
            
            //EventQueueAdd([this, client, c]() mutable {
            std::cout << fmt::format("Client Socket connected {}", (uint64_t)client) << std::endl;
            //});
        });
        
        m_WebSocket->SetOnSocketDisconnected([this](void* client, int reason) {
            std::cout << fmt::format("Client Socket disconnected {}", (uint64_t)client) << std::endl;
            std::scoped_lock lock(m_ClientMutex);
            
            auto it = m_ConnectedClients.find(client);
            if(it != m_ConnectedClients.end())
                m_ConnectedClients.erase(it);
        });
        
        m_WebSocket->SetOnSocketDataReceived([this](void* client, char* data, int length) {
            std::scoped_lock lock(m_ClientMutex);
            //EventQueueAdd([this, client, data, length]() mutable {
            if(m_ConnectedClients[client])
                m_ConnectedClients[client]->ProcessData(data, length);
            //});
        });
        
        m_WebSocket->Listen();
    }
    void Broker::ProcessMessages()
    {
        return;
        std::scoped_lock lock(m_QueueMutex);
        std::cout << "Message queue size: " << m_EventQueue.size() << std::endl;
        std::cout << fmt::format("Client count {}", m_ConnectedClients.size()) << std::endl;
        
        if(m_EventQueue.size() == 0) return;
        
        std::cout << "Actually not doing shit" << std::endl;
        
        for(auto& cb : m_EventQueue)
            cb();
            
        m_EventQueue.clear();
    }

    MQTT::ConnAckFlags Broker::OnMQTTClientConnected(Ref<Client> client, const MQTT::Authentication& auth)
    {
        m_WebSocket->SetSocketTimeout(client->m_NativeSocket, client->m_KeepAliveFor);
        client->m_Authentication = auth;
        
        std::scoped_lock lock(m_QueueMutex);
        //EventQueueAdd([this, client]() mutable {
            m_OnClientConnected(client);
        //});
        
        return MQTT::ConnAckFlags::Accepted;
    }
    void Broker::OnMQTTPublishReceived(Ref<Client> client, Ref<MQTT::PublishMessage> message)
    {
        std::scoped_lock lock(m_QueueMutex);
        //EventQueueAdd([this, client, message]() mutable {
            std::cout << message->ToString() << std::endl;
        //});
    }
}

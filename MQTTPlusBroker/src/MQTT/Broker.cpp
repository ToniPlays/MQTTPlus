#include "Broker.h"
#include "Core/MQTTPlusException.h"


namespace MQTTPlus
{
    Broker::Broker(const BrokerCreateSettings& settings) : m_Settings(settings) 
    {
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
            Ref<MQTTClient> c = Ref<MQTTClient>::Create(this, client);
            
            c->SetSendMessageCallback([socket = m_WebSocket, client](Ref<MQTT::Message> message) mutable {
                socket->Write(client, message->GetBytes());
            });
            
            std::scoped_lock lock(m_ClientMutex);
            m_ConnectedClients[client] = c;
            
            std::cout << fmt::format("Client Socket connected {}", (uint64_t)client) << std::endl;
            
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
            
            if(m_ConnectedClients[client])
                m_ConnectedClients[client]->ProcessData(data, length);
            
        });
        
        m_WebSocket->Listen();
    }

    BrokerStatus Broker::GetStatus() const
    {
        if (!m_WebSocket) return BrokerStatus::Disabled;

        return m_WebSocket->IsListening() ? BrokerStatus::Connected : BrokerStatus::Crashed;
    }

    MQTT::ConnAckFlags Broker::OnMQTTClientConnected(Ref<MQTTClient> client, const MQTT::Authentication& auth)
    {
        m_WebSocket->SetSocketTimeout(client->m_NativeSocket, client->m_KeepAliveFor);
        client->m_Authentication = auth;
        
        m_OnClientConnected(client);
        
        return MQTT::ConnAckFlags::Accepted;
    }

    void Broker::OnMQTTClientDisonnected(Ref<MQTTClient> client, int reason)
    {
        m_OnClientDisconnected(client, reason);
    }

    void Broker::OnMQTTPublishReceived(Ref<MQTTClient> client, Ref<MQTT::PublishMessage> message)
    {
        std::cout << message->ToString() << std::endl;
    }
}

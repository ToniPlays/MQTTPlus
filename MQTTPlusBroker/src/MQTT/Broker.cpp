#include "Broker.h"
#include "Core/MQTTPlusException.h"
#include "Core/Logger.h"

namespace MQTTPlus
{
    Broker::Broker(const BrokerCreateSettings& settings) : m_Settings(settings) 
    {
        try 
        {
            m_WebSocket = WebSocket::Create(settings.Port, settings.UseSSL);
        } catch(MQTTPlusException e)
        {
            MQP_ERROR("Failed to create WebSocket for Broker: {0}", e.what());
            m_WebSocket = nullptr;
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
            
            m_ClientMutex.lock();
            m_ConnectedClients[client] = c;
            m_ClientMutex.unlock();
        });
        
        m_WebSocket->SetOnSocketDisconnected([this](void* client, int reason) mutable {
            m_ClientMutex.lock();
            
            auto it = m_ConnectedClients.find(client);
            if(it != m_ConnectedClients.end())
            {
                OnMQTTClientDisconnected(m_ConnectedClients[client], reason);
                m_ConnectedClients.erase(it);
            }
            m_ClientMutex.unlock();
            
        });
        
        m_WebSocket->SetOnSocketDataReceived([this](void* client, char* data, int length) {
            if(m_ConnectedClients[client])
                m_ConnectedClients[client]->ProcessData(data, length);
        });
        
        m_WebSocket->Listen();
        MQP_WARN("MQTT Server listening on port {}", m_WebSocket->GetPort());
    }

    void Broker::Disconnect(Ref<MQTTClient> client)
    {
        m_WebSocket->DisconnectClient(client->m_NativeSocket);
    }

    BrokerStatus Broker::GetStatus() const
    {
        if (!m_WebSocket) return BrokerStatus::Disabled;

        return m_WebSocket->IsListening() ? BrokerStatus::Connected : BrokerStatus::Crashed;
    }

    MQTTPlus::MQTT::ConnAckFlags Broker::OnMQTTClientConnected(Ref<MQTTClient> client, const MQTT::Authentication& auth)
    {
        if(!auth.Valid)
            return MQTT::ConnAckFlags::IdentifierRejected;

        m_WebSocket->SetSocketTimeout(client->m_NativeSocket, client->m_KeepAliveFor);
        bool isNew = client->GetAuth().ClientID.empty();
        client->m_Authentication = auth;
        
        if(isNew)
            m_OnClientConnectionChange(client, true, 0);

        return MQTT::ConnAckFlags::Accepted;
    }

    void Broker::OnMQTTClientDisconnected(Ref<MQTTClient> client, int reason)
    {
        if(client->GetAuth().Valid)
            m_OnClientConnectionChange(client, false, reason);
    }

    bool Broker::OnMQTTPublishReceived(Ref<MQTTClient> client, Ref<MQTT::PublishMessage> message)
    {
        if(!m_OnClientPublished) return false;

        m_OnClientPublished(client, message->GetTopic(), message->GetMessage());
        return true;
    }
    bool Broker::OnMQTTSubscribeReceived(Ref<MQTTClient> client, Ref<MQTT::SubscribeMessage> message)
    {
        if(!m_OnClientSubscribed) return false;

        for(auto& topic : message->GetTopics())
            m_OnClientSubscribed(client, topic);

        return true;
    }
}

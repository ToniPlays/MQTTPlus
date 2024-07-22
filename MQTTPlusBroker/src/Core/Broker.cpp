#include "Broker.h"
#include "MQTTPlusException.h"
#include <format>


namespace MQTTPlus
{
    Broker::Broker(const BrokerCreateSettings& settings) : m_Settings(settings) {
        m_WebSocket = WebSocket(settings.Port, settings.UseSSL);
    }

    void Broker::Listen()
    {
        m_WebSocket.SetOnSocketConnected([this](us_socket_t* client) {
            Ref<Client> c = Ref<Client>::Create(this, client);
            c->SetSendMessageCallback([socket = m_WebSocket, client](Ref<MQTT::Message> message) mutable {
                socket.Write(client, message->GetBytes());
            });
            m_ConnectedClients[client] = c;
        });
        
        m_WebSocket.SetOnSocketDisconnected([this](us_socket_t* client, int reason) {
            auto it = m_ConnectedClients.find(client);
            if(it != m_ConnectedClients.end())
                m_ConnectedClients.erase(it);
        });
        
        m_WebSocket.SetOnSocketDataReceived([this](us_socket_t* client, char* data, int length) {
            if(m_ConnectedClients[client])
                m_ConnectedClients[client]->ProcessData(data, length);
        });
        
        m_WebSocket.Listen();
    }
    MQTT::ConnAckFlags Broker::OnMQTTClientConnected(Ref<Client> client, const MQTT::Authentication& auth)
    {
        m_WebSocket.SetSocketTimeout(client->m_Socket, client->m_KeepAliveFor);
        return MQTT::ConnAckFlags::Accepted;
    }
}

#pragma once

#include "BrokerCreateSettings.h"
#include "WebSocket.h"
#include "Client/Client.h"

#include <iostream>
#include <string.h>

namespace MQTTPlus 
{
using OnClientConnected = std::function<void(Ref<Client>)>;
    class Broker
    {
        friend class Client;
    public:
        Broker(const BrokerCreateSettings& settings);
        ~Broker() = default;
        
        void Listen();
        
        void SetOnClientConnected(OnClientConnected&& callback) {
            m_OnClientConnected = callback;
        }
        
    private:
        MQTT::ConnAckFlags OnMQTTClientConnected(Ref<Client> client, const MQTT::Authentication& auth);
        
    private:
        
        struct SocketExt {
            Broker* Broker;
        };
        
        BrokerCreateSettings m_Settings = {};
        WebSocket m_WebSocket;

        std::unordered_map<us_socket_t*, Ref<Client>> m_ConnectedClients;
        
        OnClientConnected m_OnClientConnected;
    };
}

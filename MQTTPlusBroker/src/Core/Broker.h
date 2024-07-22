#pragma once

#include "BrokerCreateSettings.h"
#include "WebSocket.h"
#include "Client/Client.h"

#include <iostream>
#include <string.h>

namespace MQTTPlus 
{
    using OnClientConnected = std::function<void(Ref<Client>)>;
    using OnPublished = std::function<void(Ref<Client>&, Buffer)>;

    class Broker
    {
        friend class Client;
    public:
        Broker(const BrokerCreateSettings& settings);
        ~Broker() = default;
        
        void Listen();
        void OnPublish(const std::string& topic, OnPublished&& callback) {}
        
        void SetOnClientConnected(OnClientConnected&& callback) {
            m_OnClientConnected = callback;
        }
        
    private:
        MQTT::ConnAckFlags OnMQTTClientConnected(Ref<Client> client, const MQTT::Authentication& auth);
        void OnMQTTPublishReceived(Ref<Client> client, Ref<MQTT::PublishMessage> message);
        
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

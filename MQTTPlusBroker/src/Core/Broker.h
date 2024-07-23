#pragma once

#include "BrokerCreateSettings.h"
#include "WebSocket.h"
#include "Client/Client.h"

#include <mutex>
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
        void ProcessMessages();
        void OnPublish(const std::string& topic, OnPublished&& callback) {}
        
        void SetOnClientConnected(OnClientConnected&& callback) {
            m_OnClientConnected = callback;
        }
        
        void EventQueueAdd(std::function<void()>&& callback)
        {
                m_QueueMutex.lock();
                m_EventQueue.push_back(callback);
                std::cout << m_EventQueue.size() << std::endl;
                m_QueueMutex.unlock();
        }
        
    private:
        MQTT::ConnAckFlags OnMQTTClientConnected(Ref<Client> client, const MQTT::Authentication& auth);
        void OnMQTTPublishReceived(Ref<Client> client, Ref<MQTT::PublishMessage> message);
        
    private:
        
        BrokerCreateSettings m_Settings = {};
        Ref<WebSocket> m_WebSocket;
        std::vector<std::function<void()>> m_EventQueue;
        
        std::mutex m_QueueMutex;
        std::mutex m_ClientMutex;

        std::unordered_map<void*, Ref<Client>> m_ConnectedClients;        
        OnClientConnected m_OnClientConnected;
    };
}

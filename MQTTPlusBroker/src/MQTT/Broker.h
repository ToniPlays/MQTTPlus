#pragma once

#include "BrokerCreateSettings.h"
#include "Core/WebSocket.h"
#include "MQTTClient.h"
#include "Core/Logger.h"

#include <mutex>
#include <iostream>
#include <string.h>
#include <thread>

namespace MQTTPlus 
{
    using OnClientConnected = std::function<void(Ref<MQTTClient>)>;
    using OnClientDisconnected = std::function<void(Ref<MQTTClient>, int)>;
    using OnPublished = std::function<void(Ref<MQTTClient>&, Buffer)>;

    enum class BrokerStatus
    {
        Disabled,
        Connected,
        Crashed
    };

    class Broker
    {
        friend class MQTTClient;
    public:
        Broker(const BrokerCreateSettings& settings);
        Broker() = delete;
        ~Broker() = default;
        
        void Listen();
        void ProcessMessages();
        void OnPublish(const std::string& topic, OnPublished&& callback) {}
        
        void SetOnClientConnected(OnClientConnected&& callback) {
            m_OnClientConnected = callback;
        }
        void SetOnClientDisconnected(OnClientDisconnected&& callback) {
            m_OnClientDisconnected = callback;
        }

        BrokerStatus GetStatus() const;
        Ref<WebSocket> GetWebSocket() const { return m_WebSocket; }

        uint32_t GetConnectedClientCount() {
            return m_ConnectedClients.size(); 
        }
        
    private:
        MQTT::ConnAckFlags OnMQTTClientConnected(Ref<MQTTClient> client, const MQTT::Authentication& auth);
        void OnMQTTClientDisonnected(Ref<MQTTClient> client, int reason);
        void OnMQTTPublishReceived(Ref<MQTTClient> client, Ref<MQTT::PublishMessage> message);
        
    private:
        
        BrokerCreateSettings m_Settings = {};
        Ref<WebSocket> m_WebSocket;
        std::mutex m_ClientMutex;

        std::unordered_map<void*, Ref<MQTTClient>> m_ConnectedClients;        
        OnClientConnected m_OnClientConnected;
        OnClientDisconnected m_OnClientDisconnected;
    };
}

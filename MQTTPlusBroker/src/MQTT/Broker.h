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
    using OnClientConnectionChange = std::function<void(Ref<MQTTClient>, bool, int)>;
    using OnClientSubscribed = std::function<void(Ref<MQTTClient>, const MQTT::SubscribeMessage::Topic&)>;
    using OnClientPublished = std::function<void(Ref<MQTTClient>&, const std::string&, const std::string&)>;

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
        void Disconnect(Ref<MQTTClient> client);
        void ProcessMessages();
        
        void SetOnClientChange(OnClientConnectionChange&& callback) {
            m_OnClientConnectionChange = callback;
        }
        void SetOnSubscribed(OnClientSubscribed&& callback) {
            m_OnClientSubscribed = callback;
        }
        void SetOnPublished(OnClientPublished&& callback) {
            m_OnClientPublished = callback;
        }

        BrokerStatus GetStatus() const;
        Ref<WebSocket> GetWebSocket() const { return m_WebSocket; }

        uint32_t GetConnectedClientCount() {
            return (uint32_t)m_ConnectedClients.size();
        }
        
    private:
        MQTT::ConnAckFlags OnMQTTClientConnected(Ref<MQTTClient> client, const MQTT::Authentication& auth);
        void OnMQTTClientDisconnected(Ref<MQTTClient> client, int reason);
        bool OnMQTTPublishReceived(Ref<MQTTClient> client, Ref<MQTT::PublishMessage> message);
        bool OnMQTTSubscribeReceived(Ref<MQTTClient> client, Ref<MQTT::SubscribeMessage> message);
        
    private:
        
        BrokerCreateSettings m_Settings = {};
        Ref<WebSocket> m_WebSocket;
        std::mutex m_ClientMutex;

        std::unordered_map<void*, Ref<MQTTClient>> m_ConnectedClients;        
        OnClientConnectionChange m_OnClientConnectionChange;
        OnClientSubscribed m_OnClientSubscribed;
        OnClientPublished m_OnClientPublished;
    };
}

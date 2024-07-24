#pragma once

#include "Core/MQTT/MQTTMessageDecoder.h"
#include "MQTTPlusBroker.h"

namespace MQTTPlus 
{
    class Broker;
    class Client : public RefCount
    {
        friend class Broker;
    public:
        Client(Broker* broker, void* nativeSocket) : m_Broker(broker), m_NativeSocket(nativeSocket) {}
        ~Client() = default;
        
        const MQTT::Authentication& GetAuth() const { return m_Authentication; }
        
        void SetSendMessageCallback(std::function<void(Ref<MQTT::Message>)> callback)
        {
            m_SendMessageCallback = callback;
        }
        
        void ProcessData(char* data, int length);
        void SendMessage(Ref<MQTT::Message> message);

    private:
        MQTT::Authentication m_Authentication;
        uint32_t m_KeepAliveFor = 0;
        MQTTMessageDecoder m_Decoder;
        
        std::function<void(Ref<MQTT::Message>)> m_SendMessageCallback;
        Broker* m_Broker;
        void* m_NativeSocket;
    };
}

#pragma once

#include "Core/MQTTMessageDecoder.h"
#include "MQTTPlusBroker.h"

#include <libusockets.h>

namespace MQTTPlus 
{
    class Broker;
    class Client : public RefCount
    {
        friend class Broker;
    public:
        Client(Broker* broker, us_socket_t* socket) : m_Broker(broker), m_Socket(socket) {}
        ~Client() = default;
        
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
        us_socket_t* m_Socket;
    };
}

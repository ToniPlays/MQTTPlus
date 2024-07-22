#include "Client.h"
#include "Ref.h"
#include <format>

#include "Core/Broker.h"

namespace MQTTPlus 
{
    void Client::ProcessData(char* data, int length)
    {
        m_Decoder.DecodeMessage(data, length, [this](Ref<MQTT::Message> message) {
            std::cout << message->ToString() << std::endl;
            switch(message->GetType())
            {
                case MQTT::MessageType::Connect:
                {
                    auto result = m_Broker->OnMQTTClientConnected(this, message.As<MQTT::ConnectMessage>()->GetAuthentication());
                    SendMessage(Ref<MQTT::ConnAckMessage>::Create(result));
                    
                    break;
                }
                default:
                    std::cout << std::format("Message not implemented\n");
                    break;
            }
        });
    }

    void Client::SendMessage(Ref<MQTT::Message> message)
    {
        m_SendMessageCallback(message);
    }
}

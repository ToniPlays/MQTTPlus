#include "Client.h"
#include "Ref.h"

#include "Core/Broker.h"
#include "spdlog/fmt/fmt.h"

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
                    Ref<MQTT::ConnectMessage> msg = message.As<MQTT::ConnectMessage>();
                    auto result = m_Broker->OnMQTTClientConnected(this, msg->GetAuthentication());
                    SendMessage(Ref<MQTT::ConnAckMessage>::Create(result));
                    
                    break;
                }
                case MQTT::MessageType::Publish:
                {
                    Ref<MQTT::PublishMessage> msg = message.As<MQTT::PublishMessage>();
                    m_Broker->OnMQTTPublishReceived(this, msg);
                    break;
                }
                    
                default:
                    std::cout << fmt::format("Message not implemented\n");
                    break;
            }
        });
    }

    void Client::SendMessage(Ref<MQTT::Message> message)
    {
        m_SendMessageCallback(message);
    }
}

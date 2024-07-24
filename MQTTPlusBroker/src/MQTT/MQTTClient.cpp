#include "MQTTClient.h"
#include "Ref.h"

#include "Broker.h"
#include "spdlog/fmt/fmt.h"

namespace MQTTPlus 
{
    void MQTTClient::ProcessData(char* data, int length)
    {
        std::vector<char> buffer(data, data + length);
        
        m_Decoder.DecodeMessage(buffer, [this](Ref<MQTT::Message> message) {
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

    void MQTTClient::SendMessage(Ref<MQTT::Message> message)
    {
        m_SendMessageCallback(message);
    }
}

#include "MQTTClient.h"
#include "Ref.h"

#include "Broker.h"
#include "spdlog/fmt/fmt.h"

namespace MQTTPlus 
{
    void MQTTClient::Disconnect() 
    {
        m_Broker->Disconnect(this);
    }

    void MQTTClient::ProcessData(char* data, int length)
    {
        std::vector<char> buffer(data, data + length);
        
        m_Decoder.DecodeMessage(buffer, [this](Ref<MQTT::Message> message) {
            
            MQP_INFO("MQTT message: {}", message->ToString());
            switch(message->GetType())
            {
                case MQTT::MessageType::Connect:
                {
                    Ref<MQTT::ConnectMessage> msg = message.As<MQTT::ConnectMessage>();
                    auto result = m_Broker->OnMQTTClientConnected(this, msg->GetAuthentication());
                    SendMessage(Ref<MQTT::ConnAckMessage>::Create(result));

                    if(result != MQTT::ConnAckFlags::Accepted)
                        Disconnect();

                    break;
                }
                case MQTT::MessageType::Disconnect:
                {
                    Ref<MQTT::DisconnectMessage> msg = message.As<MQTT::DisconnectMessage>();
                    Disconnect();
                    break;
                }
                case MQTT::MessageType::Publish:
                {
                    Ref<MQTT::PublishMessage> msg = message.As<MQTT::PublishMessage>();
                    m_Broker->OnMQTTPublishReceived(this, msg);
                    break;
                }
                case MQTT::MessageType::Subscribe:
                {
                    Ref<MQTT::SubscribeMessage> msg = message.As<MQTT::SubscribeMessage>();
                    bool result = m_Broker->OnMQTTSubscribeReceived(this, msg);

                    if(result)
                    {
                        std::vector<uint8_t> acks;
                        for(auto& topic : msg->GetTopics())
                            acks.push_back(topic.QOS);
                        SendMessage(Ref<MQTT::SubAckMessage>::Create(acks, msg->GetMessageID()));
                    }
                    
                    break;
                }
                default:
                    MQP_WARN("Message not implemented {}\n", (uint8_t)message->GetType());
                    break;
            }
        });
    }

    void MQTTClient::SendMessage(Ref<MQTT::Message> message)
    {
        m_SendMessageCallback(message);
    }
}

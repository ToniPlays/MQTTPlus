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
                case MQTT::MessageType::Subscribe:
                {
                    Ref<MQTT::SubscribeMessage> msg = message.As<MQTT::SubscribeMessage>();
                    for(auto& topics : msg->GetTopics())
                        MQP_INFO("Client {} subscribes to {} (QoS: {})", m_Authentication.ClientID, topics.Topic, topics.QOS);
                    break;
                }
                default:
                    MQP_WARN("Message not implemented\n");
                    break;
            }
        });
    }

    void MQTTClient::SendMessage(Ref<MQTT::Message> message)
    {
        m_SendMessageCallback(message);
    }
}

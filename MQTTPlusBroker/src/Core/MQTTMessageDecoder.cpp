#include "MQTTMessageDecoder.h"
#include "MQTT.h"
#include <memory>

#include "spdlog/fmt/fmt.h"

namespace MQTTPlus {
    static int RemainingLength(gsl::span<uint8_t> bytes) noexcept {
        // Decerealiser dec{bytes};
        // return dec.create<MqttFixedHeader>().remaining;
        //let's cheat and assume small messages
        return bytes[1];
    }

    static int TotalLength(gsl::span<uint8_t> bytes) noexcept {
        return bytes.size() >= MQTT::FixedHeader::SIZE
        ? RemainingLength(bytes) + MQTT::FixedHeader::SIZE
        : MQTT::FixedHeader::SIZE;
    }

    MQTTMessageDecoder::MQTTMessageDecoder(uint32_t size)
    {
        m_Buffer.reserve(size);
        m_Begin = m_Buffer.begin();
    }

    void MQTTMessageDecoder::DecodeMessage(char* data, int length, OnMessageDecoded&& callback)
    {
        m_Buffer.assign(data, data + length);
        
        auto slice = gsl::as_span(m_Buffer.data(), std::distance(m_Buffer.begin(), m_Begin) + length);
        auto totalLength = TotalLength(slice);
        
        while(slice.size() >= totalLength)
        {
            const auto message = slice.sub(0, totalLength);
            Ref<MQTT::Message> msg = GetMessage(message);
            if(msg)
                callback(msg);
            
            slice = slice.sub(totalLength);
            
            totalLength = TotalLength(slice);
        }
        
        copy(slice.cbegin(), slice.cend(), m_Buffer.begin());
        m_Begin = m_Buffer.begin() + slice.size();
    }
    Ref<MQTT::Message> MQTTMessageDecoder::GetMessage(const gsl::span<uint8_t>& data)
    {
        const MQTT::MessageType type = MQTT::GetMessageType(data[0]);
        CachedBuffer dataBuffer = CachedBuffer(data.data(), data.size());
        
        switch (type) {
            case MQTT::MessageType::Connect:
                return Ref<MQTT::ConnectMessage>::Create(dataBuffer);
            case MQTT::MessageType::Publish: {
                return Ref<MQTT::PublishMessage>::Create(dataBuffer);
            }
            default:
                std::cout << fmt::format("Not implemented {}\n", (uint32_t)type);
                return nullptr;
        }
    }
}

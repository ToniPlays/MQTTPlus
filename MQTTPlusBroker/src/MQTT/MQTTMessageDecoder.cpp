#include "MQTTMessageDecoder.h"
#include "MQTT.h"
#include <memory>
#include <span>

#include "spdlog/fmt/fmt.h"

namespace MQTTPlus {
    static int RemainingLength(std::span<uint8_t> bytes) noexcept {
        return bytes[1];
    }

    static int TotalLength(std::span<uint8_t> bytes) noexcept {
        return bytes.size() >= MQTT::FixedHeader::SIZE
        ? RemainingLength(bytes) + MQTT::FixedHeader::SIZE
        : MQTT::FixedHeader::SIZE;
    }

    MQTTMessageDecoder::MQTTMessageDecoder(uint32_t size)
    {
        m_Buffer.reserve(size);
        m_Begin = m_Buffer.begin();
    }

    void MQTTMessageDecoder::DecodeMessage(const std::vector<char> data, OnMessageDecoded&& callback)
    {
        m_Buffer.insert(m_Buffer.end(), data.begin(), data.begin() + data.size());

        
        auto slice = std::span<uint8_t>(m_Buffer.begin(), m_Buffer.begin() + data.size());
        auto totalLength = TotalLength(slice);
    
        
        while(slice.size() >= totalLength)
        {
            const auto message = slice.subspan(0, totalLength);
            Ref<MQTT::Message> msg = GetMessage(message);
            
            if(msg)
                callback(msg);
            
            slice = slice.subspan(totalLength);
            
            totalLength = TotalLength(slice);
        }
        
        copy(slice.begin(), slice.end(), m_Buffer.begin());
        m_Begin = m_Buffer.begin() + slice.size();
    }
    Ref<MQTT::Message> MQTTMessageDecoder::GetMessage(const std::span<uint8_t>& data)
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

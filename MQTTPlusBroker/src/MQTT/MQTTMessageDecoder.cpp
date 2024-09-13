#include "MQTTMessageDecoder.h"
#include "MQTT.h"
#include "Core/Logger.h"
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

    void MQTTMessageDecoder::DecodeMessage(const std::vector<char>& data, OnMessageDecoded&& callback)
    {
        //m_Buffer.insert(m_Buffer.end(), data.begin(), data.begin() + data.size());

        //auto slice = std::span<uint8_t>(m_Buffer.begin(), m_Buffer.begin() + data.size());
        //auto totalLength = TotalLength(slice);
    
        while(true)
        {
            //const auto message = slice.subspan(0, std::span);
            Ref<MQTT::Message> msg = GetMessage(data);
            
            if(msg)
                callback(msg);
            
            //slice = slice.subspan(totalLength);
            
            //totalLength = TotalLength(slice);
            break;
        }
        
        //copy(slice.begin(), slice.end(), m_Buffer.begin());
        //m_Begin = m_Buffer.begin() + slice.size();
    }
    Ref<MQTT::Message> MQTTMessageDecoder::GetMessage(const std::vector<char>& data)
    {
        const MQTT::MessageType type = MQTT::GetMessageType(data[0]);
        CachedBuffer dataBuffer = CachedBuffer((void*)data.data(), data.size());

        try {
            switch (type) {
                case MQTT::MessageType::Connect:
                    return Ref<MQTT::ConnectMessage>::Create(dataBuffer);
                case MQTT::MessageType::Publish:
                    return Ref<MQTT::PublishMessage>::Create(dataBuffer);
                case MQTT::MessageType::Subscribe:
                    return Ref<MQTT::SubscribeMessage>::Create(dataBuffer);
                default:
                    std::cout << fmt::format("Not implemented {}\n", (uint32_t)type);
                    return nullptr;
            }
        } catch(MQTTPlusException e)
        {
            MQP_ERROR("Failed to decode message with type: {0} {1}", (uint32_t)type, e.what());
            return nullptr;
        } catch(...)
        {
            MQP_FATAL("Unknown error while decoding message");
            return nullptr;
        }
    }
}

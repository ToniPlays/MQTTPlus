#pragma once
#include "MQTT.h"
#include <iostream>
#include <vector>
#include <functional>
#include "Ref.h"

#include <gsl.h>

namespace MQTTPlus 
{
    using OnMessageDecoded = std::function<void(Ref<MQTT::Message>&)>;
    class MQTTMessageDecoder
    {
    public:
        MQTTMessageDecoder(uint32_t size = 500);
        ~MQTTMessageDecoder() = default;
        
        auto Begin() noexcept { return m_Begin; }
        auto ReadableData() noexcept { return m_Buffer.data() + std::distance(m_Buffer.begin(), m_Begin); }
        auto ReadableSize() noexcept { return std::distance(m_Buffer.begin(), m_Begin); }
        
        void DecodeMessage(char* data, int length, OnMessageDecoded&& callback);
        Ref<MQTT::Message> GetMessage(const gsl::span<uint8_t>& data);
    private:
        std::vector<uint8_t> m_Buffer;
        decltype(m_Buffer)::iterator m_Begin;
    };
}

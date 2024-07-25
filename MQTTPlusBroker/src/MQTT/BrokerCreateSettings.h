#pragma once

#include <iostream>
#include <cstdint>

namespace MQTTPlus {
    struct BrokerCreateSettings
    {
        uint32_t Port = 0;
        bool UseSSL = false;
    };
}

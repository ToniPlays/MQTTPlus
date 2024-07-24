#pragma once

#include <iostream>

namespace MQTTPlus {
    struct BrokerCreateSettings
    {
        uint32_t Port = 0;
        bool UseSSL = false;
    };
}

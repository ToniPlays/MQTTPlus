#pragma once

#include <spdlog/spdlog.h>

#define MQP_INFO(...) ::MQTTPlus::Logger::Get()->info(__VA_ARGS__)
#define MQP_WARN(...) ::MQTTPlus::Logger::Get()->warn(__VA_ARGS__)
#define MQP_ERROR(...) ::MQTTPlus::Logger::Get()->error(__VA_ARGS__)
#define MQP_FATAL(...) ::MQTTPlus::Logger::Get()->critical(__VA_ARGS__)
#define MQP_TRACE(...) ::MQTTPlus::Logger::Get()->trace(__VA_ARGS__)
#define MQP_ASSERT(success, ...) if(!success) {::MQTTPlus::Logger::Get()->info(__VA_ARGS__); assert(false); }


namespace MQTTPlus {

    using SPLogger = std::shared_ptr<spdlog::logger>;

    class Logger {
    public: 
        static void Init();
        static SPLogger Get() { return s_Logger; }

    private:
        inline static SPLogger s_Logger;
    };
}
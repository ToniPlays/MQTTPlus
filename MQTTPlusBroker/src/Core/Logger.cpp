#include "Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace MQTTPlus 
{
    void Logger::Init() {
        spdlog::set_pattern("%^[%T.%e] [%n]: %v%$");
        s_Logger = spdlog::stdout_color_mt("MQTTPlus");
        s_Logger->set_level(spdlog::level::trace);
    }
}
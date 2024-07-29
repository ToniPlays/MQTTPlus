#pragma once

#include <string>
#include <filesystem>

namespace MQTTPlus
{
    class File {
        public:
        static std::string ReadFile(const std::filesystem::path& path);

        static bool Exists(const std::filesystem::path& path);
    };
}
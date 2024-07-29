#include "File.h"

namespace MQTTPlus
{
    std::string File::ReadFile(const std::filesystem::path& path)
    {
        return "";
    }

    bool File::Exists(const std::filesystem::path& path)
    {
        return std::filesystem::exists(path);
    }
}
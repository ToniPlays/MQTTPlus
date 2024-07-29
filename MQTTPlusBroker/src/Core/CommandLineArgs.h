#pragma once

#include "StringUtility.h"
#include "File.h"


#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <sstream>
#include "Logger.h"

namespace MQTTPlus
{
    class CommandLineArgs {
    public:
        static void Init(int argc, char* argv[])
        {
            MQP_INFO("Working directory {}", std::filesystem::current_path().string());
            std::stringstream ss;
            for(uint32_t i = 0; i < argc; i++)
                ss << argv[i] << " ";

            ProcessArgs(ss.str());

            auto confFile = Get<std::filesystem::path>("c");
            MQP_INFO(confFile.string());
            if(File::Exists(confFile))
            {
                MQP_INFO("Reading config file {}", confFile.string());

                ProcessArgs(File::ReadFile(confFile));
            }
        }
        template<typename T>
        static T Get(const std::string& key) { assert(false); }
        template<>
        static std::string Get(const std::string& key)
        {
            return s_Args[key];
        }
        template<>
        static std::filesystem::path Get(const std::string& key)
        {
            return std::filesystem::path(s_Args[key]).lexically_normal();
        }
        template<>
        static int Get(const std::string& key)
        {
            return atoi(s_Args[key].c_str());
        }
        

    private:
        static void ProcessArgs(const std::string& value)
        {
            std::vector<std::string> args = StringUtility::SplitString(value, '-');
            for(auto& arg : args)
            {
                auto pos = arg.find_first_of(' ');
                if(pos == std::string::npos) continue;

                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                s_Args[key] = value.substr(0, value.length() - 1);
            }
        }


    private:
        inline static std::unordered_map<std::string, std::string> s_Args;
    };
}
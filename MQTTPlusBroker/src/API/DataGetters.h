#pragma once

#include "MQTTPlusAPI.h"
#include "Core/StringUtility.h"
#include "Core/Service/ServiceManager.h"
#include "Database/DatabaseService.h"

namespace MQTTPlus::API
{
    static std::vector<std::string> GetObjectExpandOpts(const std::vector<std::string>& opts)
    {
        std::vector<std::string> results;
        for(auto& opt : opts)
        {
            if(opt.find_first_of('.') != std::string::npos)
                results.emplace_back(opt.substr(opt.find_first_of('.') + 1));
        }
        return results;
    }
    static std::vector<std::string> GetExpandOpts(const std::vector<std::string>& opts)
    {
        std::vector<std::string> results;

        for(auto& opt : opts)
        {
            std::string string = opt.substr(0, opt.find_first_of('.'));
            if(string.length() == 0) continue;
            if(opt.length() == string.length()) continue;
            if(opt.find_first_of('.', string.length() + 1) != std::string::npos) continue;
            
            results.push_back(opt.substr(string.length() + 1));
        }

        return results;
    }
    static std::vector<std::string> ExpandOpts(const nlohmann::json& json)
    {
        if(!json.contains("opts")) return {};
        if(!json["opts"].contains("expands")) return {};

        if(!json["opts"]["expands"].is_null())
            return json["opts"]["expands"].get<std::vector<std::string>>();
        return {};
    }
}
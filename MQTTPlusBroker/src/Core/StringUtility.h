#pragma once

#include <string>
#include <vector>

namespace MQTTPlus 
{
    class StringUtility {
    public:
        static std::vector<std::string> SplitString(const std::string& string, char delim)
        {
            std::vector<std::string> result;
            std::istringstream f(string);
            std::string s;
            while(getline(f, s, delim))
            {
                if(!s.empty())
                    result.push_back(s);
            }

            return result;
        }
    };
}
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
        static std::string GetPreprocessor(const char* type, const std::string& source, size_t endPosition, size_t* offset)
        {
            size_t valueOffset = OffsetOf(source, type, endPosition);
            if (valueOffset == std::string::npos) {
                *offset = std::string::npos;
                return "";
            }
            size_t lineEndOffset = source.find_first_of('\r\n', valueOffset);
            std::string line = source.substr(valueOffset, lineEndOffset - valueOffset);
            *offset = valueOffset + line.length();
            return line.substr(line.find_first_of(' ') + 1);
        }
        static size_t OffsetOf(const std::string& source, char delim, size_t offset)
        {
            std::string offsetValue = source;

            for (uint32_t i = 0; i < offset; i++) {
                offsetValue = offsetValue.substr(offsetValue.find_first_of(delim) + 1);
            }
            return offsetValue.find_first_of(delim, offset);
        }

        static size_t OffsetOf(const std::string& source, const std::string& value, size_t startOffset) 
        {
            return source.find(value, startOffset);
        }
    };
}
#pragma once

#include <string>
#include <vector>
#include <random>

#include <iostream>
#include <iomanip>
#include <sstream>

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
            
            size_t lineEndOffset = source.find_first_of('\n', valueOffset);
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

        static std::string_view Between(const std::string_view& source, const std::string& start, const std::string& end, uint64_t offset)
        {
            size_t startPos = source.find(start, offset);
            size_t endPos = source.find(end, startPos + 1);

            return source.substr(startPos + start.length(), endPos - startPos - end.length());
        }
        static std::string Hex16() {
            // Create a random number generator
            std::random_device rd;                             // Obtain a random number from hardware
            std::mt19937 rng(rd());                            // Seed the generator
            std::uniform_int_distribution<std::mt19937::result_type> dist(0, 0xF); // Define the range (0 to 15 for hexadecimal)

            // Create a stringstream to build the hexadecimal string
            std::stringstream ss;

            // Generate 32 random hex digits
            for (int i = 0; i < 16; ++i) {
                // Get a random value and convert it to hexadecimal
                int randomHexDigit = dist(rng);
                ss << std::hex << std::setw(1) << std::setfill('0') << randomHexDigit;
            }

            // Return the resulting string
            return ss.str();
        }
    };
}
#pragma once

#include <string.h>
#include <vector>

namespace MQTTPlus
{
#define EVENT_CLASS_TYPE(type) static const char* GetStaticName() {return #type; } \
								virtual const char* GetName() const override { return #type; }

    class Event
    {
    public:
        virtual ~Event() = default;
        virtual const char* GetName() const = 0;
        virtual std::string ToString() const = 0;

        template<typename E>
        static bool Is(Event& e)
        {
            return e.GetName() == E::GetStaticName();
        }
    };
    
    template<typename T>
    static bool Contains(const std::vector<T>& arr, const T& value)
    {
        return std::find(arr.begin(), arr.end(), value) != arr.end();
    }

    template<>
    bool Contains(const std::vector<std::string>& arr, const std::string& value)
    {
        for(auto& s : arr)
        {
            if(strcmp(s.c_str(), value.c_str()) == 0)
                return true;
        }

        return false;
    }

}
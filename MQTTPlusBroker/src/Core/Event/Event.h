#pragma once

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
    
}
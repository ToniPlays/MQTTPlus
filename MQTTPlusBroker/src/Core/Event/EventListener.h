#pragma once

#include "Core/Event/Event.h"

#include <iostream>
#include <unordered_map>
#include <functional>

namespace MQTTPlus
{
    using ListenCallback = std::function<void(Event&)>;
    class EventListener {
    public:
        EventListener() = default;
        ~EventListener() = default;

        void Invoke(Event& e) {
            uint64_t hash = std::hash<std::string>{}(e.GetName());
            auto it = m_ListenerCallbacks.find(hash);

            if(it != m_ListenerCallbacks.end())
                m_ListenerCallbacks[hash](e);
        }

        void AddListener(const std::string& type, const ListenCallback& callback)
        {
            uint64_t hash = std::hash<std::string>{}(type);
            m_ListenerCallbacks[hash] = callback;
        }

        void Clear() {
            m_ListenerCallbacks.clear();
        }

    private:
        std::unordered_map<uint64_t, ListenCallback> m_ListenerCallbacks;
    };
} // namespace MQTTPlus

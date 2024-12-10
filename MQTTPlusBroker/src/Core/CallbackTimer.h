#pragma once

#include "Ref.h"
#include "Threading/JobSystem.h"
#include "Core/Logger.h"
#include <chrono>

namespace MQTTPlus {
    
    struct CallbackEvent
    {
        uint64_t NextMillis = 0;
        uint64_t Interval = 1000;
        std::function<void()> Callback;
    };
    
    class CallbackTimer : public RefCount
    {
    public:
        CallbackTimer() = default;
        ~CallbackTimer() {
            m_Running = false;
            m_Running.notify_one();
            
        };
        
        void Start() {
            m_Running = true;
            m_Thread = Ref<Thread>::Create("CallbackTimer", [instance = this]() {
                CallbackTimer::TimerFunc(instance);
            });
            
            m_Running.notify_one();
        }
        
        void Clear() {
            m_Mutex.lock();
            m_Events.clear();
            m_Mutex.unlock();
        }
        
        void AddCallback(float interval, const std::function<void()>& callback) {
            using namespace std::chrono;
            
            milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            uint64_t nextMillis = (now + milliseconds((int)(interval * 1000.0f))).count();
            
            m_Mutex.lock();
            m_Events.push_back({ nextMillis, (uint64_t)(interval * 1000.0f), callback });
            m_TimedEventCount = m_Events.size();
            m_Mutex.unlock();
            
            m_TimedEventCount.notify_one();
        }
        
    private:
        static void TimerFunc(CallbackTimer* timer)
        {
            using namespace std::chrono;
            while(timer->m_Running)
            {
                timer->m_TimedEventCount.wait(0);
                
                uint64_t currentMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                uint64_t nextMillis = UINT64_MAX;
                timer->m_Mutex.lock();
                
                for(auto& cb : timer->m_Events)
                {
                    if(cb.NextMillis <= currentMillis)
                    {
                        cb.NextMillis += cb.Interval;
                        cb.Callback();
                    } 

                    if(cb.NextMillis < nextMillis)
                        nextMillis = cb.NextMillis;
                }
                
                timer->m_Mutex.unlock();
                int64_t sleepMillis = std::max((int64_t)10, (int64_t)nextMillis - (int64_t)currentMillis);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepMillis));
            }
        }
        
    private:
        Ref<Thread> m_Thread;
        std::mutex m_Mutex;
        std::atomic_bool m_Running = false;
        std::vector<CallbackEvent> m_Events;
        std::atomic<uint64_t> m_TimedEventCount;
    };
}

#pragma once

#include "Ref.h"
#include "Threading/Thread.h"
#include "Core/Logger.h"
#include <chrono>

namespace MQTTPlus {
    
    struct CallbackEvent
    {
        uint64_t NextMillis;
        uint64_t Interval;
        std::function<void()> Callback;
    };
    
    class CallbackTimer : public RefCount
    {
    public:
        CallbackTimer() = default;
        ~CallbackTimer() {
            m_Running = false;
            m_Running.notify_all();
            
        };
        
        void Start() {
            m_Running = true;
            m_Running.notify_all();
            m_Thread = Ref<Thread>::Create(std::thread(CallbackTimer::TimerFunc, this));
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
            m_Events.push_back({ nextMillis, (uint64_t)interval * 1000, callback });
            m_TimedEventCount = m_Events.size();
            m_Mutex.unlock();
            
            m_TimedEventCount.notify_all();
        }
        
    private:
        static void TimerFunc(CallbackTimer* timer)
        {
            using namespace std::chrono;
            while(timer->m_Running)
            {
                timer->m_TimedEventCount.wait(0);
                
                uint64_t nextMillis = UINT64_MAX;
                uint64_t currentMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                timer->m_Mutex.lock();
                for(auto& cb : timer->m_Events)
                {
                    int64_t diff = cb.NextMillis - currentMillis;
                    if(diff < 0)
                    {
                        cb.NextMillis += cb.Interval;
                        cb.Callback();
                    } else if(cb.NextMillis < nextMillis)
                    {
                        nextMillis = cb.NextMillis;
                    }
                }
                
                timer->m_Mutex.unlock();
                uint64_t sleepMillis = nextMillis - currentMillis;
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            }
            MQP_WARN("Timer stopped");
        }
        
    private:
        Ref<Thread> m_Thread;
        std::mutex m_Mutex;
        std::atomic_bool m_Running = false;
        std::vector<CallbackEvent> m_Events;
        std::atomic<uint64_t> m_TimedEventCount;
    };
}
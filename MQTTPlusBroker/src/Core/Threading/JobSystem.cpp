#include "JobSystem.h"
#include "Core/Logger.h"
#include "Core/MQTTPlusException.h"

#include <spdlog/fmt/fmt.h>

namespace MQTTPlus 
{
    void Thread::Execute(const std::function<void(Ref<Thread>)>& callback)
    {
        if (callback == nullptr) return;

        m_Status = ThreadStatus::Executing;
        try
        {
            callback(this);
            m_Status = ThreadStatus::Finished;
            MQP_ERROR("{}: Finished", m_DebugName);
        }
        catch (MQTTPlusException e)
        {
            MQP_ERROR("{}: MQTT error {}", m_DebugName, e.what());
            m_Status = ThreadStatus::Failed;
        }
        catch (std::exception e)
        {
            MQP_ERROR("{}: Unknown error {}", m_DebugName, e.what());
            m_Status = ThreadStatus::Failed;
        }
    }

    JobSystem::JobSystem(uint32_t threads) : m_Running(true)
    {
        m_Threads.resize(threads);

        for (uint32_t i = 0; i < threads; i++)
        {
            m_Threads[i] = Ref<Thread>::Create(i);
            m_Threads[i]->m_Thread = new std::thread(&JobSystem::ThreadFunc, this, m_Threads[i]);
        }
    }
    JobSystem::~JobSystem()
    {
        m_Running = false;
        for (auto& thread : m_Threads)
            thread->Join();
    }

    void JobSystem::ThreadFunc(Ref<Thread> thread)
    {
        //Initialize thread state
        thread->m_Status = ThreadStatus::Waiting;

        while (m_Running)
        {
            m_JobCount.wait(0);
            
            m_Mutex.lock();
            auto callback = m_QueuedJobs.front();
            m_QueuedJobs.pop();
            m_JobCount = m_QueuedJobs.size();
            m_Mutex.unlock();

            m_JobCount.notify_one();

            if(!callback) continue;
            
            thread->Execute(callback);
        }

        thread->m_Status = ThreadStatus::Terminated;
        thread->m_Status.notify_all();

        std::string msg = fmt::format("Thread {} terminated", thread->GetThreadID());
        SendMessage(msg);
    }


    void JobSystem::WaitForJobsToFinish()
    {
        while (m_JobCount != 0)
            m_JobCount.wait(m_JobCount);

        for (auto& thread : m_Threads)
            thread->WaitForIdle();
    }
    void JobSystem::Terminate()
    {
        m_Running = false;
        m_JobCount = 1;
        m_JobCount.notify_all();
    }

    uint64_t JobSystem::WaitForUpdate()
    {
        if (m_JobCount != 0)
            m_JobCount.wait(m_JobCount);
        if (m_RunningJobCount != 0)
            m_RunningJobCount.wait(m_RunningJobCount);

        return m_JobCount;
    }
}
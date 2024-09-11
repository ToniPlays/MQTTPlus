#include "JobSystem.h"
#include "Core/Logger.h"

#include <spdlog/fmt/fmt.h>

namespace MQTTPlus 
{
    void Thread::Execute(const std::function<void()>& callback)
    {
        if (callback == nullptr) return;

        m_Status = ThreadStatus::Executing;
        try
        {
            callback();
            m_Status = ThreadStatus::Finished;
        }
        catch (std::exception e)
        {
            m_Status = ThreadStatus::Failed;
            throw e;
        }
    }

    JobSystem::JobSystem(uint32_t threads) : m_Running(true)
    {
        m_Threads.resize(threads);

        for (uint32_t i = 0; i < threads; i++)
        {
            m_Threads[i] = Ref<Thread>::Create(i);
            m_Threads[i]->m_Thread = std::thread(&JobSystem::ThreadFunc, this, m_Threads[i]);
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
        MQP_WARN("Starting thread {}", thread->GetThreadID());
        //Initialize thread state
        thread->m_Status = ThreadStatus::Waiting;

        while (m_Running)
        {
            m_JobCount.wait(0);
            m_Mutex.lock();
            auto callback = m_QueuedJobs[0];
            m_QueuedJobs.erase(m_QueuedJobs.begin());
            m_JobCount = m_QueuedJobs.size();
            m_Mutex.unlock();

            m_JobCount.notify_one();

            if(!callback) continue;

                thread->Execute(callback);
            try 
            {
            } catch(std::exception e)
            {
                MQP_FATAL("Thread crashed with {}", e.what());
            } catch(...)
            {
                
            }
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
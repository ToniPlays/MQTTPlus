#pragma once

#include <Ref.h>
#include <functional>
#include "JobException.h"
#include "JobFlags.h"
#include "Core/Callback.h"
#include "Core/Coroutine.h"
#include <optional>

namespace MQTTPlus 
{
    class JobGraph;
    class Thread;
    struct JobInfo;

    class Job : public RefCount
    {
        friend class JobSystem;
        friend class JobGraph;
        using JobCallback = std::function<Coroutine(JobInfo)>;

    public:
        
        ~Job() 
        {
            if(m_ResultBuffer)
                delete m_ResultBuffer;
        }
        
        const std::string& GetName() const { return m_JobName; }
        JobStatus GetStatus() const { return m_Status; }
        const Coroutine& GetCoroutine() const { return m_JobCoroutine; }

        void Execute(JobInfo info);
        void Progress(float progress);

        float GetExecutionTime() const { return m_ExecutionTime; }
        float GetProgress() const { return m_Progress; }
        
        std::optional<JobException> GetException() const { return m_Exception; }
        
        template<typename T>
        void SetResult(T value)
        {
            m_ResultBuffer = (void*)new T(value);
        }
        template<typename T>
        T GetResult() const
        {
            if(!m_ResultBuffer)
                return T();
            return *(T*)m_ResultBuffer;
        }
        
    public:
        template<typename Fn, typename... Args>
        static Ref<Job> Create(const std::string& name, Fn&& callback, Args&&... args)
        {
            return new Job(name, callback, args...);
        }
        
        static Ref<Job> Lambda(const std::string& name, JobCallback&& callback)
        {
            Ref<Job> job = Create();
            job->m_JobName = name;
            job->m_JobCallback = callback;
            return job;
        }
        static Ref<Job> Create()
        {
            return new Job();
        }
        
    private:
        Job() = default;
        
        template<typename Fn, typename... Args>
        Job(const std::string& name, Fn&& callback, Args&&... args) : m_JobName(name)
        {
            m_JobCallback = std::bind(std::forward<Fn>(callback), std::placeholders::_1, std::forward<Args>(args)...);
        }

    private:
        Coroutine m_JobCoroutine;
        JobCallback m_JobCallback;
        std::string m_JobName;

        std::atomic<float> m_Progress = 0.0f;
        std::atomic<float> m_ExecutionTime = 0.0f;
        
        uint32_t m_InvocationId = 0;
        JobGraph* m_JobGraph = nullptr;
        void* m_ResultBuffer = nullptr;

        JobStatus m_Status = JobStatus::None;
        std::optional<JobException> m_Exception;
    };

    struct JobInfo
    {
        friend class Job;

        Ref<Job> Current;
        Ref<JobGraph> Graph;
        uint32_t StageIndex;
        uint32_t ExecutionID;
        Thread* Thread;
        
        template<typename T>
        void Result(T value)
        {
            Current->SetResult<T>(value);
        };

        void ContinueWith(const std::vector<Ref<Job>>& jobs);
    };
}
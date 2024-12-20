#pragma once
#include <Ref.h>
#include <thread>
#include "Job.h"
#include "JobFlags.h"
#include "Core/Hooks.h"
#include "Core/Severity.h"
#include "Promise.h"

#include "spdlog/fmt/fmt.h"

namespace MQTTPlus 
{
	class JobGraph;

	class Thread : public RefCount
	{
		friend class JobSystem;
	public:
		
		Thread(uint32_t id) : m_ThreadID(id) {};
		~Thread() = default;
		
		uint32_t GetThreadID() const { return m_ThreadID; }
		bool IsWaiting() const { return m_Status.load() == ThreadStatus::Waiting; }
		ThreadStatus GetStatus() const { return m_Status; }
		Ref<Job> GetCurrentJob() const { return m_CurrentJob; }
		const std::string& GetLastError() const { return m_LastError; }

		void Join() { m_Thread.join(); };
		void Detach() { m_Thread.detach(); };
		void WaitForIdle() { m_Status.wait(ThreadStatus::Executing); }
		void Wait(ThreadStatus status) { m_Status.wait(status); }
		
	private:
		void Execute(JobSystem* system, Ref<Job> job);

	private:
		std::thread m_Thread;
		uint32_t m_ThreadID = 0;
		Ref<Job> m_CurrentJob = nullptr;
		std::string m_LastError;
		std::atomic<ThreadStatus> m_Status = ThreadStatus::Waiting;
	};

	enum class JobSystemHook
	{
		Submit = 0,
		Status,
		Finished,
		Failure,
		Message
	};
	class JobSystem
	{
		friend class Thread;
		friend class JobGraph;

	public:
		JobSystem(uint32_t threads = std::thread::hardware_concurrency());
		~JobSystem();

		const std::vector<Ref<Thread>>& GetThreads() const { return m_Threads; }
		std::vector<Ref<JobGraph>> GetQueuedGraphs() const { return m_QueuedGraphs; }

		const uint64_t GetJobCount() const { return m_JobCount; }

		void WaitForJobsToFinish();
		void Terminate();
		
		template<typename T>
		Promise<T> Submit(Ref<JobGraph> graph)
		{
			if (!graph)
			{
				std::string msg = "Tried submitting nullptr, aborting";
				SendMessage(Severity::Warning, msg);
				return Promise<T>();
			}
				
			if (!graph->SubmitJobs(this))
			{
				
				m_HookCallbacks.Add([this, graph]() mutable {
					std::string message = fmt::format("Failed to submit graph {0}: No starting jobs specified", graph->GetName());
					m_MessageHook.Invoke(Severity::Error, message);
				});
				m_RunHooks = true;
				m_RunHooks.notify_all();

				return Promise<T>();
			}

			m_GraphMutex.lock();
			m_QueuedGraphs.push_back(graph);
			m_HookCallbacks.Add([this, graph]() mutable {
				m_Hooks.Invoke(JobSystemHook::Submit, graph);
			});

			m_RunHooks = true;
			m_RunHooks.notify_all();
			
			m_GraphMutex.unlock();
			
			return Promise<T>(graph);
		}
		
		template<typename T>
		Promise<T> Submit(Ref<Job> job)
		{
			JobGraphInfo info = {
				.Name = job->GetName(),
				.Stages = { { "Run", 1.0f, { job } } }
			};
			
			return Submit<T>(Ref<JobGraph>::Create(info));
		}
		
		uint64_t WaitForUpdate();

		void WaitForHooks()
		{
			m_RunHooks.wait(false);
		}

		void Update() 
		{
			m_HookCallbacks.Invoke();
			m_HookCallbacks.Clear();
			m_RunHooks = false;
			m_RunHooks.notify_all();
		}

		void Hook(JobSystemHook hook, const std::function<void(Ref<JobGraph>)>& callback)
		{
			m_Hooks.AddHook(hook, callback);
		}
		void Hook(const std::function<void(Ref<Thread>, ThreadStatus)>& callback)
		{
			m_StatusHook.Add(callback);
		}
		void Hook(const std::function<void(Severity, const std::string&)>& callback)
		{
			m_MessageHook.Add(callback);
		}

	private:
		bool QueueJobs(const std::vector<Ref<Job>>& jobs);
		void RemoveJob(Ref<Job> job);
		void AddJob(Ref<Job> job);
		void TerminateGraphJobs(Ref<JobGraph> graph);
		void OnGraphFinished(Ref<JobGraph> graph);

		void SendMessage(Severity severity, const std::string& message)
		{
			std::scoped_lock lock(m_MessageMutex);
			m_HookCallbacks.Add([this, msg = message, severity]() mutable {
				m_MessageHook.Invoke(severity, msg);
			});

			m_RunHooks = true;
			m_RunHooks.notify_all();
		}

		Ref<Job> FindAvailableJob();
		void ThreadFunc(Ref<Thread> thread);

	private:
		std::vector<Ref<JobGraph>> m_QueuedGraphs;
		std::vector<Ref<Thread>> m_Threads;
		std::vector<Ref<Job>> m_Jobs;

		std::atomic_bool m_Running = false;
		std::atomic_bool m_RunHooks = false;

		std::atomic_uint64_t m_JobCount = 0;

		std::mutex m_JobMutex;
		std::mutex m_RunningJobMutex;
		std::mutex m_GraphMutex;
		std::mutex m_MessageMutex;
		
		Hooks<JobSystemHook, void(Ref<JobGraph>)> m_Hooks;
		Callback<void(Severity, const std::string&)> m_MessageHook;
		Callback<void(Ref<Thread>, ThreadStatus)> m_StatusHook;
		Callback<void()> m_HookCallbacks;
	};

}
#pragma once

#include <Ref.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <queue>
#include "ThreadStatus.h"
#include "Core/Logger.h"

#include "Core/Hooks.h"

namespace MQTTPlus 
{
	class Thread : public RefCount
	{
		friend class JobSystem;
	public:

		Thread(uint32_t id) : m_ThreadID(id), m_DebugName(fmt::format("Thread {}", id)) {};
		Thread(const std::string& name, const std::function<void()>& callback) {
			m_DebugName = name;
			m_Thread = new std::thread([this, name, callback]() {
				Execute([callback](Ref<Thread> t) { callback(); });
			});
		}
		~Thread() = default;

		uint32_t GetThreadID() const { return m_ThreadID; }
		bool IsWaiting() const { return m_Status.load() == ThreadStatus::Waiting; }
		ThreadStatus GetStatus() const { return m_Status; }
		const std::string& GetDebugName() const { return m_DebugName; }

		void Join() { m_Thread->join(); };
		void Detach() { m_Thread->detach(); };
		void WaitForIdle() { m_Status.wait(ThreadStatus::Executing); }

	private:
		void Execute(const std::function<void(Ref<Thread>)>& callback);

	private:
		std::thread* m_Thread;
		std::string m_DebugName;
		uint32_t m_ThreadID = 0;
		std::atomic<ThreadStatus> m_Status = ThreadStatus::Waiting;
	};

	class JobSystem
	{
		friend class Thread;
		friend class JobGraph;

	public:
		enum JobSystemHook
		{
			Submit = 0,
			Status,
			Finished,
			Failure,
			Message
		};

	public:
		JobSystem(uint32_t threads = std::thread::hardware_concurrency());
		~JobSystem();

		const std::vector<Ref<Thread>>& GetThreads() const { return m_Threads; }

		void WaitForJobsToFinish();
		void Terminate();

		bool Queue(const std::function<void(Ref<Thread>)>& callback) 
		{
			m_Mutex.lock();
			m_QueuedJobs.push(callback);
			m_JobCount = m_QueuedJobs.size();
			m_Mutex.unlock();
			m_JobCount.notify_all();
			return true;
		};
		
		uint64_t WaitForUpdate();

		void Hook(JobSystemHook hook, const std::function<void()>& callback)
		{
			m_Hooks.AddHook(hook, callback);
		}
		void Hook(JobSystemHook hook, const std::function<void(Ref<Thread>, ThreadStatus)>& callback)
		{
			m_StatusHook.Add(callback);
		}
		void Hook(JobSystemHook hook, const std::function<void(const std::string&)>& callback)
		{
			m_MessageHook.Add(callback);
		}

	private:

		void SendMessage(const std::string& message)
		{
			std::scoped_lock lock(m_MessageMutex);
			m_MessageHook.Invoke(message);
		}

		void ThreadFunc(Ref<Thread> thread);

	private:
		std::queue<std::function<void(Ref<Thread>)>> m_QueuedJobs;

		std::vector<Ref<Thread>> m_Threads;
		std::atomic_bool m_Running = false;

		std::atomic_uint64_t m_JobCount = 0;
		std::atomic_uint64_t m_RunningJobCount = 0;

		std::mutex m_Mutex;
		std::mutex m_MessageMutex;
		
		Hooks<JobSystemHook, void()> m_Hooks;
		Callback<void(const std::string&)> m_MessageHook;
		Callback<void(Ref<Thread>, ThreadStatus)> m_StatusHook;
	};
}

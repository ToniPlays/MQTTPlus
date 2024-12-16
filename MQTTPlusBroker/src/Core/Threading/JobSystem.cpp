#include "JobSystem.h"
#include "JobGraph.h"

#include <spdlog/fmt/fmt.h>
#include "Core/Timer.h"

namespace MQTTPlus
{
	void Thread::Execute(JobSystem* system, Ref<Job> job)
	{
		if (job == nullptr) return;

		m_CurrentJob = job;

		try
		{
			m_LastError = "";
			JobInfo info = {};
			info.Thread = this;
			info.Current = job;

			m_Status = ThreadStatus::Executing;
			job->Execute(info);
			m_Status = ThreadStatus::Finished;
		}
		catch (JobException e)
		{
			m_Status = ThreadStatus::Failed;
			m_LastError = e.what();
			throw e;
		}
		catch(std::exception_ptr e)
		{
			m_Status = ThreadStatus::Failed;
			m_LastError = "Something broke";
			throw std::exception();
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
		//Initialize thread state
		while (m_Running)
		{
			thread->m_Status = ThreadStatus::Waiting;
			thread->m_Status.notify_all();

			//Wait if no jobs
			m_JobCount.wait(0);
			if (!m_Running) break;

			Ref<Job> job;
			{
				std::scoped_lock lock(m_JobMutex);
				job = FindAvailableJob();

				if (!job)
					continue;

				RemoveJob(job);
			}

			try
			{
				//Execute job
				thread->m_CurrentJob = job;
				m_StatusHook.Invoke(thread, ThreadStatus::Executing);
				m_RunHooks = true;
				m_RunHooks.notify_all();
				thread->Execute(this, job);
				m_StatusHook.Invoke(thread, ThreadStatus::Finished);
				m_RunHooks = true;
				m_RunHooks.notify_all();

			}
			catch (JobException e)
			{
				std::string msg = fmt::format("JobException: {0} {1}", job->GetName(), e.what());
				SendMessage(Severity::Error, msg);
				m_StatusHook.Invoke(thread, thread->GetStatus());
			}

			if(!job->GetCoroutine().Done())
			{
				std::scoped_lock lock(m_JobMutex);
				AddJob(job);
			}

			thread->m_CurrentJob = nullptr;
		}

		thread->m_Status = ThreadStatus::Terminated;
		thread->m_Status.notify_all();

		std::string msg = fmt::format("Thread {} terminated", thread->GetThreadID());
		SendMessage(Severity::Info, msg);
	}

	bool JobSystem::QueueJobs(const std::vector<Ref<Job>>& jobs)
	{
		if (jobs.size() == 0) return false;

		std::scoped_lock lock(m_JobMutex);
		m_Jobs.insert(m_Jobs.end(), jobs.begin(), jobs.end());
		m_JobCount = m_Jobs.size();

		m_JobCount.notify_all();

		std::string msg = fmt::format("Queued {} jobs", jobs.size());
		SendMessage(Severity::Info, msg);
		return true;
	}

	void JobSystem::RemoveJob(Ref<Job> job)
	{
		m_Jobs.erase(std::find(m_Jobs.begin(), m_Jobs.end(), job));
		m_JobCount = m_Jobs.size();
		m_JobCount.notify_one();
	}

	void JobSystem::AddJob(Ref<Job> job)
	{
		m_Jobs.push_back(job);
		m_JobCount = m_Jobs.size();
		m_JobCount.notify_one();
	}

	void JobSystem::TerminateGraphJobs(Ref<JobGraph> graph)
	{
		m_JobMutex.lock();
		std::vector<Ref<Job>> jobs;
		jobs.reserve(m_JobCount);

		for (auto& job : m_Jobs)
		{
			if (job->m_JobGraph != graph)
				jobs.emplace_back(job);
		}

		m_Jobs = jobs;
		m_JobMutex.unlock();
	}

	void JobSystem::OnGraphFinished(Ref<JobGraph> graph)
	{
		m_GraphMutex.lock();

		auto it = std::find(m_QueuedGraphs.begin(), m_QueuedGraphs.end(), graph);
		if (it != m_QueuedGraphs.end())
			m_QueuedGraphs.erase(it);

		m_GraphMutex.unlock();
		m_HookCallbacks.Add([this, graph]() mutable {
			if (graph->DidFail())
				m_Hooks.Invoke(JobSystemHook::Failure, graph);
			else 
				m_Hooks.Invoke(JobSystemHook::Finished, graph);
		});

		m_RunHooks = true;
		m_RunHooks.notify_all();
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

		return m_JobCount;
	}

	Ref<Job> JobSystem::FindAvailableJob()
	{
		for (auto& job : m_Jobs)
		{
			if (job->GetCoroutine().CanContinue())
				return job;
		}
		return nullptr;
	}
}
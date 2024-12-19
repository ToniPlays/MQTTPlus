#include "Job.h"
#include "JobGraph.h"
#include "Core/Timer.h"

#include "spdlog/fmt/fmt.h"

namespace MQTTPlus 
{
	void Job::Execute(JobInfo info)
	{
		info.Graph = m_JobGraph;
		info.ExecutionID = m_InvocationId;

		m_Status = JobStatus::Executing;
		Timer timer;

		if (!m_JobCoroutine)
			m_JobCoroutine = m_JobCallback(info);

		try
		{
			m_JobCoroutine.MoveNext();
			if (m_JobCoroutine.Done())
			{
				m_Progress = 1.0f;
				m_ExecutionTime = timer.ElapsedMillis();
				m_Status = JobStatus::Success;

				if (m_JobGraph)
					m_JobGraph->OnJobFinished(this);
			}
		}
		catch (JobException e)
		{
			m_Status = JobStatus::Failure;
			m_ExecutionTime = timer.ElapsedMillis();
			m_Exception = e;
			if (m_JobGraph)
				m_JobGraph->OnJobFailed(this);
			throw e;
		}
		catch(std::exception e)
		{
			throw JobException("std::exception");
		}
	}

	void Job::Progress(float progress)
	{
		m_Progress = progress;
		m_Progress.notify_all();
	}

	void JobInfo::ContinueWith(const std::vector<Ref<Job>>& jobs)
	{
		Graph->ContinueWith(jobs);
	}
}
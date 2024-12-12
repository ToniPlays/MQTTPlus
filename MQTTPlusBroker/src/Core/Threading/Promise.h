#pragma once

#include "JobFlags.h"

#include "Ref.h"
#include "Core/Awaitable.h"
#include "JobGraph.h"

namespace MQTTPlus {
		
	class Job;
	class JobSystem;

	template<typename T>
	class Promise : public Awaitable<std::vector<T>>
	{
		friend class JobSystem;

	public:
		Promise() = default;

		const std::string& GetJobName() const
		{
			return m_JobGraph->GetName();
		}

		bool Succeeded() const
		{
			return m_JobGraph->GetFlags() & JOB_GRAPH_SUCCEEDED;
		}

		Promise<T> Wait() const
		{
			if (m_JobGraph)
				m_JobGraph->Wait();
			return *this;
		}

		Promise<T> ContinueWith(const std::function<void(const std::vector<T>&)>& callback) {
			if (m_JobGraph)
				m_JobGraph->AddOnFinished([callback, graph = m_JobGraph]() {
					callback(graph->template GetResults<T>());
					});

			return *this;
		}

		Promise<T> Catch(std::function<void(const JobException&)> callback) {
			if (m_JobGraph)
				m_JobGraph->AddOnFailed(callback);
			return *this;
		}

		std::vector<T> GetResults() override
		{
			if (m_JobGraph)
				return m_JobGraph->GetResults<T>();
			else return std::vector<T>();
		}

		void OnSuspend() override
		{
			if (!m_JobGraph) return;
			
			ContinueWith([instance = this](const auto&) mutable {
				instance->Resolve();
			});
		}

		static Promise<T> Create(Ref<JobGraph> graph)
		{
			return Promise<T>(graph);
		}

	private:
		Promise(Ref<JobGraph> graph) : m_JobGraph(graph) {}

	private:
		Ref<JobGraph> m_JobGraph = nullptr;
	};
}
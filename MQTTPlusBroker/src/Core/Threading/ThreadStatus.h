#pragma once

namespace MQTTPlus
{
    enum class ThreadStatus
    {
        None,
        Terminated,
        Waiting,
        Executing,
        Failed,
        Finished
    };

    enum JobGraphFlags
    {
        JOB_GRAPH_TERMINATE_ON_ERROR = (1 << 0),
        JOB_GRAPH_TERMINATED = (1 << 1),
        JOB_GRAPH_FAILED = (1 << 2),
        JOB_GRAPH_SUCCEEDED = (1 << 3)
    };

    enum class JobStatus
    {
        None = 0,
        Failure = -1,
        Executing = 1,
        Success = 2
    };
}
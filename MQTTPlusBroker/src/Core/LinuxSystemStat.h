#pragma once

#ifdef MQP_LINUX

#include <sys/vfs.h>
#include <iostream>
#include <unistd.h>
#include <sys/sysinfo.h>

namespace MQTTPlus
{
    static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
    // Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
    // You'll need to call this at regular intervals, since it measures the load between
    // the previous call and the current one.
    static float GetCPULoad()
    {
        float f_load = 1.f / (1 << SI_LOAD_SHIFT);
        float nproc = 1.0f / get_nprocs();
        struct sysinfo info;
        sysinfo(&info);
        return info.loads[0] * f_load * nproc;
    }

    static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
    {
        static unsigned long long _previousTotalTicks = 0;
        static unsigned long long _previousIdleTicks = 0;
        
        unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
        unsigned long long idleTicksSinceLastTime  = idleTicks-_previousIdleTicks;
        float ret = 1.0f-((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);
        _previousTotalTicks = totalTicks;
        _previousIdleTicks  = idleTicks;
        return ret;
    }

    // Returns a number between 0.0f and 1.0f, with 0.0f meaning all RAM is available, and 1.0f meaning all RAM is currently in use
    uint64_t GetSystemMemoryAvailable()
    {
        struct sysinfo info;
        sysinfo(&info);

        return info.freeram;
    }

    uint64_t GetAvailableSystemMemory() {
        struct sysinfo info;
        sysinfo(&info);
        return info.totalram;
    }
    
    static std::vector<uint64_t> GetSystemDiskUsage() {
        std::vector<uint64_t> result(2);
        struct statfs stStg;
        statfs(".", &stStg);
        uint64_t total_sz = stStg.f_bsize * stStg.f_blocks;
        uint64_t free_sz = stStg.f_bsize * stStg.f_bfree;
        
        result[0] = total_sz;
        result[1] = total_sz - free_sz;
        
        return result;
    }
}

#endif


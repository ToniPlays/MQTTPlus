#pragma once

#ifdef MQP_MACOS

#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/mount.h>

#include <iostream>

namespace MQTTPlus
{
    static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
    // Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
    // You'll need to call this at regular intervals, since it measures the load between
    // the previous call and the current one.
    static float GetCPULoad()
    {
        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS)
        {
            unsigned long long totalTicks = 0;
            for(int i = 0; i < CPU_STATE_MAX; i++)
                totalTicks += cpuinfo.cpu_ticks[i];
            return CalculateCPULoad(cpuinfo.cpu_ticks[CPU_STATE_IDLE], totalTicks);
        }
        else
            return -1.0f;
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
    static double ParseMemValue(const char * b)
    {
        while((*b)&&(isdigit(*b) == false))
            b++;
        return isdigit(*b) ? atof(b) : -1.0;
    }

    // Returns a number between 0.0f and 1.0f, with 0.0f meaning all RAM is available, and 1.0f meaning all RAM is currently in use
    float GetSystemMemoryUsagePercentage()
    {
        FILE * fpIn = popen("/usr/bin/vm_stat", "r");
        if (fpIn)
        {
            double pagesUsed = 0.0, totalPages = 0.0;
            char buf[512];
            while(fgets(buf, sizeof(buf), fpIn) != NULL)
            {
                if (strncmp(buf, "Pages", 5) == 0)
                {
                    double val = ParseMemValue(buf);
                    if (val >= 0.0)
                    {
                        if ((strncmp(buf, "Pages wired",  11) == 0) ||
                            (strncmp(buf, "Pages active", 12) == 0)
                           )

                            pagesUsed += val;

                        totalPages += val;
                    }
                }
                else
                  if (strncmp(buf, "Mach Virtual Memory Statistics", 30) != 0)
                      break; // Stop at "Translation Faults". We don't care
                             // about anything at or below that
            }
            pclose(fpIn);

            if (totalPages > 0.0)
                return (float) (pagesUsed/totalPages);
        }
        return -1.0f;  // Indicate failure
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


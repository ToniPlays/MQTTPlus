#pragma once

#ifdef MQP_LINUX

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/statvfs.h>

namespace MQTTPlus
{

    struct CPUStats
    {
        int User;
        int Nice;
        int System;
        int Idle;
        int Iowait;
        int Irq;
        int Softirq;
        int Steal;
        int Guest;
        int GuestNice;

        int GetTotalIdle()
        const {
            return Idle + Iowait;
        }

        int GetTotalActive()
        const {
            return User + Nice + System + Irq + Softirq + Steal + Guest + GuestNice;
        }

    };

    struct MemoryStats
    {
        int TotalMemory;
        int AvailableMemory;
        int TotalSwap;
        int DreeSwap;

        float GetMemoryUsage() {
            return static_cast<float>(TotalMemory - AvailableMemory) / TotalMemory;
        }

        float GetSwapUsage() {
            return = static_cast<float>(TotalSwap - FreeSwap) / TotalSwap;
        }

    };

    inline CPUStats ReadCpuData()
    {
        CPUStats result;
        std::ifstream proc_stat("/proc/stat");

        if (proc_stat.good())
        {
            std::string line;
            getline(proc_stat, line);

            unsigned int *stats_p = (unsigned int *)&result;
            std::stringstream iss(line);
            std::string cpu;
            iss >> cpu;
            while (iss >> *stats_p)
            {
                stats_p++;
            };
        }

        proc_stat.close();

        return result;
    }

    inline int GetVal(const std::string &target, const std::string &content) {
        int result = -1;
        std::size_t start = content.find(target);
        if (start != std::string::npos) {
            int begin = start + target.length();
            std::size_t end = content.find("kB", start);
            std::string substr = content.substr(begin, end - begin);
            result = std::stoi(substr);
        }
        return result;
    }

    inline Memory_stats ReadMemoryData()
    {
        Memory_stats result;
        std::ifstream proc_meminfo("/proc/meminfo");

        if (proc_meminfo.good())
        {
            std::string content((std::istreambuf_iterator<char>(proc_meminfo)),
                    std::istreambuf_iterator<char>());

            result.total_memory = get_val("MemTotal:", content);
            result.total_swap = get_val("SwapTotal:", content);
            result.free_swap = get_val("SwapFree:", content);
            result.available_memory = get_val("MemAvailable:", content);

        }

        proc_meminfo.close();

        return result;
    }

    inline float GetCpuUsage(const CPU_stats &first, const CPU_stats &second) {
        const float active_time    = static_cast<float>(second.get_total_active() - first.get_total_active());
        const float idle_time    = static_cast<float>(second.get_total_idle() - first.get_total_idle());
        const float total_time    = active_time + idle_time;
        return active_time / total_time;
    }

    inline float GetDiskUsage(const std::string & disk) {
        struct statvfs diskData;

        statvfs(disk.c_str(), &diskData);

        auto total = diskData.f_blocks;
        auto free = diskData.f_bfree;
        auto diff = total - free;

        float result = static_cast<float>(diff) / total;

        return result;
    }

    inline int FindThermalzoneIndex() {
        int result = 0;
        bool stop = false;
        // 20 must stop anyway
        for (int i = 0; !stop && i < 20; ++i) {
            std::ifstream thermal_file("/sys/class/thermal/thermal_zone" + std::to_string(i) + "/type");

            if (thermal_file.good())
            {
                std::string line;
                getline(thermal_file, line);

                if (line.compare("x86_pkg_temp") == 0) {
                    result = i;
                    stop = true;
                }

            } else {
                stop = true;
            }

            thermal_file.close();
        }
        return result;
    }

    inline int GetThermalzoneTemperature(int thermal_index) {
        int result = -1;
        std::ifstream thermal_file("/sys/class/thermal/thermal_zone" + std::to_string(thermal_index) + "/temp");

        if (thermal_file.good())
        {
            std::string line;
            getline(thermal_file, line);

            std::stringstream iss(line);
            iss >> result;
        } else {
            throw std::invalid_argument(std::to_string(thermal_index) + " doesn't refer to a valid thermal zone.");
        }

        thermal_file.close();

        return result;
    }

}

#endif

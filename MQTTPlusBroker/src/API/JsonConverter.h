#pragma once

#include "MQTTPlusAPI.h"
#include "Core/ServiceManager.h"
#include <chrono>
#include <iostream>
#include <format>
#include <nlohmann/json.hpp>

namespace MQTTPlus::API
{
	using json = nlohmann::json;

    static std::string FormatTime(const auto& value)
    {
        return std::format("{:%FT%TZ}", value);
    }


    static void to_json(json& j, const ServiceInfo& info)
    {
        j = json {{ "running", info.Running },
                  { "startup_time", FormatTime(info.StartupTime.Value()) }
        };
    }

    static void to_json(json& j, const Service& service)
    {
        j = json {{ "name", service.Name }, { "info", service.Info }};
    }

	static void to_json(json& j, const ServerStatus& status)
	{
		j = json {
                  { "startup_time", FormatTime(status.StartupTime.Value()) },
                  { "service_count", status.ServiceCount },
                  { "running_service_count", status.RunningServices },
				  { "services", status.Services.Values() },
		};
	}
}

namespace MQTTPlus {
    using namespace nlohmann;
    //Put non API elements here
    static void to_json(json& j, const SystemStatus& status)
    {
        j = json {{ "cpu_load", status.UsageCPU },
                  { "memory_usage", status.MemoryUsage },
                  { "disk_space_total", status.DiskTotalSpace },
                  { "disk_space_used", status.DiskSpaceUsed }
        };
    }
}

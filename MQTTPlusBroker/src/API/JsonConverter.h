#pragma once

#include "MQTTPlusAPI.h"
#include "Core/Service/ServiceManager.h"
#include <chrono>
#include <iostream>
#ifdef MQP_MACOS
#include <format>
#endif
#include <iomanip>
#include <sstream>
#include <spdlog/fmt/fmt.h>
#include <nlohmann/json.hpp>


namespace MQTTPlus {
    using namespace nlohmann;
    //Put non API elements here
    static void to_json(json& j, const SystemStatus& status)
    {
        j = json {{ "cpu_load", status.UsageCPU },
                  { "memory_total", status.TotalMemory },
                  { "memory_available", status.AvailableMemory },
                  { "disk_space_total", status.DiskTotalSpace },
                  { "disk_space_used", status.DiskSpaceUsed }
        };
    }
}

namespace MQTTPlus::API
{
	using json = nlohmann::json;

    static std::string FormatTime(const auto& value)
    {
        #ifdef MQP_MACOS
        return std::format("{:%FT%TZ}", value);
        #elif MQP_LINUX
        auto itt = std::chrono::system_clock::to_time_t(value);
        std::ostringstream ss;
        ss << std::put_time(gmtime(&itt), "%FT%TZ");
        return ss.str();
        #endif
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
                  { "status", status.Status }
		};
	}

    static void to_json(json& j, const APIDevice& device)
	{
		j = json {
                  { "publicID", device.PublicID },
                  { "device_name", device.DeviceName },
                  { "nickname", device.Nickname },
				  { "status", device.Status },
                  { "last_seen", device.LastSeen.Value() }
		};
	}
}



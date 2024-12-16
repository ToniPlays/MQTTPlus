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


namespace MQTTPlus 
{
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
}

namespace MQTTPlus::API
{
	using json = nlohmann::json;

    static void to_json(json& j, const API::Service& service)
    {
        j = json {
                { "name", service.Name }, 
                { "running", service.Running }, 
                { "startup_time", FormatTime(service.StartupTime.Value()) }
            };
    }

	static void to_json(json& j, const API::ServerStatus& status)
	{
		j = json {
                  { "startup_time", FormatTime(status.StartupTime.Value()) },
                  { "service_count", status.ServiceCount },
                  { "running_service_count", status.RunningServices },
				  { "services", status.Services },
                  { "status", status.Status }
		};
	}

    static void to_json(json& j, const API::APINetwork& network)
	{
		j = json {
                  { "id", network.PublicID },
                  { "name", network.NetworkName },
                  { "type", network.NetworkType },
                  { "active_devices", network.ActiveDevices },
                  { "total_devices", network.TotalDevices },
                  { "devices", network.Devices }
		};
	}

    static void to_json(json& j, const API::MQTTFieldValue& value)
    {
        j = json {
                  { "id", value.PublicID },
                  { "device_id", value.DeviceID },
                  //{ "topic", value.Topic.Value() },
                  { "value", value.RawValue },
                  { "display_value", value.DisplayValue },
                  { "formatter", value.Formatter },
                  { "last_updated", value.LastUpdated.Value() }
		};

        Field<std::string> Formatter;
        Field<std::string> DisplayValue;
        Field<std::string> LastUpdated;
    };

    static void to_json(json& j, const API::APIDevice& device)
	{
		j = json {
                  { "id", device.PublicID },
                  { "client_id", device.DeviceName },
                  { "nickname", device.Nickname },
				  { "status", device.Status },
                  { "last_seen", device.LastSeen.Value() },
                  { "network", device.Network },
                  { "fields", device.Fields }
		};
	}
}



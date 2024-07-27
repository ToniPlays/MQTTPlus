#pragma once

#include "MQTTPlusAPI.h"
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>

namespace MQTTPlus::API
{
	using json = nlohmann::json;

    static void to_json(json& j, const ServiceInfo& info)
    {
        j = json {{ "running", info.Running },
            //{ "time_started", fmt::format("{:%FT%TZ}", info.StartupTime.Value()) }
        };
    }

    static void to_json(json& j, const Service& service)
    {
        j = json {{ "name", service.Name }, { "info", service.Info }};
    }

	static void to_json(json& j, const ServerStatus& status)
	{
		j = json {{ "service_count", status.ServiceCount },
				  { "services", status.Services.Values() },
		};
	}
}

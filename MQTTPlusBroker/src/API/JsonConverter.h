#pragma once

#include "MQTTPlusAPI.h"
#include <nlohmann/json.hpp>

namespace MQTTPlus::API
{
	using json = nlohmann::json;

	static void to_json(json& j, const MQTTServiceStatus& status)
	{
		j = json {{ "port", status.Port },
				  { "status", status.BrokerStatus },
				  { "connected_clients", status.ConnectedClients }
		};
	}
}

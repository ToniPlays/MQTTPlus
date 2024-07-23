#include "MQTTPlusBroker.h"
#include "Core/Broker.h"
#include "Core/WebSocket.h"
#include <chrono>
#include <signal.h>
#include <iostream>
#include <thread>
#include "spdlog/fmt/fmt.h"

#include "../vendor/cpp-httplib/httplib.h"

static bool running = true;

static void on_close(int signal)
{
    running = false;
}

int main(int argc, char* argv[])
{
    using namespace MQTTPlus;
    auto decoder = MQTTPlus::MQTTMessageDecoder(0);
    
    const auto settings = MQTTPlus::BrokerCreateSettings {
        .Port = 8883,
        .UseSSL = false,
    };
    
    Broker broker(settings);
    
    broker.SetOnClientConnected([&broker](Ref<MQTTPlus::Client> client) {
        std::cout << fmt::format("Client {} connected", client->GetAuth().ClientID) << std::endl;
    });
    
    broker.OnPublish("/test", [](Ref<MQTTPlus::Client> client, Buffer payload) {
        std::cout << "To data to /test" << std::endl;
    });
    
    
    broker.Listen();
	httplib::Server clientServer;
	
	clientServer.Post("/", [](const httplib::Request& reg, httplib::Response resp) {
		std::cout << "Got post" << std::endl;
		resp.set_content("some content", "text/plain");
	});
	
	std::cout << fmt::format("Listening for clients in 8884") << std::endl;
	clientServer.listen("0.0.0.0", 8884);
    
    return 0;
}

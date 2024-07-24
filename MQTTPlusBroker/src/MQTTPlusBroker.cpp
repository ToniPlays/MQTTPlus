#include "MQTTPlusBroker.h"
#include "Core/Broker.h"
#include "Core/WebSocket.h"
#include "Client/HTTPServer.h"
#include <chrono>
#include <signal.h>
#include <iostream>
#include <thread>
#include "spdlog/fmt/fmt.h"
#include <nlohmann/json.hpp>
#include "Client/Endpoints.h"

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
    
    HTTPServer clientServer(8884);
    
    clientServer.SetMessageResolver([](const char* endpoint, const std::string& message) {
        try {
            auto json = nlohmann::json::parse(message);
            std::string requestEndpoint = json["endpoint"];
            if(endpoint == requestEndpoint)
                return true;
            
            return false;
        } catch(std::exception& e)
        {
            return false;
        }
    });
    
    InitializeClientServerEndpoints(clientServer);
    
    clientServer.Listen();
    
    while(true)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        broker.ProcessMessages();
        //Keep listening
    }
    
    return 0;
}

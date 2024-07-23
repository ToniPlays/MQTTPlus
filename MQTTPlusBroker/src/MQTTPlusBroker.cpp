#include "MQTTPlusBroker.h"
#include "Core/Broker.h"
#include "Core/WebSocket.h"
#include <chrono>
#include <signal.h>
#include <iostream>
#include <thread>
#include "spdlog/fmt/fmt.h"

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
        .Port = 8886,
        .UseSSL = false,
    };
    
    Broker broker(settings);
    Ref<WebSocket> clientSocket = WebSocket::Create(8883, false);
    
    broker.SetOnClientConnected([&broker](Ref<MQTTPlus::Client> client) {
        std::cout << fmt::format("Client {} connected", client->GetAuth().ClientID) << std::endl;
    });
    
    broker.OnPublish("/test", [](Ref<MQTTPlus::Client> client, Buffer payload) {
        std::cout << "To data to /test" << std::endl;
    });
    
    clientSocket->SetOnSocketConnected([clientSocket](void* socket) mutable {
        clientSocket->Write(socket, "Hello from my server");
    });
    
    broker.Listen();
    clientSocket->Listen();
    
    
    
    signal(SIGINT, on_close);
    using namespace std::chrono_literals;
    
    while(running)
    {
        broker.ProcessMessages();
        std::this_thread::sleep_for(250ms);
    }
    
    return 0;
}

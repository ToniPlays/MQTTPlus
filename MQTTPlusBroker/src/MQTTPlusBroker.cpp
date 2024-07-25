#include "MQTTPlusBroker.h"
#include "Core/Logger.h"
#include "Server/Services/MQTTClientService.h"
#include "Server/Services/FrontendService.h"
#include "Server/ServiceManager.h"
#include <signal.h>
#include <chrono>
#include "spdlog/fmt/fmt.h"

static MQTTPlus::ServiceManager serviceManager;

static void on_close(int signal)
{
    serviceManager.Stop();
}

int main(int argc, char* argv[])
{
    using namespace MQTTPlus;

    Logger::Init();
    MQP_INFO("MQTTPlus v0.1a");
    signal(SIGABRT, on_close);
    
    
    auto mqtt = serviceManager.AddService<MQTTClientService>(8883);
    serviceManager.AddService<FrontendService>(8884);
    
    serviceManager.Start();
    
    using namespace std::chrono_literals;
    while(true)
    {
        MQP_INFO("Current client count {}", mqtt->GetBroker().GetConnectedClientCount());
        std::this_thread::sleep_for(2s);
    }
    
    
    return 0;
}

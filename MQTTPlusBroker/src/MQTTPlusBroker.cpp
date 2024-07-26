#include "MQTTPlusBroker.h"
#include "Core/Logger.h"
#include "Server/Services/MQTTClientService.h"
#include "Server/Services/FrontendService.h"
#include "Server/ServiceManager.h"
#include <signal.h>
#include <chrono>
#include "spdlog/fmt/fmt.h"

static void on_close(int signal)
{
    MQTTPlus::ServiceManager::Stop();
}

int main(int argc, char* argv[])
{
    using namespace MQTTPlus;

    Logger::Init();
    MQP_INFO("MQTTPlus v0.1a");
    signal(SIGABRT, on_close);
    
    
    auto mqtt = ServiceManager::AddService<MQTTClientService>(8883);
    ServiceManager::AddService<FrontendService>(8884);
    
    ServiceManager::Start();
    
    using namespace std::chrono_literals;
    while(true)
    {
        mqtt->Check();
        std::this_thread::sleep_for(2s);
    }
    
    
    return 0;
}

#include "MQTTPlusBroker.h"
#include "Core/Logger.h"
#include "MQTT/MQTTClientService.h"
#include "Server/FrontendService.h"
#include "Database/DatabaseService.h"
#include "Core/ServiceManager.h"
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
    
    ServiceManager::AddService<DatabaseService>();
    ServiceManager::AddService<MQTTClientService>(8883);
    ServiceManager::AddService<FrontendService>(8884);
    
    ServiceManager::Start();
    ServiceManager::WaitUntilStopped();
    ServiceManager::Stop();
    
    
    return 0;
}

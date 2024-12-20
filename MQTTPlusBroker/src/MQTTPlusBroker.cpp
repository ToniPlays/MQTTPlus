#include "MQTTPlusBroker.h"
#include "Core/Logger.h"
#include "MQTT/MQTTClientService.h"
#include "Server/FrontendService.h"
#include "Database/DatabaseService.h"
#include "Core/Service/ServiceManager.h"
#include "Core/CommandLineArgs.h"
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
    signal(SIGABRT, on_close);

    Logger::Init();
    MQP_INFO("MQTTPlus v0.1a");
    CommandLineArgs::Init(argc, argv);

    auto path = CommandLineArgs::Get<std::string>("wdir");
    if(!path.empty())
        std::filesystem::current_path(path);

    ServiceManager::AddService<DatabaseService>();
    ServiceManager::AddService<MQTTClientService>(CommandLineArgs::Get<int>("port_mqtt"));
    ServiceManager::AddService<FrontendService>(CommandLineArgs::Get<int>("port_http"));
    
    ServiceManager::Start();
    ServiceManager::Stop();
    
    return 0;
}

#include "MQTTPlusBroker.h"
#include "Server/Services/MQTTClientService.h"
#include "Server/Services/FrontendService.h"
#include "Server/ServiceManager.h"
#include <signal.h>
#include "spdlog/fmt/fmt.h"

static bool running = true;

static void on_close(int signal)
{
    running = false;
}

int main(int argc, char* argv[])
{
    using namespace MQTTPlus;
    
    ServiceManager serviceManager;
    serviceManager.AddService<MQTTClientService>(8883);
    serviceManager.AddService<FrontendService>(8884);
    
    serviceManager.Start();
    serviceManager.Stop();
    
    return 0;
}

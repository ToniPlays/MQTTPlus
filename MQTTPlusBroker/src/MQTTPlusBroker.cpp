#include "MQTTPlusBroker.h"
#include "Client/Services/MQTTClientService.h"
#include "Client/ServiceManager.h"
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
    serviceManager.AddService<MQTTClientService>();
    
    serviceManager.Start();
    serviceManager.Stop();
    
    return 0;
}

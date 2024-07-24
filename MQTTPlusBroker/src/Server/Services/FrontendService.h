#pragma once

#include "Service.h"
#include "Server/HTTPServer.h"

#include <iostream>

namespace MQTTPlus
{
    class FrontendService : public Service {
        public:
            FrontendService(uint32_t port);
            ~FrontendService() = default;

            void Start(ServiceManager* manager) override;
            void Stop() override;

            std::string GetName() const override { return "FrontendService"; }
        private:
            std::unique_ptr<HTTPServer> m_Server;
    };
}
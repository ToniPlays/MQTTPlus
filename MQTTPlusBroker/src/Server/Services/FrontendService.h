#pragma once

#include "Service.h"
#include "Server/HTTPServer.h"
#include "Core/Threading/Thread.h"

#include <iostream>

namespace MQTTPlus
{
    class FrontendService : public Service {
        public:
            FrontendService(uint32_t port);
            ~FrontendService() = default;

            void Start() override;
            void Stop() override;

            std::string GetName() const override { return "FrontendService"; }
        private:
            HTTPServer* m_Server;
            Ref<Thread> m_Thread;
    };
}
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>

#include "Ref.h"

namespace MQTTPlus {

    using SocketConnected = std::function<void(void*)>;
    using SocketDisconnected = std::function<void(void*, int)>;
    using SocketDataReceive = std::function<void(void*, char*, int)>;

    class WebSocket : public RefCount
    {
    public:

        virtual uint32_t GetPort() const = 0;

        virtual void Listen() = 0;
        virtual void DisconnectClient(void* socket) = 0;
        virtual void SetSocketTimeout(void* socket, uint32_t timeout) = 0;
        
        virtual void Write(void* socket, std::vector<uint8_t> bytes) = 0;
        virtual void Write(void* socket, const std::string& message) = 0;
        
        virtual void SetOnSocketConnected(SocketConnected&& callback) = 0;
        virtual void SetOnSocketDisconnected(SocketDisconnected&& callback) = 0;
        virtual void SetOnSocketDataReceived(SocketDataReceive&& callback) = 0;

        virtual bool IsListening() const = 0;
        
        static Ref<WebSocket> Create(uint32_t port, bool ssl = false);
    };
}

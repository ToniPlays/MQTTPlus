#pragma once

#include <iostream>
#include <vector>
#include <format>
#include "Ref.h"
#include "gsl.h"
#include "Buffer/CachedBuffer.h"

namespace MQTT {
    enum class MessageType {
        Reserved1 = 0,
        Connect = 1,    //Created
        ConnAck = 2,    //Created
        Publish = 3,    
        PubAck = 4,
        PubRec = 5,
        PubRel = 6,
        PubComp = 7,
        Subscribe = 8,
        SubAck = 9,
        Unsubscribe = 10,
        UnsubAck = 11,
        PingReg = 12,
        PingResp = 13,
        Disconnect = 14,
        Reserved2 = 15,
    };
    enum class ProtocolVersion : uint8_t
    {
        MQTT3 = 4,
        MQTT5 = 5,
    };
    enum class ConnectFlags: uint8_t {
        CleanSession = (1 << 1),
        WillFlag = (1 << 2),
        WillQoS1 = (1 << 3),
        WillQoS2 = (1 << 4),
        WillRetain = (1 << 5),
        Password = (1 << 6),
        UserName = (1 << 7)
    };


    struct FixedHeader {
        enum { SIZE = 2 };
        
        MessageType Type;
        bool Dup;
        uint8_t QoS;
        bool retain;
        uint32_t Remaining;
    };

    struct Authentication {
        std::string ClientID;
        std::string User;
        std::string Password;
        
        std::string WillTopic;
        std::string WillMessage;
    };
    enum class ConnAckFlags : uint8_t {
        Accepted = 0x00,
        ProtocolVersionUnavailable = 0x01,
        IdentifierRejected = 0x02,
        ServerUnavailable = 0x03,
        BadUsernameOrPassword = 0x04,
        NotAuthorized = 0x05,
    };


    static MessageType GetMessageType(uint8_t byte)
    {
        return static_cast<MessageType>(byte >> 4);
    }

    class Message : public RefCount
    {
    public:
        virtual MessageType GetType() const = 0;
        virtual std::vector<uint8_t> GetBytes() const = 0;
        virtual std::string ToString() const = 0;
    };

    class ConnectMessage : public Message
    {
    public:
        ConnectMessage(CachedBuffer& buffer);
        
        MessageType GetType() const override { return MessageType::Connect; }
        uint16_t GetKeepAlive() const { return m_KeepAliveFor; }
        
        const Authentication& GetAuthentication() const { return m_Authentication; }
        
        virtual std::vector<uint8_t> GetBytes() const override
        {
            return std::vector<uint8_t> { 0x10, 0x29, 0, 0 };
        }
        virtual std::string ToString() const override {
            return std::format("ConnectMessage: ClientID {}, Flags {}, Alive for {}", m_Authentication.ClientID, m_Flags, m_KeepAliveFor);
        }
        
    private:
        uint8_t m_Flags = 0;
        uint16_t m_KeepAliveFor = 0;
        ProtocolVersion m_Version;
        
        Authentication m_Authentication;
    };

    class ConnAckMessage : public Message
    {
    public:
        
        ConnAckMessage(ConnAckFlags ack, bool sessionParent = false) : m_Ack((uint8_t)ack), m_HasParent(sessionParent) {}
        
        MessageType GetType() const override { return MessageType::ConnAck; }
        virtual std::vector<uint8_t> GetBytes() const override
        {
            return std::vector<uint8_t> { 0x20, 0x02, (uint8_t)(m_HasParent ? 0x01 : 0x00), m_Ack };
        }
        virtual std::string ToString() const override {
            return std::format("ConnectMessage");
        }
        
    private:
        uint8_t m_Ack;
        bool m_HasParent = false;
    };
    class PublishMessage : public Message
    {
    public:
        PublishMessage(CachedBuffer& buffer);
        
        MessageType GetType() const override { return MessageType::ConnAck; }
        virtual std::vector<uint8_t> GetBytes() const override
        {
            return std::vector<uint8_t> { 0 };
        }
        virtual std::string ToString() const override {
            return std::format("PublishMessage: {}", m_Topic);
        }
        
        private:
            std::string m_Topic;
            std::vector<uint8_t> m_Payload;
    };

    static std::string ReadString(CachedBuffer& buffer)
    {
        uint16_t length = (buffer.Read<uint8_t>() << 8) + buffer.Read<uint8_t>();
        std::string result;
        
        for(uint32_t i = 0; i < length; i++)
            result += buffer.Read<char>();
        return result;
    }
}

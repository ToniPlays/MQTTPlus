#include "MQTT.h"

#include "Core/Logger.h"

namespace MQTTPlus::MQTT
{
    ConnectMessage::ConnectMessage(CachedBuffer& buffer)
    {
        buffer.Read<uint8_t>();
        uint8_t remaining = buffer.Read<uint8_t>();
        if((buffer.GetSize() - buffer.GetCursor()) < remaining)
        {
            m_Authentication.Valid = false;
            return;
        }
        
        std::string protocolName = ReadString(buffer, ReadU16(buffer));
        
        m_Version = buffer.Read<ProtocolVersion>();
        m_Flags = buffer.Read<uint8_t>(); //Connect flags
        m_KeepAliveFor = ReadU16(buffer);
        
        m_Authentication.ClientID += ReadString(buffer, ReadU16(buffer));
        
        if(m_Flags & (uint8_t)ConnectFlags::WillFlag)
        {
            m_Authentication.WillTopic = ReadString(buffer, ReadU16(buffer));
            m_Authentication.WillMessage = ReadString(buffer, ReadU16(buffer));
        }   
        
        if(m_Flags & (uint8_t)ConnectFlags::UserName)
            m_Authentication.User = ReadString(buffer, ReadU16(buffer));
        
        if(m_Flags & (uint8_t)ConnectFlags::Password)
            m_Authentication.Password = ReadString(buffer, ReadU16(buffer));
    }

    DisconnectMessage::DisconnectMessage(CachedBuffer& buffer)
    {
        buffer.Read<uint8_t>();
        buffer.Read<uint8_t>(); //Remaining length
        if(!buffer.Available()) return;
        
        m_Reason = buffer.Read<uint8_t>();
    }

    PublishMessage::PublishMessage(CachedBuffer& buffer)
    {
        uint8_t flags = buffer.Read<uint8_t>();
        buffer.Read<uint8_t>();
        uint8_t QOSLevel = (flags >> 1) & 0x03; //Fixed header
        uint16_t messageId = 0;

        m_Topic = ReadString(buffer, ReadU16(buffer));

        if(QOSLevel > 0)
            messageId = ReadU16(buffer);
        
        m_Message = ReadString(buffer, buffer.GetSize() - buffer.GetCursor()); 
    }

    SubscribeMessage::SubscribeMessage(CachedBuffer& buffer)
    {
        uint8_t flags = buffer.Read<uint8_t>();
        uint8_t remainingLength = buffer.Read<uint8_t>();
        m_MessageID = ReadU16(buffer);

        while(buffer.Available())
        {
            std::string topic = ReadString(buffer, ReadU16(buffer));
            uint8_t qos = buffer.Read<uint8_t>();
            m_Topics.push_back({ topic, qos });
        }
    }
}

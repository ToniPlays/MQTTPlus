#include "MQTT.h"

namespace MQTT {
    ConnectMessage::ConnectMessage(CachedBuffer& buffer)
    {
        buffer.Read<uint16_t>(); //Fixed header
        
        std::string protocolName = ReadString(buffer);
        
        m_Version = buffer.Read<ProtocolVersion>();
        m_Flags = buffer.Read<uint8_t>(); //Connect flags
        m_KeepAliveFor = (buffer.Read<uint8_t>() << 8) + buffer.Read<uint8_t>();
        
        m_Authentication.ClientID += ReadString(buffer);
        
        if(m_Flags & (uint8_t)ConnectFlags::WillFlag)
        {
            m_Authentication.WillTopic = ReadString(buffer);
            m_Authentication.WillMessage = ReadString(buffer);
        }
                
        
        if(m_Flags & (uint8_t)ConnectFlags::UserName)
        {
            m_Authentication.User = ReadString(buffer);
        }
        
        if(m_Flags & (uint8_t)ConnectFlags::Password)
        {
            m_Authentication.Password = ReadString(buffer);
        }
    }

    PublishMessage::PublishMessage(CachedBuffer& buffer)
    {
        buffer.Read<uint8_t>(); //Fixed header
        buffer.Read<uint8_t>(); //Fixed header
        m_Topic = ReadString(buffer);
        
    }
}

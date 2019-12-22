#include "protocol.hpp"
#include "packet_manager.hpp"

void Protocol::SendMessage(const char* message)
{
    PacketManager::WritePacketFlag();
    PacketManager::WritePacketData(0x10); // Пакет с сообщением
    PacketManager::WritePacketData( reinterpret_cast<const uint8_t *> (message), strlen(message) );
    PacketManager::WritePacketFlag();
}
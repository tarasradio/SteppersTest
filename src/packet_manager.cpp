#include "packet_manager.hpp"

#define PACKET_SIZE 64

static uint8_t buffer[PACKET_SIZE * 2];
static uint8_t bufferTail = 0;

uint8_t packetBuffer[PACKET_SIZE];

const uint8_t EscSymbol = 0x7D;
const uint8_t FlagSymbol = 0xDD;

static bool escapeFlag = false;

static uint8_t packetTail = 0;

PacketManager::PacketManager(IPacketListener * listener)
{
    _listener = listener;
}

void PacketManager::ReadPacket()
{
    bufferTail = 0;
    uint8_t countBytes = Serial.available();
    if (countBytes > 0)
    {
        while (countBytes != 0)
        {
            buffer[bufferTail++] = Serial.read();
            countBytes--;
        }
    }
}

void PacketManager::WritePacketData(uint8_t byte)
{
    if(byte == FlagSymbol || 
    byte == EscSymbol)
    {
        Serial.write(EscSymbol);
    }
    Serial.write(byte);
}

void PacketManager::WritePacketData(uint8_t const * bytes, uint8_t bytesNumber)
{
    for(uint8_t i = 0; i < bytesNumber; i++)
    {
        uint8_t packetByte = bytes[i];

        WritePacketData(packetByte);
    }
}

void PacketManager::WritePacketFlag()
{
    Serial.write(FlagSymbol);
}

void PacketManager::tryPacketBuild(uint8_t bufferPosition)
{
    packetBuffer[packetTail++] = buffer[bufferPosition];

    if(packetTail == PACKET_SIZE)
    {
        packetTail = 0;
    }
    escapeFlag = false;
}

void PacketManager::findByteStuffingPacket()
{
    uint8_t position = 0;

    while(position < bufferTail)
    {
        if(FlagSymbol == buffer[position])
        {
            if(escapeFlag)
            {
                tryPacketBuild(position);
            }
            else
            {
                _listener->listenPacket(packetBuffer, packetTail-1);
                packetTail = 0;
            }
        }
        else if(EscSymbol == buffer[position])
        {
            if(escapeFlag)
            {
                tryPacketBuild(position);
            }
            else
            {
                escapeFlag = true;
            }
        }
        else
        {
            tryPacketBuild(position);
        }
        position++;
    }
}
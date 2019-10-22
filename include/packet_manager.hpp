#ifndef packet_manager_hpp
#define packet_manager_hpp

#include <Arduino.h>

class IPacketListener
{
public:
    virtual void listenPacket(uint8_t *packet, uint8_t packetLength) = 0;
};

class PacketManager
{
private:
    IPacketListener *_listener;
public:
    PacketManager(IPacketListener * );
    void ReadPacket();

    static void WritePacketData(uint8_t byte);
    static void WritePacketData(uint8_t *bytes, uint8_t bytesNumber);
    static void WritePacketFlag();

    void tryPacketBuild(uint8_t bufferPosition);
    void findByteStuffingPacket();
};

#endif
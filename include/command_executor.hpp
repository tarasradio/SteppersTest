#ifndef command_executor_hpp
#define command_executor_hpp

#include <Arduino.h>

#include "steppers_controller.hpp"
#include "packet_manager.hpp"

class CommandExecutor : public IPacketListener
{
private:
    SteppersController * _steppersController;

    uint32_t readLong(uint8_t *buffer);
    uint16_t readInt(uint8_t *buffer);

    void executeMoveCommand(uint8_t *packet, uint32_t packetId);
    void executeRunCommand(uint8_t *packet, uint32_t packetId);
    void executeStopCommand(uint8_t *packet, uint32_t packetId);
    void executeSetDeviceStateCommand(uint8_t *packet, uint32_t packetId);

public:
    CommandExecutor() {}
    CommandExecutor(SteppersController * );
    void listenPacket(uint8_t *packet, uint8_t packetLength);
};

#endif
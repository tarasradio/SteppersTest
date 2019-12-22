#include "command_executor.hpp"

#include "packet_manager.hpp"

#include "protocol.hpp"

#include "steppers_controller.hpp"
#include "sensors.hpp"
#include "devices.hpp"

int cmd = 0;

CommandExecutor::CommandExecutor(SteppersController * steppersController)
{
    this->_steppersController = steppersController;
}

void CommandExecutor::listenPacket(uint8_t *packet, uint8_t packetLength)
{
    uint32_t packetId = readLong(packet + 0);
    byte commandType = packet[4];

    {
        String message = "command = " + String(commandType);
        Protocol::SendMessage(message.c_str());
    }

    String message = "cmd = " + String(cmd++);
    Protocol::SendMessage(message.c_str());

    switch (commandType)
    {
        case CMD_MOVE:
        {
            executeMoveCommand(packet + 5, packetId);
        }
        break;
        case CMD_RUN:
        {
            executeRunCommand(packet + 5, packetId);
        }
        break;
        case CMD_STOP:
        {
            executeStopCommand(packet + 5, packetId);
        }
        break;
        case CMD_SET_DEVICE_STATE:
        {
            executeSetDeviceStateCommand(packet + 5, packetId);
        }
        break;
        default:
        {
            {
                String message = "Unknown command!";
                Protocol::SendMessage(message.c_str());
            }
        }
        break;
    }
}

uint32_t CommandExecutor::readLong(uint8_t *buffer)
{
    return *((unsigned long *)(buffer));
}

uint16_t CommandExecutor::readInt(uint8_t *buffer)
{
    return *((unsigned int *)(buffer));
}

void CommandExecutor::executeMoveCommand(uint8_t *packet, uint32_t packetId)
{
    int8_t stepper = packet[0];
    int32_t speed = readLong(packet + 1);
    int32_t steps = readLong(packet + 5);

    if(speed < 0)
    {
        steps = -steps;
    }

    _steppersController->stop(stepper);
    _steppersController->move(stepper, steps, abs(speed));
    
    {
        String message = "Move command: ";
        message += "stepper = " + String(stepper);
        message += "speed = " + String(speed); 
        message += "steps = " + String(steps);

        Protocol::SendMessage(message.c_str());
    }
}

void CommandExecutor::executeRunCommand(uint8_t *packet, uint32_t packetId)
{
    int8_t stepper = packet[0];
    int32_t fullSpeed = readLong(packet + 1);

    _steppersController->run(stepper, fullSpeed);
    
    {
        String message = "Run command: ";
        message += "stepper = " + String(stepper);
        message += "speed = " + String(fullSpeed);

        Protocol::SendMessage(message.c_str());
    }
}

void CommandExecutor::executeStopCommand(uint8_t *packet, uint32_t packetId)
{
    uint8_t stepper = packet[0];

    _steppersController->stop(stepper);
    
    {
        String message = "Stop command: ";
        message += "stepper = " + String(stepper);
    
        Protocol::SendMessage(message.c_str());
    }
}

void CommandExecutor::executeSetDeviceStateCommand(uint8_t *packet, uint32_t packetId)
{
    uint8_t device = packet[0];
    uint8_t state = packet[1];

    Devices::SetState(device, state);

    {
        String message = "Set device state: ";
        message += "dev = " + String(device);
        message += ", state = " + String(state);
    
        Protocol::SendMessage(message.c_str());
    }
}
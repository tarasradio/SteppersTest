#include "command_executor.hpp"

#include "packet_manager.hpp"

#include "protocol.hpp"

#include "steppers_controller.hpp"
#include "sensors.hpp"
#include "devices.hpp"

CommandExecutor::CommandExecutor(SteppersController * steppersController)
{
    this->_steppersController = steppersController;
}

uint8_t CommandExecutor::checkMode()
{
    uint8_t mode = _steppersController->getControlMode();
    if(mode == SteppersController::HAND_CONTROL)
    {
        String message = "Error: hand control is active!";
        Protocol::SendMessage(message.c_str());
    }

    return mode;
}

void CommandExecutor::listenPacket(uint8_t *packet, uint8_t packetLength)
{
    uint32_t packetId = readLong(packet + 0);
    byte commandType = packet[4];

    {
        String message = "command = " + String(commandType);
        Protocol::SendMessage(message.c_str());
    }

    switch (commandType)
    {
        case CMD_MOVE:
        {
            if(checkMode() == 
            SteppersController::CNC_CONTROL)
            {
                executeMoveCommand(packet + 5, packetId);
            }
        }
        case CMD_RUN:
        {
            if(checkMode() == 
            SteppersController::CNC_CONTROL)
            {
                executeRunCommand(packet + 5, packetId);
            }
        }
        break;
        case CMD_STOP:
        {
            if(checkMode() == 
            SteppersController::CNC_CONTROL)
            {
                executeStopCommand(packet + 5, packetId);
            }
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
    int32_t steps = readLong(packet + 1);

    _steppersController->move(stepper, steps);
    
    {
        String message = "Move command: ";
        message += "stepper = " + String(stepper);
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
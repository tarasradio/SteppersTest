#ifndef protocol_hpp
#define protocol_hpp

#include <Arduino.h>

enum Commands
{
    CMD_MOVE = 0x09,
    CMD_RUN = 0x10,
    CMD_STOP = 0x11,
    CMD_SET_DEVICE_STATE = 0x12,
};

class Protocol
{
public:
    static void SendMessage(const char* message);
};

#endif

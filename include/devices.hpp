#ifndef devices_hpp
#define devices_hpp

#include <Arduino.h>

class Devices
{
public:
    static void Init();
    static void DeviceOn(int device);
    static void DeviceOff(int device);
    static void SetState(int8_t device, int8_t state);
};

#endif
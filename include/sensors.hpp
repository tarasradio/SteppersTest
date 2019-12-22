#ifndef sensors_hpp
#define sensors_hpp

#include <Arduino.h>

class Sensors
{
public:
    static void Init();
    static int16_t getSensorValue(uint8_t sensorNumber);
    static uint8_t getSwitchState(uint8_t switchNumber);
};

#endif
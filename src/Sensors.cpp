#include "sensors.hpp"

#define SW_A_PORT PORTD
#define SW_A_DDR DDRD
#define SW_A_PIN PIND
#define SW_A_FIRST_PIN 0

int16_t Sensors::getSensorValue(uint8_t sensorNumber)
{
    return analogRead(A0 + sensorNumber);
}

uint8_t Sensors::getSwitchState(uint8_t switchNumber)
{
    return SW_A_PIN & (1 << switchNumber);
}

void Sensors::Init()
{
    SW_A_DDR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 2);
    SW_A_PORT |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 2);
}
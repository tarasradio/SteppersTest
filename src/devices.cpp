#include "devices.hpp"

#define DEVICES_A_PORT PORTL
#define DEVICES_A_DDR DDRL
#define DEVICES_A_FIRST_PIN 0

#define DEVICES_B_PORT PORTB
#define DEVICES_B_DDR DDRB
#define DEVICES_B_FIRST_PIN 0

void Devices::Init()
{
    DEVICES_A_DDR |= (1 << 7) | (1 << 6) | (1 << 5) | (1 << 4);
    DEVICES_A_DDR |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
    DEVICES_B_DDR |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
}

void Devices::DeviceOn(int device)
{
    SetState(device, 1);
}

void Devices::DeviceOff(int device)
{
    SetState(device, 0);
}

void Devices::SetState(int8_t device, int8_t state)
{
    if(device >= 0 && device <= 7)
    {
        state > 0 ? DEVICES_A_PORT |= (1 << device) : DEVICES_A_PORT &= ~(1 << device);
    }
    else if(device > 7 && device <= 11)
    {
        device = device - 8;
        state > 0 ? DEVICES_B_PORT |= (1 << device) : DEVICES_B_PORT &= ~(1 << device);
    }
}
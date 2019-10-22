#include "relays.hpp"

#define RELAYS_A_PORT PORTD
#define RELAYS_A_DDR DDRD
#define RELAYS_A_FIRST_PIN 0

#define RELAYS_B_PORT PORTH
#define RELAYS_B_DDR DDRH
#define RELAYS_B_FIRST_PIN 0

#define RELAYS_C_PORT PORTJ
#define RELAYS_C_DDR DDRJ
#define RELAYS_C_FIRST_PIN 0

void Relays::Init()
{
    RELAYS_A_DDR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 2);
    RELAYS_B_DDR |= (1 << 0) | (1 << 1);
    RELAYS_C_DDR |= (1 << 0) | (1 << 1);
}

void Relays::RelayOn(int relay)
{
    setState(relay, 1);
}

void Relays::RelayOff(int relay)
{
    setState(relay, 0);
}

void Relays::setState(int8_t relay, int8_t state)
{
    if (relay >= 0 && relay <= 3) // RELAYS_A : (3 - 0)
    {
        state > 0 ? RELAYS_A_PORT |= (1 << relay) : RELAYS_A_PORT &= ~(1 << relay);
    }
    if (relay > 3 && relay <= 5) // RELAYS_B : (1 - 0)
    {
        relay = (relay - 4);
        state > 0 ? RELAYS_B_PORT |= (1 << relay) : RELAYS_B_PORT &= ~(1 << relay);
    }
    else if (relay > 5 && relay <= 7) // RELAYS_C : (1 - 0)
    {
        relay = (relay - 6);
        state > 0 ? RELAYS_C_PORT |= (1 << relay) : RELAYS_C_PORT &= ~(1 << relay);
    }
}
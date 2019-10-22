#ifndef relays_hpp
#define relays_hpp

#include <Arduino.h>

class Relays
{
public:
    static void Init();
    static void RelayOn(int relay);
    static void RelayOff(int relay);
private:
    static void setState(int8_t relay, int8_t state);
};

#endif
#include <Arduino.h>

#include "steppers_controller.hpp"
#include "sensors.hpp"
#include "relays.hpp"
#include "devices.hpp"

#include "command_executor.hpp"
#include "packet_manager.hpp"

#define BAUDRATE 115200
#define POLLING_TIMEOUT 50

SteppersController controller;
CommandExecutor commandExecutor(&controller);
PacketManager packetManager(&commandExecutor);

ISR(TIMER1_COMPA_vect)
{
    controller.interruptHandler();
}

void setup()
{
    Serial.begin(BAUDRATE);

    controller.init();
    Relays::Init();
    Devices::Init();
    Sensors::Init();

    controller.setMinSpeed(0, 400);
    controller.setMinSpeed(1, 400);
    controller.setMinSpeed(2, 400);
    controller.setMinSpeed(3, 400);
    controller.setMinSpeed(4, 400);

    controller.setSpeed(0, 8000);
    controller.setSpeed(1, 8000);
    controller.setSpeed(2, 8000);
    controller.setSpeed(3, 8000);
    controller.setSpeed(4, 8000);
}

void loop()
{
    packetManager.ReadPacket();
    packetManager.findByteStuffingPacket();

    delay(POLLING_TIMEOUT);
}



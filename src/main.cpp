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

void updateStepperState(int8_t sensor, int8_t stepper);

void testSensors();
void printFakeSteppers();

void setup()
{
    Serial.begin(BAUDRATE);

    controller.init();
    Relays::Init();
    Devices::Init();

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

long deg360 = 1600;

void loop()
{

    packetManager.ReadPacket();
    packetManager.findByteStuffingPacket();

    if(controller.getControlMode() == SteppersController::HAND_CONTROL)
    {
        updateStepperState(0, 0);
        updateStepperState(1, 1);
        updateStepperState(2, 2);
        updateStepperState(3, 3);
        updateStepperState(4, 4);
        updateStepperState(5, 5);
        updateStepperState(6, 6);
        updateStepperState(7, 7);
    }
    
    delay(POLLING_TIMEOUT);
    
    
    //controller.PrintSteppers();

    //delay(50);

/*  steppers.move(0, deg360*16);
    steppers.move(1, deg360*16);
    steppers.runAndWait();

    steppers.move(0, -deg360*16);
    steppers.move(1, deg360*16);
    steppers.runAndWait(); */
    
    //delay(100);
}

int16_t minValue = 100;
int16_t maxValue = 512;

int minSpeed = 50;
int maxSpeed = 10000;

int getSpeed(uint16_t value)
{
    return minSpeed + (float)value * ((float)(maxSpeed - minSpeed) / (float)(maxValue - minValue));
}



void updateStepperState(int8_t sensor, int8_t stepper)
{
    int16_t value = Sensors::getSensorValue(sensor);
    int16_t shiftValue = value - 512;
    int speed = getSpeed( abs(shiftValue) );

    if( shiftValue < -minValue)
        controller.run(stepper, -speed);
    else if( shiftValue > minValue)
        controller.run(stepper, speed);
    else
        controller.stop(stepper);
}

void testSensors()
{
    for(uint8_t i = 0; i < 8; i++)
    {
        Serial.print(" S" + String(i) + ": " + String(Sensors::getSensorValue(i)));
    }
    Serial.println();
}


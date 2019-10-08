#include <Arduino.h>

#include "steppers_controller.hpp"
#include "sensors.hpp"

SteppersController steppers;

ISR(TIMER1_COMPA_vect)
{
    steppers.interruptHandler();
}

void updateStepperState(uint8_t sensor, uint8_t stepper);

void setup()
{
    Serial.begin(9600);
    steppers.init();

    steppers.setMinSpeed(0, 400);
    steppers.setMinSpeed(1, 400);
    steppers.setMinSpeed(2, 400);

    steppers.setSpeed(0, 4000);
    steppers.setSpeed(1, 4000);
    steppers.setSpeed(2, 4000);
}

long deg360 = 1600;

void loop()
{
    updateStepperState(0, 0);
    updateStepperState(1, 1);
    updateStepperState(2, 2);

    Serial.println();

/*  steppers.move(0, deg360*16);
    steppers.move(1, deg360*16);
    steppers.runAndWait();

    steppers.move(0, -deg360*16);
    steppers.move(1, deg360*16);
    steppers.runAndWait(); */
    
    delay(100);
}

int16_t minValue = 64;
int16_t maxValue = 512;

int minSpeed = 50;
int maxSpeed = 4000;

int getSpeed(uint16_t value)
{
    return minSpeed + (float)value * ((float)(maxSpeed - minSpeed) / (float)(maxValue - minValue));
}

void updateStepperState(uint8_t sensor, uint8_t stepper)
{
    int16_t value = Sensors::getSensorValue(sensor);

    int16_t shiftValue = value - 512;

    int speed = getSpeed( abs(shiftValue) );
    
    Serial.print(" s[" + String(stepper) + "] speed = " + String(speed));

    if( shiftValue < -minValue)
    {
        steppers.run(stepper, -speed );
    }
    else if( shiftValue > minValue)
    {
        steppers.run(stepper, speed );
    }
    else
    {
        steppers.stop(stepper);
    }
}


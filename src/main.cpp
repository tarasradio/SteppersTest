#include <Arduino.h>

#include "steppers.hpp"

Steppers steppers;

ISR(TIMER1_COMPA_vect)
{
    steppers.interruptHandler();
}

void setup()
{
    Serial.begin(9600);
    steppers.init();
}

long deg360 = 1600;

void loop()
{
    steppers.setMinSpeed(0, 10);
    steppers.setMinSpeed(1, 10);

    steppers.setSpeed(0, 500);
    steppers.setSpeed(1, 500);

    steppers.move(0, deg360 * 4);
    steppers.move(1, deg360 * 4);
    steppers.runAndWait();

    steppers.setMinSpeed(0, 20);
    steppers.setMinSpeed(1, 10);

    steppers.setSpeed(0, 500);
    steppers.setSpeed(1, 500);

    steppers.move(0, deg360 * 4);
    steppers.move(1, deg360 * 4);
    steppers.runAndWait();

    steppers.setMinSpeed(0, 30);
    steppers.setMinSpeed(1, 10);

    steppers.setSpeed(0, 1000);
    steppers.setSpeed(1, 1000);

    steppers.move(0, deg360 * 4);
    steppers.move(1, deg360 * 4);
    steppers.runAndWait();

    delay(1000);
}


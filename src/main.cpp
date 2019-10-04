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
    steppers.setMinSpeed(0, 200);
    steppers.setMinSpeed(1, 200);

    steppers.setSpeed(0, 1000);
    steppers.setSpeed(1, 1000);

    for(int i = 0; i < 10; i++)
    {
        steppers.move(0, deg360 / 4);
        steppers.move(1, -deg360 / 4);
        steppers.runAndWait();

        steppers.move(0, -deg360 / 4);
        steppers.move(1, deg360 / 4);
        steppers.runAndWait();
    }
    
    delay(1000);
}


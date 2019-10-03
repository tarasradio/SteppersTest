#ifndef steppers_hpp
#define steppers_hpp

#include "stepper.hpp"

#include <Arduino.h>

#define NUM_STEPPERS 2

class Steppers
{
public:
    void init();
    void interruptHandler();
    void setSpeed(byte stepper, unsigned int stepsPerSecond);
    void setMinSpeed(byte stepper, unsigned int stepsPerSecond);
    void move(byte stepper, long steps);
    void runAndWait();
private:
    void initPins();
    void initTimer();
    void initSteppers();

    void setNextInterruptInterval();

    Stepper steppers[NUM_STEPPERS];
    volatile byte remainingSteppersFlag = 0;
    volatile byte nextStepperFlag = 0;
    long ind = 0;
};

#endif
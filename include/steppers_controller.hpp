#ifndef steppers_controller_hpp
#define steppers_controller_hpp

#include "stepper.hpp"

#include <Arduino.h>

#define NUM_STEPPERS 3

class SteppersController
{
public:
    SteppersController();
    void init();
    void interruptHandler();

    void setSpeed(uint8_t stepper, unsigned int stepsPerSecond);
    void setMinSpeed(uint8_t stepper, unsigned int stepsPerSecond);
    void move(uint8_t stepper, long steps);
    void run(uint8_t stepper, int speed);
    void home(uint8_t stepper, int speed);
    void stop(uint8_t stepper);

    void runAndWait();
private:
    void initPins();
    void initTimer();
    void initSteppers();

    void setNextInterruptInterval();

    uint8_t remainingStepper(uint8_t number);
    uint8_t nextStepper(uint8_t number);

    Stepper steppers[NUM_STEPPERS];
    volatile byte remainingSteppersFlag = 0;
    volatile byte nextStepperFlag = 0;
};

#endif
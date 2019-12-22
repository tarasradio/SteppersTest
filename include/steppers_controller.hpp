#ifndef steppers_controller_hpp
#define steppers_controller_hpp

#include <Arduino.h>
#include "stepper.hpp"

#define NUM_STEPPERS 5 //5
#define NUM_FAKE_STEPPERS 8 //8

class SteppersController
{
public:
    enum ControlState
    {
        HAND_CONTROL,
        CNC_CONTROL
    };

    SteppersController();
    void init();
    void interruptHandler();

    void setSpeed(int8_t stepper, unsigned int stepsPerSecond);
    void setMinSpeed(int8_t stepper, unsigned int stepsPerSecond);
    void move(int8_t stepper, long steps, int speed);
    void run(int8_t stepper, int speed);
    void home(int8_t stepper, int speed);
    void stop(int8_t stepper);

    void PrintSteppers();

    void runAndWait();
    void setControlMode(uint8_t mode);
    uint8_t getControlMode();
private:
    void initPins();
    void initTimer();
    void initSteppers();

    int tryStepper(int8_t stepper, uint8_t needsRun);

    void setNextInterruptInterval();

    uint8_t remainingStepper(uint8_t number);
    uint8_t nextStepper(uint8_t number);

    Stepper steppers[NUM_STEPPERS];

    byte remainingSteppersFlag;
    byte nextStepperFlag;

    uint8_t fakeSteppers[NUM_FAKE_STEPPERS];

    uint8_t _currentMode;
};

#endif
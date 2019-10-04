#ifndef stepper_hpp
#define stepper_hpp

#include <Arduino.h>

class Stepper
{
public:
    Stepper(volatile uint8_t *port, uint8_t stepPin, uint8_t dirPin);
    Stepper();
    void resetInfo();
    void reset();
    void setDir(uint8_t dir);
    void doStep();
    bool updateState();
    void setCorrection(unsigned int correction);

    volatile uint8_t * _port;
    volatile uint8_t _stepPin;
    volatile uint8_t _dirPin;

    float acceleration;
    unsigned int minStepInterval;   // ie. max speed, smaller is faster
  
    // derived parameters
    unsigned int c0;                // step interval for first step, determines acceleration
    long stepPosition;              // current position of stepper (total of all movements taken so far)

    // per movement variables (only changed once per movement)
    int dir;                        // current direction of movement, used to keep track of position
    unsigned long totalSteps;        // number of steps requested for current movement
    bool movementDone = false;      // true if the current movement has been completed (used by main program to wait for completion)
    unsigned long rampUpStepCount;   // number of steps taken to reach either max speed, or half-way to the goal (will be zero until this number is known)

    // per iteration variables (potentially changed every interrupt)
    unsigned int n;                 // index in acceleration curve, used to calculate next interval
    float d;                        // current interval length
    unsigned long di;               // above variable truncated
    unsigned long stepCount;         // number of steps completed in current movement
};

#endif
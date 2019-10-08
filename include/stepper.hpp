#ifndef stepper_hpp
#define stepper_hpp

#include <Arduino.h>

enum MoveState
{
    MOVE_STATE_MOVE,
    MOVE_STATE_RUN,
    MOVE_STATE_HOME,
    MOVE_STATE_STOP
};

class Stepper
{
public:
    Stepper(volatile uint8_t *port, uint8_t stepPin, uint8_t dirPin, uint8_t homePin);
    Stepper();

    void resetInfo();
    void reset();
    void setDir(uint8_t dir);
    void doStep();
    bool updateState();
    void decreaseInterval(unsigned int value);

    unsigned int getInterval() { return this->di; }
    void setSteps(unsigned long steps) { this->totalSteps = steps; }
    void setMoveState(uint8_t moveState) {this->moveState = moveState; }
    uint8_t getMoveState() { return this->moveState; }
    
    float acceleration;
    unsigned int minStepInterval;   // ie. max speed, smaller is faster
  
    // derived parameters
    unsigned int c0;                // step interval for first step, determines acceleration
    long stepPosition;              // current position of stepper (total of all movements taken so far)
private:
    void updateMoveState();
    void updateRunState();
    void updateHomeState();

    volatile uint8_t * _port;
    volatile uint8_t _stepPin;
    volatile uint8_t _dirPin;
    volatile uint8_t _homePin;

    uint8_t moveState;

     // per movement variables (only changed once per movement)
    int dir;                        // current direction of movement, used to keep track of position
    unsigned long totalSteps;        // number of steps requested for current movement
    bool movementDone = false;      // true if the current movement has been completed (used by main program to wait for completion)
    
    // per iteration variables (potentially changed every interrupt)
    unsigned int n;                 // index in acceleration curve, used to calculate next interval
    float d;                        // current interval length
    unsigned long di;               // above variable truncated
    unsigned long stepCount;        // number of steps completed in current movement
    unsigned long rampUpStepCount;  // number of steps taken to reach either max speed, 
                                    // or half-way to the goal (will be zero until this number is known)
};

#endif
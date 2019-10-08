#include "stepper.hpp"

Stepper::Stepper()
{
    resetInfo();
    moveState = MOVE_STATE_STOP;
}

Stepper::Stepper(volatile uint8_t *port, uint8_t stepPin, uint8_t dirPin, uint8_t homePin)
{
    Stepper();
    _port = port;
    _stepPin = stepPin;
    _dirPin = dirPin;
    _homePin = homePin;
}

void Stepper::setDir(uint8_t dir)
{
    this->dir = dir;
    if (dir == 0)
        (*_port) = ~(1 << _dirPin) & (*_port);
    else
        (*_port) = (1 << _dirPin) | (*_port);
}

void Stepper::doStep()
{
    (*_port) = (1 << _stepPin) | (*_port);
    (*_port) = ~(1 << _stepPin) & (*_port);
}

void Stepper::resetInfo()
{
    n = 0;
    d = 0;
    di = 0;
    stepCount = 0;
    rampUpStepCount = 0;
    totalSteps = 0;
    stepPosition = 0;
    movementDone = false;
}

void Stepper::reset()
{
    c0 = acceleration;
    d = c0;
    di = d;
    stepCount = 0;
    n = 0;
    rampUpStepCount = 0;
    movementDone = false;
}

bool Stepper::updateState()
{
    switch (this->moveState)
    {
    case MOVE_STATE_MOVE:
        updateMoveState();
        break;

    case MOVE_STATE_RUN:
        updateRunState();
        break;

    case MOVE_STATE_HOME:
        updateHomeState();
        break;
    
    default:
        break;
    }

    di = d;
    
    return movementDone;
}

void Stepper::updateMoveState()
{
    if (stepCount < totalSteps)
    {
        doStep();
        stepCount++;
        stepPosition += dir;

        if (stepCount >= totalSteps)
        {
            movementDone = true;
        }
    }

    if (rampUpStepCount == 0)
    {
        n++;
        d = d - (2 * d) / (4 * n + 1);
        if (d <= minStepInterval)
        {
            d = minStepInterval;
            rampUpStepCount = stepCount;
        }
        if (stepCount >= totalSteps / 2)
        {
            rampUpStepCount = stepCount;
        }
    }
    else if (stepCount >= totalSteps - rampUpStepCount)
    {
        d = (d * (4 * n + 1)) / (4 * n + 1 - 2);
        n--;
    }
}

void Stepper::updateRunState()
{
    doStep();
}

void Stepper::updateHomeState()
{
    doStep();
}

void Stepper::decreaseInterval(unsigned int value)
{
    di -= value;
}
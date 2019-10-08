#include "steppers_controller.hpp"

#define X_PORT PORTB
#define X_DDR DDRB
#define X_DIR_PIN 6
#define X_STEP_PIN 7

#define Y_PORT PORTB
#define Y_DDR DDRB
#define Y_DIR_PIN 4
#define Y_STEP_PIN 5

#define Z_PORT PORTH
#define Z_DDR DDRH
#define Z_DIR_PIN 5
#define Z_STEP_PIN 6

#define A_PORT PORTH
#define A_DDR DDRH
#define A_DIR_PIN 3
#define A_STEP_PIN 4

#define B_PORT PORTE
#define B_DDR DDRE
#define B_DIR_PIN 1
#define B_STEP_PIN 3

#define C_PORT PORTE
#define C_DDR DDRE
#define C_DIR_PIN 4
#define C_STEP_PIN 5

#define TIMER1_INTERRUPTS_ON TIMSK1 |= (1 << OCIE1A)
#define TIMER1_INTERRUPTS_OFF TIMSK1 &= ~(1 << OCIE1A)

SteppersController::SteppersController()
{
    
}

void SteppersController::init()
{
    initPins();
    initTimer();
    initSteppers();

    TIMER1_INTERRUPTS_ON;
}

void SteppersController::initPins()
{
    X_DDR |= (1 << X_DIR_PIN) | (1 << X_STEP_PIN);
    Y_DDR |= (1 << Y_DIR_PIN) | (1 << Y_STEP_PIN);
    Z_DDR |= (1 << Z_DIR_PIN) | (1 << Z_STEP_PIN);
    A_DDR |= (1 << A_DIR_PIN) | (1 << A_STEP_PIN);
    B_DDR |= (1 << B_DIR_PIN) | (1 << B_STEP_PIN);
    C_DDR |= (1 << C_DIR_PIN) | (1 << C_STEP_PIN);
}

void SteppersController::initTimer()
{
    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = 1000;

    TCCR1B |= (1 << WGM12);
    TCCR1B |= ((1 << CS11) | (1 << CS10));

    sei();
}

void SteppersController::initSteppers()
{
    steppers[0] = Stepper(&X_PORT, X_STEP_PIN, X_DIR_PIN, 0);
    steppers[1] = Stepper(&Y_PORT, Y_STEP_PIN, Y_DIR_PIN, 0);
    steppers[2] = Stepper(&Z_PORT, Z_STEP_PIN, Z_DIR_PIN, 0);

    steppers[0].acceleration = 100;
    steppers[0].minStepInterval = 250;

    steppers[1].acceleration = 100;
    steppers[1].minStepInterval = 250;

    steppers[2].acceleration = 100;
    steppers[2].minStepInterval = 250;
}

void SteppersController::move(uint8_t stepper, long steps)
{
    steppers[stepper].setMoveState(MOVE_STATE_MOVE);

    steppers[stepper].setDir(steps < 0 ? 1 : 0);
    steppers[stepper].setSteps( abs(steps) );
    steppers[stepper].reset();

    remainingSteppersFlag |= (1 << stepper);
}

void SteppersController::run(uint8_t stepper, int speed)
{
    steppers[stepper].setMoveState(MOVE_STATE_RUN);

    steppers[stepper].setDir(speed < 0 ? 1 : 0);
    setMinSpeed(stepper, abs(speed));
    setSpeed(stepper, abs(speed));
    steppers[stepper].reset();
    
    remainingSteppersFlag |= (1 << stepper);
}

void SteppersController::home(uint8_t stepper, int speed)
{
    steppers[stepper].setDir(speed < 0 ? 1 : 0);
    setSpeed(stepper, abs(speed));
    steppers[stepper].reset();
    
    steppers[stepper].setMoveState(MOVE_STATE_HOME);
    remainingSteppersFlag |= (1 << stepper);
}

void SteppersController::stop(uint8_t stepper)
{
    steppers[stepper].setMoveState(MOVE_STATE_STOP);
    remainingSteppersFlag &= ~(1 << stepper);
}

void SteppersController::setNextInterruptInterval()
{
    unsigned int mind = UINT16_MAX;

    for (int i = 0; i < NUM_STEPPERS; i++)
    {
        if ( remainingStepper(i) && (steppers[i].getInterval() < mind) )
        {
            mind = steppers[i].getInterval();
        }
    }

    nextStepperFlag = 0;

    for (int i = 0; i < NUM_STEPPERS; i++)
    {
        if ( remainingStepper(i) && (steppers[i].getInterval() == mind) )
        {
            nextStepperFlag |= (1 << i);
        }
    }

    if (remainingSteppersFlag == 0)
    {
        OCR1A = 65500;
    }

    OCR1A = mind;
}

void SteppersController::interruptHandler()
{
    unsigned int tmpCounter = OCR1A;

    OCR1A = 65500;

    for (int number = 0; number < NUM_STEPPERS; number++)
    {
        if ( !remainingStepper(number) )
            continue;

        if ( !nextStepper(number) )
        {
            steppers[number].decreaseInterval(tmpCounter);
            continue;
        }

        if (steppers[number].updateState() == true) // true => movement done
        {
            stop(number);
            Serial.println("stop");
        }
    }

    setNextInterruptInterval();

    TCNT1 = 0;
}

void SteppersController::runAndWait()
{
    setNextInterruptInterval();
    while (remainingSteppersFlag != 0)
    {
        ;
    }
}

void SteppersController::setMinSpeed(uint8_t stepper, unsigned int stepsPerSecond)
{
    double period = 1e6 / stepsPerSecond;
    period = period * 0.25;
    steppers[stepper].acceleration = (unsigned int)period;
}

void SteppersController::setSpeed(uint8_t stepper, unsigned int stepsPerSecond)
{
    double period = 1e6 / stepsPerSecond;
    period = period * 0.25;
    steppers[stepper].minStepInterval = (unsigned int)period;
}

uint8_t SteppersController::remainingStepper(uint8_t number)
{
    return (1 << number) & remainingSteppersFlag;
}

uint8_t SteppersController::nextStepper(uint8_t number)
{
    return (1 << number) & nextStepperFlag;
}
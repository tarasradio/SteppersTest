#include "steppers_controller.hpp"
#include "relays.hpp"
#include "stepper.hpp"
#include "protocol.hpp"

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
#define B_DIR_PIN 4
#define B_STEP_PIN 5

#define C_PORT PORTA
#define C_DDR DDRA
#define C_DIR_PIN 3
#define C_STEP_PIN 1

#define D_PORT PORTA
#define D_DDR DDRA
#define D_DIR_PIN 2
#define D_STEP_PIN 0

#define TIMER1_INTERRUPTS_ON TIMSK1 |= (1 << OCIE1A)
#define TIMER1_INTERRUPTS_OFF TIMSK1 &= ~(1 << OCIE1A)

SteppersController::SteppersController()
{
    _currentMode = HAND_CONTROL;
}

void SteppersController::setControlMode(uint8_t mode)
{
    if(mode == this->_currentMode)
        return;
    this->_currentMode = mode;

    for(uint8_t stepper = 0; stepper < NUM_STEPPERS; stepper++)
    {
        stop(stepper);
    }
}

uint8_t SteppersController::getControlMode()
{
    return this->_currentMode;
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

   // C_DDR |= (1 << C_DIR_PIN) | (1 << C_STEP_PIN);
    //D_DDR |= (1 << D_DIR_PIN) | (1 << D_STEP_PIN);
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
    steppers[3] = Stepper(&A_PORT, A_STEP_PIN, A_DIR_PIN, 0);
    steppers[4] = Stepper(&B_PORT, B_STEP_PIN, B_DIR_PIN, 0);

    //steppers[5] = Stepper(&C_PORT, C_STEP_PIN, C_DIR_PIN, 0);
    //steppers[6] = Stepper(&D_PORT, D_STEP_PIN, D_DIR_PIN, 0);

    for(uint8_t i = 0; i < NUM_STEPPERS; i++)
    {
        steppers[i].acceleration = 100;
        steppers[i].minStepInterval = 250;
    }

    for(uint8_t i = 0; i < NUM_FAKE_STEPPERS; i++)
        fakeSteppers[i] = 0;
}

int SteppersController::tryStepper(int8_t stepper, uint8_t needsRun)
{
    if(stepper >= NUM_FAKE_STEPPERS) 
    {
        return -1;
    }
    
    if( (stepper >= 5) && (fakeSteppers[stepper - 3] == 0) )
    {
        if(needsRun == 1)
            Relays::RelayOn(12 - stepper); // 7, 6, 5
        stepper -= 3;
    }
    else if( (stepper < 5) && (fakeSteppers[stepper + 3] == 0))
    {
        if(needsRun == 1)
            Relays::RelayOff(9 - stepper); // 7, 6, 5
    }
 /*    else if( (stepper >= 10) && (stepper < 12) && (fakeSteppers[stepper - 2] == 0) )
    {
        {String message = "stepper >= 10";
        Protocol::SendMessage(message.c_str());}
        if(needsRun == 1)
            Relays::RelayOn(14 - stepper); // 4, 3
        stepper -= 5;
    }
    else if( (stepper >= 8) && (fakeSteppers[stepper + 2] == 0))
    {
        {
            String message = "stepper >= 8 && < 10 ";
            Protocol::SendMessage(message.c_str());
        }
        
        if(needsRun == 1)
            Relays::RelayOff(12 - stepper); // 4, 3
        stepper -= 3;
    } */
    else
    {
        return -1;
    }

/*     if( (stepper >=  NUM_STEPPERS) && (fakeSteppers[stepper - 5] == 0) )
    {
        if(needsRun == 1)
            Relays::RelayOn(12 - stepper);//????
        stepper -= 5;
    }
    else if( (stepper < NUM_STEPPERS) && (fakeSteppers[stepper + 5] == 0) )
    {
        if(needsRun == 1)
            Relays::RelayOff(9 - stepper);//?????
    }
    else
    {
        return -1;
    } */

    {String message = "real stepper = " + String(stepper);
        Protocol::SendMessage(message.c_str());}
    return stepper;
}

void SteppersController::move(int8_t stepper, long steps, int speed)
{
    int8_t realStepper = tryStepper(stepper, 1);
    if(realStepper < 0)
        return;

    fakeSteppers[stepper] = 1;
    
    steppers[realStepper].setMoveState(MOVE_STATE_MOVE);

    steppers[realStepper].setDir(steps < 0 ? 1 : 0);
    steppers[realStepper].setSteps( abs(steps) );
    setSpeed(realStepper, abs(speed));
    steppers[realStepper].reset();

    remainingSteppersFlag |= (1 << realStepper);
}

void SteppersController::run(int8_t stepper, int speed)
{
    int8_t realStepper = tryStepper(stepper, 1);
    if(realStepper < 0)
        return;

    fakeSteppers[stepper] = 1;
    
    steppers[realStepper].setMoveState(MOVE_STATE_RUN);

    steppers[realStepper].setDir(speed < 0 ? 1 : 0);
    setMinSpeed(realStepper, abs(speed));
    setSpeed(realStepper, abs(speed));
    steppers[realStepper].reset();
    
    remainingSteppersFlag |= (1 << realStepper);
}

void SteppersController::home(int8_t stepper, int speed)
{
    int8_t realStepper = tryStepper(stepper, 1);
    if(realStepper < 0)
        return;

    fakeSteppers[stepper] = 1;
    
    steppers[realStepper].setDir(speed < 0 ? 1 : 0);
    setSpeed(realStepper, abs(speed));
    steppers[realStepper].reset();
    
    steppers[realStepper].setMoveState(MOVE_STATE_HOME);
    remainingSteppersFlag |= (1 << realStepper);
}

void SteppersController::stop(int8_t stepper)
{
    int8_t realStepper = tryStepper(stepper, 0);
    if(realStepper < 0)
        return;

    fakeSteppers[stepper] = 0;

    steppers[realStepper].setMoveState(MOVE_STATE_STOP);
    remainingSteppersFlag &= ~(1 << realStepper);
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

void SteppersController::setMinSpeed(int8_t stepper, unsigned int stepsPerSecond)
{
    double period = 1e6 / stepsPerSecond;
    period = period * 0.25;
    steppers[stepper].acceleration = static_cast<unsigned int>(period);
}

void SteppersController::setSpeed(int8_t stepper, unsigned int stepsPerSecond)
{
    double period = 1e6 / stepsPerSecond;
    period = period * 0.25;
    steppers[stepper].minStepInterval = static_cast<unsigned int>(period);
}

uint8_t SteppersController::remainingStepper(uint8_t number)
{
    return (1 << number) & remainingSteppersFlag;
}

uint8_t SteppersController::nextStepper(uint8_t number)
{
    return (1 << number) & nextStepperFlag;
}

void SteppersController::PrintSteppers()
{
    for(uint8_t i = 0; i < 8; i++)
    {
        Serial.print(" S" + String(i) + ": " + String(fakeSteppers[i]));
    }
    Serial.println();
}
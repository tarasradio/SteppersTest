#include "steppers.hpp"

#define X_PORT PORTB
#define X_DDR DDRB
#define X_DIR_PIN 6
#define X_STEP_PIN 7

#define X_STEP_UP X_PORT |= (1 << X_STEP_PIN)
#define X_STEP_DOWN X_PORT &= ~(1 << X_STEP_PIN)

#define Y_PORT PORTB
#define Y_DDR DDRB
#define Y_DIR_PIN 4
#define Y_STEP_PIN 5

#define Y_STEP_UP Y_PORT |= (1 << Y_STEP_PIN)
#define Y_STEP_DOWN Y_PORT &= ~(1 << Y_STEP_PIN)

#define Z_PORT PORTH
#define Z_DDR DDRH
#define Z_DIR_PIN 5
#define Z_STEP_PIN 6

#define Z_STEP_UP Z_PORT |= (1 << Z_STEP_PIN)
#define Z_STEP_DOWN Z_PORT &= ~(1 << Z_STEP_PIN)

#define A_PORT PORTH
#define A_DDR DDRH
#define A_DIR_PIN 3
#define A_STEP_PIN 4

#define A_STEP_UP A_PORT |= (1 << A_STEP_PIN)
#define A_STEP_DOWN A_PORT &= ~(1 << A_STEP_PIN)

#define B_PORT PORTE
#define B_DDR DDRE
#define B_DIR_PIN 1
#define B_STEP_PIN 3

#define B_STEP_UP B_PORT |= (1 << B_STEP_PIN)
#define B_STEP_DOWN B_PORT &= ~(1 << B_STEP_PIN)

#define C_PORT PORTE
#define C_DDR DDRE
#define C_DIR_PIN 4
#define C_STEP_PIN 5

#define C_STEP_UP C_PORT |= (1 << C_STEP_PIN)
#define C_STEP_DOWN C_PORT &= ~(1 << C_STEP_PIN)

#define TIMER1_INTERRUPTS_ON TIMSK1 |=  (1 << OCIE1A)
#define TIMER1_INTERRUPTS_OFF TIMSK1 &= ~(1 << OCIE1A)

void xStep() { X_STEP_UP; X_STEP_DOWN; }
void xDir(byte dir)
{
    if(dir == 0) X_PORT &= ~(1 << X_DIR_PIN);
    else X_PORT |= (1 << X_DIR_PIN);
}

void yStep() { Y_STEP_UP; Y_STEP_DOWN; }
void yDir(byte dir)
{
    if(dir == 0) Y_PORT &= ~(1 << Y_DIR_PIN);
    else Y_PORT |= (1 << Y_DIR_PIN);
}

void zStep() { Z_STEP_UP; Z_STEP_DOWN; }
void zDir(byte dir)
{
    if(dir == 0) Z_PORT &= ~(1 << Z_DIR_PIN);
    else Z_PORT |= (1 << Z_DIR_PIN);
}

void aStep() { A_STEP_UP; A_STEP_DOWN; }
void aDir(byte dir)
{
    if(dir == 0) A_PORT &= ~(1 << A_DIR_PIN);
    else A_PORT |= (1 << A_DIR_PIN);
}

void bStep() { B_STEP_UP; B_STEP_DOWN; }
void bDir(byte dir)
{
    if(dir == 0) B_PORT &= ~(1 << B_DIR_PIN);
    else B_PORT |= (1 << B_DIR_PIN);
}

void cStep() { C_STEP_UP; C_STEP_DOWN; }
void cDir(byte dir)
{
    if(dir == 0) C_PORT &= ~(1 << C_DIR_PIN);
    else C_PORT |= (1 << C_DIR_PIN);
}

void Steppers::init()
{
    initPins();
    initTimer();
    initSteppers();

    TIMER1_INTERRUPTS_ON;
}

void Steppers::initPins()
{
    X_DDR |= (1 << X_DIR_PIN) | (1 << X_STEP_PIN);
    Y_DDR |= (1 << Y_DIR_PIN) | (1 << Y_STEP_PIN);
    Z_DDR |= (1 << Z_DIR_PIN) | (1 << Z_STEP_PIN);
    A_DDR |= (1 << A_DIR_PIN) | (1 << A_STEP_PIN);
    B_DDR |= (1 << B_DIR_PIN) | (1 << B_STEP_PIN);
    C_DDR |= (1 << C_DIR_PIN) | (1 << C_STEP_PIN);
}

void Steppers::initTimer()
{
    cli();

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    OCR1A = 1000;

    TCCR1B |= (1 << WGM12);
    TCCR1B |= ((1 << CS11) | (1 << CS10));

    sei();
}

void Steppers::initSteppers()
{
  steppers[0].dirFunc = xDir;
  steppers[0].stepFunc = xStep;
  steppers[0].acceleration = 1000;
  steppers[0].minStepInterval = 250;

  steppers[1].dirFunc = yDir;
  steppers[1].stepFunc = yStep;
  steppers[1].acceleration = 100;
  steppers[1].minStepInterval = 250;
}

void Steppers::move(byte stepper, long steps)
{
  steppers[stepper].dirFunc( steps < 0 ? 1 : 0 );
  steppers[stepper].dir = steps > 0 ? 1 : -1;
  steppers[stepper].totalSteps = abs(steps);
  steppers[stepper].reset();
  remainingSteppersFlag |= (1 << stepper);
}

void Steppers::setNextInterruptInterval() 
{
  unsigned int mind = UINT16_MAX;
  for (int i = 0; i < NUM_STEPPERS; i++) 
  {
    if ( ((1 << i) & remainingSteppersFlag) && steppers[i].di < mind ) 
    {
      mind = steppers[i].di;
    }
  }

  nextStepperFlag = 0;

  for (int i = 0; i < NUM_STEPPERS; i++) 
  {
    if ( ((1 << i) & remainingSteppersFlag) && steppers[i].di == mind )
      nextStepperFlag |= (1 << i);
  }

  if ( remainingSteppersFlag == 0 ) 
  {
    OCR1A = 65500;
  }

  OCR1A = mind;
}

void Steppers::interruptHandler()
{
  unsigned int tmpCounter = OCR1A;

  OCR1A = 65500;

  for (int i = 0; i < NUM_STEPPERS; i++) 
  {

    if ( ! ((1 << i) & remainingSteppersFlag) )
      continue;

    if ( ! (nextStepperFlag & (1 << i)) ) 
    {
      steppers[i].setCorrection(tmpCounter);
      continue;
    }

    if(steppers[i].updateState() == true) // true => movement done
    {
        remainingSteppersFlag &= ~(1 << i);
    }
  }

  setNextInterruptInterval();

  TCNT1  = 0;
}

void Steppers::runAndWait() 
{
  setNextInterruptInterval();
  while ( remainingSteppersFlag != 0){;}
}


void Steppers::setMinSpeed(byte stepper, unsigned int stepsPerSecond)
{
    double period = 1e6 / stepsPerSecond;
    period = period * 0.25;
    steppers[stepper].acceleration = (unsigned int) period;
}

void Steppers::setSpeed(byte stepper, unsigned int stepsPerSecond)
{
    double period = 1e6 / stepsPerSecond;
    period = period * 0.25;
    steppers[stepper].minStepInterval = (unsigned int) period;
}
//_____D1_class_Statemachine.cpp______________181002-181002_____
// The class Statemachine helps to make a simple state counter.
// It counts from 1 to state_max (incl.) and waits
// state_delay milliseconds on every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified 2018-10-05: some set/get added
// Released into the public domain.
#include "D1_class_Statemachine.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor______________________________________________
Statemachine::Statemachine() { setup(); }

Statemachine::Statemachine(int state_max, int state_delay)
{
 setup();
 setStateMax(state_max);
 setStateDelay(state_delay);
}

//_____setup properties_________________________________________
void Statemachine::setup() {
 stateMax=STATE_MAX_DEFAULT;
 stateDelay=STATE_DELAY_DEFAULT;
 stateCounter=STATE_ONE;
 beginMillis=millis();
}

//**************************************************************
//     set/get values
//**************************************************************
void Statemachine::setStateMax(int state_max)
{
 if(state_max>1) stateMax=state_max;
}

void Statemachine::setStateDelay(int state_delay)
{
 if(stateDelay>=0) stateDelay=state_delay;
}

int Statemachine::getStateMax()   { return stateMax; }
int Statemachine::getStateDelay() { return stateDelay; }
int Statemachine::getState()      { return stateCounter; }
int Statemachine::getDuration() {return(millis()-beginMillis);}

//**************************************************************
//     working methods
//**************************************************************
int Statemachine::loopBegin()
{
 beginMillis=millis();                       // get start "time"
 return stateCounter;
}

unsigned long Statemachine::loopEnd()
{
 if((++stateCounter)>stateMax)              // increment state
  stateCounter=STATE_ONE;                   // start with 1st again
 long loopDelay=stateDelay-(millis()-beginMillis); // rest delay
 if(loopDelay<0) loopDelay=0;               // time >= 0
 delay(loopDelay);                          // wait
 return(millis()-beginMillis);
}

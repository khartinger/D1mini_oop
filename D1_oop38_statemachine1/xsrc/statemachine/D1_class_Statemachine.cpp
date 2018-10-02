//_____D1_class_Statemachine.cpp______________181002-181002_____
// The class Statemachine helps to make a simple state counter.
// It counts from 1 to state_max (incl.) and waits
// state_delay milliseconds on every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified 20..-..-..: -
// Released into the public domain.
#include "src/statemachine/D1_class_Statemachine.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor______________________________________________
Statemachine::Statemachine() { setup(); }

Statemachine::Statemachine(int state_max, state_delay)
{
 setup();
 if(state_max>1) statemax=state_max;
 if(state_delay>=0) statedelay=state_delay;
}

//_____setup properties_________________________________________
void Statemachine::setup() {
 statemax=STATE_MAX;
 statedelay=STATE_DELAY;
 statecounter=STATE_ONE;
 loop_millis=millis();
}

//**************************************************************
//     get values
//**************************************************************

int Statemachine::getState() { return state; }
int Statemachine::getDuration() {return(millis()-loop_millis);}
int Statemachine::loopBegin()
{
 loop_millis=millis();                      // get start "time"
}

unsigned long Statemachine::loopEnd()
{
 if((++statecounter)>statemax)              // increment state
  statecounter=STATE_ONE;                   // start with 1st again
 loop_millis=millis()-loop_millis;          // loop duration
 int loop_delay=state_delay-loop_millis;    // rest delay
 if(loop_delay<0) loop_delay=0;             // time >= 0
 delay(loop_delay);                         // wait
}

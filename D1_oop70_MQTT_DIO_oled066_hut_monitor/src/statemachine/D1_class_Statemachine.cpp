//_____D1_class_Statemachine.cpp______________181002-200119_____
// The class Statemachine helps to make a simple state counter.
// It counts from 1 to state_max (incl.) and waits
// state_delay milliseconds on every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified 2018-10-05: some set/get added
//          2018-10-19: stateMin, add() added
//          2020-01-19: getBeginMillis() added
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
 stateCounter=stateMin;
}

Statemachine::Statemachine(int state_min, int state_max, int state_delay)
{
 setup();
 if(state_min>state_max) 
  { int temp=state_min; state_min=state_max; state_max=temp; }
 setStateMin(state_min);
 setStateMax(state_max);
 setStateDelay(state_delay);
 stateCounter=stateMin;
}

//_____setup properties_________________________________________
void Statemachine::setup() {
 stateMin=STATE_ONE;
 stateMax=STATE_LAST;
 stateDelay=STATE_DELAY_DEFAULT;
 stateCounter=stateMin;
 beginMillis=millis();
}

//**************************************************************
//     set/get values
//**************************************************************
bool Statemachine::setStateMin(int state_min)
{
 if(state_min<=stateMax){ stateMin=state_min; return true; }
 return false;
}

bool Statemachine::setStateMax(int state_max)
{
 if(state_max>=stateMin){ stateMax=state_max; return true; }
 return false;
}

bool Statemachine::setStateDelay(int state_delay)
{
 if(stateDelay>=0) { stateDelay=state_delay; return true; }
 return false;
}

bool Statemachine::setState(int new_state)
{
 if((new_state>=stateMin)&&(new_state<=stateMax))
 { stateCounter=new_state; return true; }
 return false;
}

int Statemachine::getStateMax()   { return stateMax; }
int Statemachine::getStateDelay() { return stateDelay; }
int Statemachine::getState()      { return stateCounter; }
int Statemachine::getDuration() {return(millis()-beginMillis);}
unsigned long Statemachine::getBeginMillis(){ return beginMillis; } 

//**************************************************************
//     working methods
//**************************************************************
//_____use this at the beginning of the loop-function___________
// returns the number of the actual state
int Statemachine::loopBegin()
{
 beginMillis=millis();                 // get start "time"
 return stateCounter;
}

//_____use this at the end of the loop-function_________________
// increases state number and returns duration of last state
unsigned long Statemachine::loopEnd()
{
 stateCounter=this->add(1);
 unsigned long endMillis=millis();
 unsigned long duration=0xFFFFFFFF;    // -1=0xFFFFFFFF
 if(endMillis>=beginMillis) duration=endMillis-beginMillis;
 else duration=endMillis+(duration-beginMillis);
 unsigned long loopDelay=0;
 if(stateDelay>duration) loopDelay=stateDelay-duration;
 delay(loopDelay);                     // wait
 return duration;
}

//_____Add a number of states to actual state___________________
// return: the new state number
// Note: Method dDoes NOT change the stateCounter!
int Statemachine::add(int numberOfStates)
{
 if(numberOfStates<=0) return stateCounter;
 int ret=stateCounter+numberOfStates;       // increment
 if(ret>stateMax)
 {
  int numberAllStates=stateMax-stateMin+1;
  int add2min=(ret-stateMax-1)%numberAllStates;
  ret=stateMin+add2min;
 }
 return ret;
}

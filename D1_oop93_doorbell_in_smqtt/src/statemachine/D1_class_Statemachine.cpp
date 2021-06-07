//_____D1_class_Statemachine.cpp______________181002-200405_____
// The class Statemachine helps to build a state counter.
// It counts from 1 to stateMax (incl.) and waits
// "stateDelay" milliseconds for every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified
// 2018-10-05 some set/get added
// 2018-10-19 stateMin, add() added
// 2020-01-19 getBeginMillis() added
// 2020-02-25 getStateMin() added
// 2020-03-03 add(..,..), diff() added
// 2020-04-05 add getBeginMillis()
// 2021-01-31 state: change int to int32_t
// 2021-04-11 add isDelayed(), delayed (ms)
// Released into the public domain.
#include "D1_class_Statemachine.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor______________________________________________
Statemachine::Statemachine() { setup(); }

Statemachine::Statemachine(int32_t state_max, int32_t state_delay)
{
 setup();
 setStateMax(state_max);
 setStateDelay(state_delay);
 stateCounter=stateMin;
}

Statemachine::Statemachine(int32_t state_min, int32_t state_max, int32_t state_delay)
{
 setup();
 if(state_min>state_max) 
  { int32_t temp=state_min; state_min=state_max; state_max=temp; }
 stateMin=state_min;
 stateMax=state_max;
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
 delayed=0;
}

//**************************************************************
//     set values
//**************************************************************

//_____set first state (=minimum), if <= state maximum__________
bool Statemachine::setStateMin(int32_t state_min)
{
 if(state_min<=stateMax){
  stateMin=state_min; 
  if(stateCounter<stateMin) stateCounter=stateMin;
  return true; 
 }
 return false;
}

//_____set last state (=maximum), if >= state minimum___________
bool Statemachine::setStateMax(int32_t state_max)
{
 if(state_max>=stateMin){
  stateMax=state_max; 
  if(stateCounter>stateMax) stateCounter=stateMax;
  return true; 
 }
 return false;
}

//_____set state delay (if >=0)_________________________________
bool Statemachine::setStateDelay(int32_t state_delay)
{
 if(stateDelay>=0) { stateDelay=state_delay; return true; }
 return false;
}

//_____set state (convert to range stateMin...stateMax)_________
bool Statemachine::setState(int32_t new_state)
{
 int32_t numberAllStates=stateMax-stateMin+1;
 //-----transform new_state to range stateMin...stateMax--------
 if(new_state>stateMax) new_state=stateMin+((new_state-stateMax-1)%numberAllStates);
 if(new_state<stateMin) new_state=stateMax-((stateMin-new_state-1)%numberAllStates);
 stateCounter=new_state;
 return true;
} 

//**************************************************************
//     get values
//**************************************************************

int32_t Statemachine::getStateMin()   { return stateMin; }
int32_t Statemachine::getStateMax()   { return stateMax; }
int32_t Statemachine::getStateDelay() { return stateDelay; }
int32_t Statemachine::getState()      { return stateCounter; }
int32_t Statemachine::getDuration() {return(millis()-beginMillis);}
uint32_t Statemachine::getBeginMillis(){ return beginMillis; } 

//**************************************************************
//     working methods
//**************************************************************

//_____use this at the beginning of the loop-function___________
// returns the number of the actual state
int32_t Statemachine::loopBegin()
{
 beginMillis=millis();                 // get start "time"
 return stateCounter;
}

//_____use this at the end of the loop-function_________________
// increases state number and returns duration of last state
uint32_t Statemachine::loopEnd()
{
 stateCounter=this->add(1);
 uint32_t endMillis=millis();
 uint32_t duration=0xFFFFFFFF;    // -1=0xFFFFFFFF
 //------duration of this state---------------------------------
 if(endMillis>=beginMillis) duration=endMillis-beginMillis;
 else duration=endMillis+(duration-beginMillis);
 uint32_t loopDelay=0;
 if(stateDelay>(duration+delayed))
 {//-----wait a little bit--------------------------------------
  loopDelay=stateDelay-duration-delayed;
  delay(loopDelay);                     // wait
  delayed=0;
 } 
 else 
 {//-----state delayed------------------------------------------
  delayed+=duration-stateDelay;
 }
 return duration;
}

//_____Add a number of states to actual state___________________
// return: the new state number
// Note: Method does NOT change the stateCounter!
int32_t Statemachine::add(int32_t numberOfStates)
{
 return add(stateCounter, numberOfStates);
}

//_____Add a number of states to given state____________________
// return: the new state number
// Note: Method does NOT change the stateCounter!
int32_t Statemachine::add(int32_t state, int32_t numberOfStates)
{
 int32_t ret=state;
 if(numberOfStates==0) return ret;
 int32_t numberAllStates=stateMax-stateMin+1;
 if(numberOfStates<0) 
  numberOfStates=numberAllStates-(-numberOfStates)%numberAllStates;
 //-----transform state to range stateMin...stateMax------------
 if(ret>stateMax)ret=stateMin+((ret-stateMax-1)%numberAllStates);
 if(ret<stateMin)ret=stateMax-((stateMin-ret-1)%numberAllStates);
 //-----transform numberOfStates to range 0...(Max-Min+1)-------
 if(numberOfStates>=0)
  numberOfStates=numberOfStates%numberAllStates;
 else
  numberOfStates=numberAllStates-((-numberOfStates)%numberAllStates);
 //-----add value to state--------------------------------------
 ret=ret+numberOfStates;
 if(ret>stateMax) ret=ret-numberAllStates;
 if(ret<stateMin) ret=ret+numberAllStates;
 return ret;
}

//_____return difference between stateCounter and old_state_____
// return difference as POSITIVE value e.g. states 11..17
// stateCounter=16, old_state=12: diff=16-12=4
// stateCounter=12, old_state=16: diff=12-16+(17-11+1)=3
// on error: return -1;
int32_t Statemachine::diff(int32_t oldState)
{
 int32_t numberAllStates=stateMax-stateMin+1; 
 //-----transform oldState to range stateMin...stateMax---------
 if(oldState>stateMax) oldState=stateMin+((oldState-stateMax-1)%numberAllStates);
 if(oldState<stateMin) oldState=stateMax-((stateMin-oldState-1)%numberAllStates);
 int32_t ret=stateCounter-oldState;
 if(ret>=0) return ret;
 return ret+numberAllStates;
}

//_____is start of this state delayed?__________________________
bool Statemachine::isDelayed() { 
 if(delayed==0) return false;
 return true;
}
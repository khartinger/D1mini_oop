//_____D1_class_Statemachine.h________________181002-200405_____
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
// Released into the public domain.
#ifndef D1_CLASS_STATEMACHINE_H
#define D1_CLASS_STATEMACHINE_H
#include "Arduino.h"
#define STATE_ONE                1     // 1st value of state
#define STATE_LAST              10     // last value of states
#define STATE_DELAY_DEFAULT    200     // state delay in ms
class Statemachine {
 protected:
  //-----properties---------------------------------------------
  int stateCounter;                    // counts from min to max
  int stateMin;                        // 1st counter value
  int stateMax;                        // last vounter value
  int stateDelay;                      // delay for one state
  unsigned long beginMillis;           // start time of state
 public:
  //-----constructor & co---------------------------------------
  Statemachine();                      // use default values
  Statemachine(int state_max, int state_delay);
  Statemachine(int state_min, int state_max, int state_delay);
 protected:
  void setup();                        // setup properties
 public:
  //-----set values---------------------------------------------
  bool setStateMin(int state_min);     //
  bool setStateMax(int state_max);     //
  bool setStateDelay(int state_delay); //
  bool setState(int new_state);        //
  //-----get values---------------------------------------------
  int  getStateMin();                  //
  int  getStateMax();                  //
  int  getStateDelay();                //
  int  getState();                     //
  int  getDuration();                  //
  unsigned long getBeginMillis();      //
  //-----working methods----------------------------------------
  int  loopBegin();                    //
  unsigned long loopEnd();             //
  int  add(int numberOfStates);        // return(stateCounter+num)
  int  add(int state, int numberOfStates); // return(state+num)
  int  diff(int oldState);             // return(stateCounter-old)
};
#endif

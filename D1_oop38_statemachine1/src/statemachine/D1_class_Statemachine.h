//_____D1_class_Statemachine.h________________181002-181019_____
// The class Statemachine helps to make a simple state counter.
// It counts from 1 to state_max (incl.) and waits
// state_delay milliseconds on every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified 2018-10-05: some set/get added
// Modified 2018-10-19: stateMin, add() added
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
  Statemachine();
  Statemachine(int state_max, int state_delay);
  Statemachine(int state_min, int state_max, int state_delay);
 protected:
  void setup();                        // setup properties
 public:
  //-----set/get values-----------------------------------------
  bool setStateMin(int state_min);     //
  bool setStateMax(int state_max);     //
  bool setStateDelay(int state_delay); //
  bool setState(int new_state);        //
  int  getStateMax();                  //
  int  getStateDelay();                //
  int  getState();                     //
  int  getDuration();                  //
  //-----working methods----------------------------------------
  int  loopBegin();                    //
  unsigned long loopEnd();             //
  int  add(int numberOfStates);        //
};
#endif

//_____D1_class_Statemachine.h________________181002-181002_____
// The class Statemachine helps to make a simple state counter.
// It counts from 1 to state_max (incl.) and waits
// state_delay milliseconds on every state.
// Created by Karl Hartinger, October 02, 2018.
// Modified 20..-..-..: -
// Released into the public domain.
#ifndef D1_CLASS_STATEMACHINE_H
#define D1_CLASS_STATEMACHINE_H
#include "Arduino.h"
#define STATE_ONE                1     // 1st number of state
#define STATE_MAX_DEFAULT       10     // max number of states
#define STATE_DELAY_DEFAULT    200     // state delay in ms
class Statemachine {
 protected:
  //-----properties---------------------------------------------
  int stateCounter;
  int stateMax, stateDelay;
  unsigned long beginMillis;
 public:
  //-----constructor & co---------------------------------------
  Statemachine();
  Statemachine(int state_max, int state_delay);
 protected:
  void   setup();                 // setup properties
 public:
  //-----set/get values-----------------------------------------
  void   setStateDelay(int stateDelay);
  int    getState();
  int    getStateMax();
  int    getDuration();
  int    loopBegin();
  unsigned long loopEnd();
};
#endif

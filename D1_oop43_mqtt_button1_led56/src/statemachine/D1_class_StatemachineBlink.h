﻿//_____D1_class_StatemachineBlink.h___________200224-200227_____
// The class StatemachineBlink helps to blink a LED in a state 
// machine. You need to know...
// ... the pin number, where the LED is wired (e.g. D4)
// ... LED is on, if pin voltage is 3V3 or 0V (= inverted)
// ... the state number when LED should start blinking
// ... the duration for LED is on (in states)
// ... the duration for LED is off (in states)
// ... the number of blinks (-1 = endless)
// Created by Karl Hartinger, February 24, 2020.
// Modified 200227: Add setStateStart()
// Released into the public domain.
#ifndef D1_CLASS_STATEMACHINEBLINK_H
#define D1_CLASS_STATEMACHINEBLINK_H
#include "Arduino.h"
#include "D1_class_Statemachine.h"

#define SMB_DEBUG            false     // true
#define SMB_PIN                 D4     // pin number blue LED
#define SMB_INVERT            true     // 0=LED on, 1 =LED off
#define SMB_STATE_START          1     // 1. state: led on 
#define SMB_STATES_ON            2     // e.g. 2x100ms = 0.2s
#define SMB_STATES_OFF           8     // e.g. 8x100ms = 0.8s
#define SMB_BLINK_ENDLESS       -1     // -1=blink endless

class StatemachineBlink {
 protected:
  //-----properties---------------------------------------------
  int  pin_;                           // led pin number
  bool invert_;                        // if true: 0=LED on
  int  stateStart_;                    // num of state LED start
  int  statesOn_;                      // num of states LED on
  int  statesOff_;                     // num of states LED off
  int  blinkMax_;                      // max. number of LED on
  int  blinkCounter_;                  // act. number of LED on
  int  nextStateOn_;                   // num of next state "on"
  int  nextStateOff_;                  // num of next state "off"
  bool restart_;                       // true: do restart
  bool started_;                       // true: blinking started
  bool off_;                           // true: LED is off 
 public:
  //-----constructor & co---------------------------------------
  StatemachineBlink();
  StatemachineBlink(int pin, bool invert,
   int stateStart, int statesOn, int statesOff, int blinkNum);
 protected:
  void setup();                        // setup properties
 public:
  //-----set/get values-----------------------------------------
  void setStateStart(int stateStart);  //
  void setParams(int stateStart, int statesOn, int statesOff, int blinkNum);
  //-----working methods----------------------------------------
  int  doBlink(Statemachine stm);      // control LED
  void off();                          // turn LED off
  void on();                           // allow blinking
};
#endif

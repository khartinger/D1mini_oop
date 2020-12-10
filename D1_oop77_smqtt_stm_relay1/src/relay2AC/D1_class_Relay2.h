//_____D1_class_Relay2.h______________________181026-201104_____
// D1 mini class for switching a relay and measuring
// the current (by Analog In)
// Class can be used with or without measuring the current:
// on, off, toggle ............... switch the relay directly
// lampOn, lampOff, lampToggle ... measure current, switch relay
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) Current transformer (ASM-010) and selfmade 
//               shield D1_INA122_V2 180924 at Analog In
//
// Uses class Ain (files D1_class_Ain.*)
// Created by Karl Hartinger, November 11, 2018.
// Updates:
// 2020-01-03 separate relay and lamp function
// 2020-11-04 setADCmax(int ADCmax) added
// Released into the public domain.

#ifndef D1_CLASS_RELAY2_H
#define D1_CLASS_RELAY2_H
#define RELAY2_PIN_RELAY         D1    // default pin
#include "Arduino.h"                   // D1

class Relay2 {
 //-----properties----------------------------------------------
 protected:
  int   pinRelay_;                     // relay pin number
  int   relay_;                        // output: 0=0V, 1=3V3
  bool  changed_;                      // reset: call changed()
  bool  invert_;                       // invert output voltage
 //-----constructor & co----------------------------------------
 public:
  Relay2();
  Relay2(int pinRelais);
  Relay2(int pinRelais, bool invert);
 protected:
  void setup(int pin, bool invert);
  //-----setter methods-----------------------------------------
 public: 
  bool   on();                         // set relay on (calls set)
  bool   off();                        // set relay off (calls set)
  bool   toggle();                     // change output (calls set)
  bool   set(int val);                 // set relay 1=on or 0=off
  bool   setPinRelay(int pin);         // set (new) relais pin
  bool   setInvertRelay(bool invert); // invert output voltage
  //-----getter methods-----------------------------------------
  bool   isOn();                       // true: relay is on
  bool   isOff();                      // true: relay is off
  bool   isChange();                   // change(s) since last call?
  int    getSwitchStatus();            // relay state 1=on, 0=off
};
#endif

//_____D1_class_Relay1.h______________________181026-200103_____
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
// Modified: 2020-01-03 separate relay and lamp function
// Released into the public domain.

#ifndef D1_CLASS_RELAY1_H
#define D1_CLASS_RELAY1_H
#define RELAY1_PIN_RELAY         D1    // default pin
#define RELAY1_MA_MAX          2000    // 2000mA=1000
#define RELAY1_MA_LAMP_ON        44    // I=P/U=10W/230V=44mA
#define RELAY1_DELAY_MS          30    // wait after relay set
#define MEASURE_DELAY_MS        100    // wait before i measure
#include "Arduino.h"                   // D1
#include "../ain/D1_class_Ain.h"       // current measuring

class Relay1 {
 //-----properties----------------------------------------------
 protected:
  int   pinRelay_;                     // relay pin number
  int   relay_;                        // output: 0=0V, 1=3V3
  int   lampstate_;                    // lamp 1=on|0=off
  bool  changed_;                      // reset: call changed()
  float mA_;                           // actual current in mA
  float mA_max_;                       // 1000 digit = max
  float mA_lamp_on_;                   // current limit for "on"
  Ain   ain;                           // object measure current
  unsigned long lastMilli_;            // ms since last change
 //-----constructor & co----------------------------------------
 public:
  Relay1();
  Relay1(int pinRelais);
  Relay1(int pinRelais, float imax_mA, float ion_mA);
 protected:
  void setup(int pin, float imax_mA, float ion_mA);
  void measuring();                    // measures mA_, lamp_
 //-----setter, getter, working methods-------------------------
 public: 
  bool   set(int val);                 // set lamp on or off
  bool   setPinRelay(int pin);         // set (new) relais pin
  void   setParams(int pinRelais, float imax_mA, float ion_mA);
  void   setCurrentOn(float mA);       // current limit for "on"
  void   setCurrentMax(float mA);      // max. current =1023 AIN
  bool   on(void);                     // logical on
  bool   off(void);                    // logical off
  bool   toggle();                     // change output
  bool   lampOn(void);                 // (try to) turn lamp on
  bool   lampOff(void);                // (try to) turn lamp off
  bool   lampToggle();                 // (try to) toggle lamp
  bool   changed();                    // true=lamp state changed
  int    getRelaystate();              // 0=off, 1=on
  int    getLampstate();               // lamp 0(=off) | 1(=on)
  float  getCurrent();                 // lamp current [mA]
  float  getCurrentOn();               // limit for lamp on
  float  getCurrentMax();              // A/D current limit
  String getsStatusOled();             // status text (10chars)
  unsigned long getDuration();         // msec since last change
};
#endif

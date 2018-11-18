//_____D1_class_Relais1.h______________________181026-181111_____
// D1 mini class for switching a relais and measuring
// the current.
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) D1_INA122_V2 180924 (Selfmade) or Analog in
//               and Current transformer ASM-010
//
// Created by Karl Hartinger, November 11, 2018.
// Released into the public domain.

#ifndef D1_CLASS_RELAIS1_H
#define D1_CLASS_RELAIS1_H
#define RELAIS1_PIN_RELAIS        D1   // default pin
#define RELAIS1_MA_MAX          2000   // 2000mA=1000
#define RELAIS1_MA_LAMP_ON      1000   // I=P/U=10W/230V=44mA
#define RELAIS1_DELAY_MS          50   // wait after set lamp
#include "Arduino.h"                   // D7
#include "../ain/D1_class_Ain.h"       // current measuring

class Relais1 {
 //-----properties----------------------------------------------
 protected:
  int   pinRelais_;                    // relais pin number
  int   relais_;                       // output: 0=0V, 1=3V3
  int   lampstate_;                    // lamp 1=on|0=off
  float mA_;                           // actual current in mA
  float mA_max_;                       // 1000 digit = max
  float mA_lamp_on_;                   // current limit for on
  Ain   ain;                           // object measure current
  unsigned long lastMilli_;            // ms since last change
 //-----constructor & co----------------------------------------
 public:
  Relais1();
  Relais1(int pinRelais);
 protected:
  void setup(int pin, float imax, float ion); // 
  void measuring();                    // measures mA_, lamp_
 //-----setter, getter, working methods-------------------------
 public: 
  bool   set(int val);                 // set lamp on or off
  bool   setPinRelais(int pin);        // set relais pin (D7)
  void   setParams(int pinRelais, float imax, float ion);
  void   setCurrentOn(float mA);       // current limit for on
  bool   on(void);                     // logical on
  bool   off(void);                    // logical off
  bool   toggle();                     // change output
  bool   changed();                    // true=lamp state changed
  int    getLampstate();               // lamp 0(=off) | 1(=on)
  float  getCurrent();                 // lamp current [mA]
  float  getCurrentOn();               // limit for lamp on
  float  getCurrentMax();              // A/D current limit
  String getsStatusOled();             // status text (10chars)
  unsigned long getDuration();         // msec since last change
};
#endif

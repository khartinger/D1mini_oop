//_____D1_class_Relay2AC.h____________________181026-201107_____
// D1 mini class for switching a relay and measuring
// the current (by Analog In)
// Class can be used with or without measuring the current:
// on, off, toggle ............... switch the relay directly
//xxxxxxxxxxx lampOn, lampOff, lampToggle ... measure current, switch relay
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) Current transformer (ASM-010) and selfmade 
//               shield D1_INA122_V3 at Analog In
//
// Uses classes Relay2 and CurrentAC !
// (files D1_class_Relay2.* and D1_class_CurrentAC.*)
// Created by Karl Hartinger, December 07, 2020.
// Updates:
// 2020-12-07 New
// Released into the public domain.

#ifndef D1_CLASS_RELAY2AC_H
#define D1_CLASS_RELAY2AC_H
#define RELAY2AC_BOARD_DEFAULT      1  // 1=D1mini (default)
#define RELAY2AC_BOARD_ESP32D1      2  // 2 = ESP32 D1 mini
#define RELAY2AC_NOMINAL_A        2.0  // nominal curret 2A
#define RELAY2AC_ON_A           0.044  //  I=P/U=10W/230V=0.044A
#define RELAY2AC_PIN_RELAY         D1  // default pin relay
#define RELAY2AC_PIN_AIN           A0  // default pin analog in
#define RELAY2AC_DELAY_MS          30  // wait after relay set

#include "Arduino.h"                   // D1
#include "D1_class_Relay2.h"           // relay switching class
#include "D1_class_CurrentAC.h"        // current measuring

class Relay2AC : public Relay2 {
 //-----properties----------------------------------------------
 protected:
  CurrentAC cac_;                      // current measure object
  int    iBoard_;                      // 1=D1mini, 2=ESP32D1
 //-----constructor & co----------------------------------------
 public:
  Relay2AC();
  Relay2AC(int pinRelay);
  Relay2AC(int pinRelay, float inom_A, float ion_A);
  Relay2AC(int pinRelay, int boardType);
  Relay2AC(int pinRelay, int boardType, float inom_A, float ion_A);
  Relay2AC(int pinRelay, int boardType, float inom_A, float ion_A, int pinAin);
 protected:
  void setup(int pinRelay, int boardType, float inom_A, float ion_A, int pinAin);

 //-----setter methods-----------------------------------------
 public:
  bool   set(int val);                 // switch relay
  bool   setCurrentOn(float aOn);      // set current limit on
  bool   setNominalCurrent(float inom_A); // nominal current
 //-----getter methods-----------------------------------------
  bool   isOn();                       // true: relay is on
  bool   isOff();                      // true: relay is off
  int    getRelayStatus();             // 0=off, 1=on
  int    getCurrentStatus();           // 0=off, 1=on
  float  getCurrent();                 // AC current in A
  float  getCurrentOn();               // AC current limit "on"
  float  getNominalCurrent();          // AC nominal current 
 //-----working methods-----------------------------------------
  bool   isOK();                       // toggle 2x, check current
};
#endif


//bool setPinAin(int pinAin);

/*
  Relay2();
  Relay2(int pinRelais);
  Relay2(int pinRelais, bool invert);
  CurrentAC();                         // type 1=D1mini,2A,0.1A
  CurrentAC(float ion_A);              // default type 1=D1mini
  CurrentAC(int boardType);            // 1=D1mini, 2=ESP32D1
  CurrentAC(int boardType, float ion_A);
  CurrentAC(int boardType, float ion_A, int pin);
*/

/*
 protected:
  void setup(int pin, float imax_mA, float ion_mA);
  void measuring();                    // measures mA_, lamp_
 //-----setter, getter, working methods-------------------------
 public: 
  bool   set(int val);                 // set lamp on or off
  bool   setPinRelay(int pin);         // set (new) relais pin
  void   setParams(int pinRelais, float imax_mA, float ion_mA);
  void   setADCmax(int ADCmax);        // 1023 or 4095, ...
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
*/
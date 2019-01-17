//_____D1_class_Pulsecounter.h________________190117-190117_____
// The class Pulsecounter counts pulses in a given period.
// Default value: 60s = 1 minute
// Created by Karl Hartinger, January 17, 2019.
// Modified: -
// Released into the public domain.
#ifndef D1_CLASS_PULSECOUNTER_H
#define D1_CLASS_PULSECOUNTER_H
#include "Arduino.h"
#define  PERIOD_MS           60000     // 1 minute

extern "C" {
#include "user_interface.h"
}

//-----properties as a struct-----------------------------------
struct {
 unsigned long counter=0;              // impulse counter
 unsigned long counterLast=0;          // final value counter
 bool isNew=false;                     //new final cnt value here
 unsigned long period_ms;              // period duration
} pulsecounterData;

class Pulsecounter {
 protected:
  //-----properties---------------------------------------------
 public:
  //-----constructor & co---------------------------------------
  Pulsecounter();                      // 
  Pulsecounter(unsigned long period_ms); //
 protected:
  void setup(unsigned long period_ms); // setup properties
 public:
  //-----set/get values-----------------------------------------
  void  reset();                       //
  bool  isNew();                       //   
  unsigned long getCounter();          //
  unsigned long getCounterNow();       //
  void  setPeriod(unsigned long period_ms); //
  unsigned long getPeriod();           //
  //-----working methods----------------------------------------
  void  add();                         // increment counter
};
#endif

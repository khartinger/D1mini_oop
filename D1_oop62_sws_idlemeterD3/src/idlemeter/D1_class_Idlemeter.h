//_____D1_class_Idlemeter.h________________190118-190118_____
// The class Idlemeter measures the time between two pulses.
// The highest value within a period is stored.
// Created by Karl Hartinger, January 18, 2019.
// Modified: -
// Released into the public domain.

#ifndef D1_CLASS_IDLEMETER_H
#define D1_CLASS_IDLEMETER_H
#include "Arduino.h"
#define  PERIOD_MS           30000     // 30 Sekunden
//#define  PERIOD_MS        86400000     // 1 Tag

extern "C" {
#include "user_interface.h"
}

//-----properties as a struct-----------------------------------
struct idleData{
 unsigned long msIdle=0;              // idle duration
 unsigned long msStart=0;             // start time
};

class Idlemeter {
 protected:
  //-----properties---------------------------------------------
  idleData last_;                      //
  idleData max1_;                      //
  idleData max2_;                      //
  bool isNew_;                         //
  unsigned long starttime_ms_;         //
  unsigned long period_ms_;            //
 public:
  //-----constructor & co---------------------------------------
  Idlemeter();                         // 
  Idlemeter(unsigned long period_ms);  //
 protected:
  void setup(unsigned long period_ms); // setup properties
 public:
  //-----set/get values-----------------------------------------
  void  reset();                       //
  bool  isNew();                      //   
  idleData getLast();                  //
  idleData getMax();                   //
  idleData getMax2();                  //
  void  setPeriod(unsigned long period_ms); //
  unsigned long getPeriod();           //
  unsigned long getStarttime();        // starttime running idle
  //-----working methods----------------------------------------
  void next();                         // finish measuring
};
#endif

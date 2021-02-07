//_____D1_class_BH1750.h______________________170414-210207_____
// D1 mini class for i2c digital light sensor BH1750.
// Default i2c address is 0x23 (other option 0x5C).
// Created by Karl Hartinger, April 14, 2017
// Updates
// 2018-10-20 add measuring delay 200ms
// 2021-02-07 Many changes: add ...Address(), begin(),
//            measuringBegin(), wait4Result_ ...
// Released into the public domain.
#include "Arduino.h"              // bool, ...
#include <Wire.h>                 // i2c bus

#ifndef D1_CLASS_BH1750_H
#define D1_CLASS_BH1750_H
#define BH1750_ADDR                0x23     // 7-bit address
#define BH1750_MEASURING_DELAY_MS   200     // ms
#define BH1750_MEASURING_DELAY_MIN  130     // ms (by testing)
//-----status---------------------------------------------------
#define BH1750_OK            0    // success
#define BH1750_ERR_TOO_LONG  1    // i2c data too long to fit in transmit buffer 
#define BH1750_ERR_NACK_ADDR 2    // i2c NACK transmit address
#define BH1750_ERR_NACK_DATA 3    // i2c NACK transmit data
#define BH1750_ERR_OTHER     4    // i2c other error
#define BH1750_ERR_NUM_BYTES 5    // i2c wrong number of bytes
#define BH1750_ERR_NO_BYTE   6    // no byte received
#define BH1750_ERR_START     9    // first status after setup

class BH1750 {
 protected:
  //-----properties----------------------------------------------
 protected:
  int    i2cAddress;              // i2c address
  int    status;                  // state of measuring
  bool   bMeasuringRunning_;      // 
  bool   bNewResult;              // true=new measurement value
  uint32_t lastMeasuring_;        // time of last measuring [ms]
  uint32_t lastMeasuringStart_;   // time of last start meas[ms]
  uint32_t waitMeasuring_;        // 200ms
  uint32_t wait4Result_;          // 120ms
  long   brightness;              // measurement value in lx
 
 //------constructor & co---------------------------------------
 public:
  BH1750();                       // default constructor
  BH1750(int i2c_address);        // constructor with i2c address
 protected:
  void setup();                   // init properties
 //------setter methods-----------------------------------------
 public:
  bool   setAddress(int i2c_address);
  bool   setWaitMeasuring(int delay_ms);
 //------getter methods-----------------------------------------
  int    getAddress();            // return i2c address
  int    getWaitMeasuring();      // measuring delay in ms
  int    getStatus();             // status as int (0=OK)
  String getsStatus();            // status as text
  double getBrightness();         // get brightness as double
  long   getBi();                 // get brightness as long int
 //------working methods----------------------------------------
  bool   begin();                 // startI2C=true
  bool   begin(bool startI2C);    // checkID, reset, set regs
  bool   measuringBegin();        // start measuring
  bool   newResult();             // is new value available?
 //------rarely used directly, public methods-------------------
  bool   measuring();             // read values from sensor
};
#endif

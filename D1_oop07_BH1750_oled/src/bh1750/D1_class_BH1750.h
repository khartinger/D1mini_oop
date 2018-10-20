//_____D1_class_BH1750.h______________________170414-181020_____
// D1 mini class for i2c digital light sensor BH1750.
// Default i2c address is 0x23 (other option 0x5C).
// Created by Karl Hartinger, April 14, 2017
// Last modified 2018-10-20: Add measuring delay 200ms
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
  int  i2cAddress;
  int  status;
  long brightness;
  unsigned long nextMeasuring_;
  int  measuringDelay_;
 protected:  
  void setup();
  void measuring();
 public:
  BH1750();
  BH1750(int i2c_address);
  int    getStatus();
  double getBrightness();
  long   getBi();
  int    setMeasuringDelay(int delay_ms);
};
#endif

//_____D1_class_AM2322.h______________________170413-180519_____
// D1 mini class for temperature and humidity i2c sensor AM2322
// AM2322: Temperature -40°..+80°C, +-0,5°C/+-0,2°C, 16bit
//         Humidity    0%..99,0%, +-3%/+-0,1%, 16bit
// Default i2c address is 0x5C
// Created by Karl Hartinger, April 13, 2017
// Last modified 2018-05-19: Move class files to /src/...
// Released into the public domain.
#include "Arduino.h"              // bool, ...
#include <Wire.h>                 // i2c bus

#ifndef D1_CLASS_AM2322_H
#define D1_CLASS_AM2322_H
#define AM2322_ADDR               0x5C // 7-bit address
#define AM2322_MEASURING_DELAY_MS 500  //time between 2 measurem
#define AM2322_ERR_VALUE          -999

//-----status---------------------------------------------------
#define AM2322_OK            0    // success
#define AM2322_ERR_TOO_LONG  1    // i2c data too long to fit in transmit buffer 
#define AM2322_ERR_NACK_ADDR 2    // i2c NACK transmit address
#define AM2322_ERR_NACK_DATA 3    // i2c NACK transmit data
#define AM2322_ERR_OTHER     4    // i2c other error
#define AM2322_ERR_NUM_BYTES 5    // i2c wrong number of bytes
#define AM2322_ERR_NO_BYTE   6    // no byte received
#define AM2322_ERR_NUM_CHARS 7    // wrong number of chars
#define AM2322_ERR_CRC       8    // crc error
#define AM2322_ERR_START     9    // first status after setup

class AM2322 {
 private:
  int i2cAddress;
  int hum_;
  int tmp_;
  int status;
  unsigned long next_measuring_;
 private:  
  void setup();
  void measuring();
  unsigned short crc16(unsigned char *ptr, unsigned char len);
 public:
  AM2322();
  AM2322(int i2c_address);
  int getStatus();
  int getValues(double &temperature, double &humidity);
  int getValues10(int &temperature, int &humidity);
};
#endif

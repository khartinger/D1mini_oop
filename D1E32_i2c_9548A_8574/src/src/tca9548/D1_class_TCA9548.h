//_____D1_class_TCA9548.h________________________khartinger_____
// D1 mini class for i2c 8 channel multiplexer TCA9548.
// Default i2c address is 0x70 (other options 0x71...0x77).
// Created by Karl Hartinger, March 27, 2025
// Updates
// 2025-03-27 New
//
// Released into the public domain.
#include "Arduino.h"              // bool, ...
#include <Wire.h>                 // i2c bus

#ifndef D1_CLASS_TCA9548_H
#define D1_CLASS_TCA9548_H
#define PIN_SDA               21   // IO21=D2
#define PIN_SCL               22   // IO22=D1
#define PIN_SDA2              17   // D3=17 or 33 (default i2c_2)
#define PIN_SCL2              16   // D4=16 or 32 (default i2c_2)
#define TCA9548_ADDR          0x70 // 7-bit address
#define TCA9548_STARTVALUE    0xFF // start byte
//-----status---------------------------------------------------
#define TCA9548_OK            0   // success

class TCA9548 {
  //-----properties----------------------------------------------
 protected:
  int    i2c_num;                 // i2c number (0 or 1)
  int    pin_sda;                 // data io pin
  int    pin_scl;                 // clock io pin
  int    i2cAddress;              // i2c address
  int    status;                  // state of i/o action
 //------constructor & co---------------------------------------
 public:
  TCA9548();                      // default constructor
  TCA9548(int i2c_address);       // constructor with i2c address
  TCA9548(int i2c_address, byte startChannel); // i2c address + start channel
  TCA9548(int i2c_number, int i2c_address, byte startChannel);
  TCA9548(int i2c_number, int pin_SDA, int pin_SCL, int i2c_address, byte startChannel);
 protected:
  void   setup();                 // init properties
  bool   checkAddress(int i2c_address); // i2c address 0x20-0x27
 //------setter methods-----------------------------------------
 public:
  bool   setAddress(int i2c_address);
 //------getter methods-----------------------------------------
  int    getAddress();            // return i2c address
  int    getStatus();             // status as int (0=OK)
  String getsStatus();            // status as text
 //------working methods----------------------------------------
  bool   begin();                 // startI2C=true
  bool   begin(bool startI2C);    // checkID, reset, set regs
  bool   setChannel(int channel_);// set IO-channel
  bool   setMultiChannel(int channel_);// set more channels
  bool   reset();                 // reset device, no channel
};
#endif

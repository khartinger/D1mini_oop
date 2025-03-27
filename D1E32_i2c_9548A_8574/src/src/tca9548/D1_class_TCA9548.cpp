//_____D1_class_TCA9548.cpp______________________khartinger_____
// D1 mini class for i2c 8 channel multiplexer TCA9548.
// Default i2c address is 0x70 (other options 0x71...0x77).
// Created by Karl Hartinger, March 27, 2025
// Updates
// 2025-03-27 New
//
// Released into the public domain.
#include "D1_class_TCA9548.h"

//**************************************************************
//       constructor & co
//**************************************************************

//_______constructor (default 0x23)_____________________________
TCA9548::TCA9548() { i2cAddress=TCA9548_ADDR; setup(); reset(); }

//_______constructor with i2c address___________________________
TCA9548::TCA9548(int i2c_address) {
 if(checkAddress(i2c_address)) i2cAddress=i2c_address;
 else i2cAddress=TCA9548_ADDR;
 i2c_num=0;
 setup();
 reset();
}

//_______constructor with i2c address + start value_____________
TCA9548::TCA9548(int i2c_address, byte startChannel) {
 if(checkAddress(i2c_address)) i2cAddress=i2c_address;
 else i2cAddress=TCA9548_ADDR;
 i2c_num=0;
 setup();
 setChannel(startChannel);
}

//_______constructor with i2c number (0|1), address + start byte
TCA9548::TCA9548(int i2c_number, int i2c_address, byte startChannel) {
 if(checkAddress(i2c_address)) i2cAddress=i2c_address;
 else i2cAddress=TCA9548_ADDR;
 if(i2c_number==1) i2c_num=1; else i2c_num=0;
 setup();
 setChannel(startChannel);
}

//_______constructor with i2c number (0|1), pins, address, start byte
TCA9548::TCA9548(int i2c_number, int pin_SDA, int pin_SCL, 
                int i2c_address, byte startChannel) {
 if(checkAddress(i2c_address)) i2cAddress=i2c_address;
 else i2cAddress=TCA9548_ADDR;
 if(i2c_number==1) i2c_num=1; else i2c_num=0;
 setup();
 pin_sda = pin_SDA;
 pin_scl = pin_SCL;
 setChannel(startChannel);
}

//_______setup properties_______________________________________
void TCA9548::setup() {
 if(i2c_num==1) {
  pin_sda = PIN_SDA2;
  pin_scl = PIN_SCL2;
 } else {
  pin_sda = PIN_SDA;
  pin_scl = PIN_SCL;
 }
}

//_______is the i2c address in the range 0x38 to 0x3F ?_________
// return true: yes, false: not in the range
bool TCA9548::checkAddress(int i2c_address) {
 if((i2c_address>=0x70) && (i2c_address<=0x77)) return true;
 return false;
}

//**************************************************************
//       setter methods
//**************************************************************

//_______set i2c address________________________________________
// return true: address set, false: use default i2c address
bool TCA9548::setAddress(int i2c_address)
{
 if(checkAddress(i2c_address)) {
  i2cAddress=i2c_address;
  return true;
 }
 i2cAddress=TCA9548_ADDR;
 return false;
}

//**************************************************************
//       getter methods
//**************************************************************

//_______return i2c address_____________________________________
int TCA9548::getAddress() { return i2cAddress; }

//_______system status as integer_______________________________
// Meaning of the numbers see getsStatus()
int TCA9548::getStatus() { return status; }

//_______system status as text__________________________________
String TCA9548::getsStatus() {
  switch(status)
 {
  case TCA9548_OK:
   return String("OK");
  default:
   return String("Unknown error number ")+String(status);
 }
 return String("Serious, unknown error");
}

//**************************************************************
//       working methods
//**************************************************************

//_______check sensor, start I2C and first measuring____________
bool TCA9548::begin() { return begin(true); }

//_______check sensor, start first measuring____________________
// set startI2C=false if I2C has already been started
// return: true=OK, false on error
bool TCA9548::begin(bool startI2C) {
 //------start I2C?---------------------------------------------
 if(startI2C) {
  if(i2c_num==0) {
   Wire.begin();                   // call Wire.begin only once
  } else {
   Wire1.begin(pin_sda, pin_scl);
  }
 }
 return true;
}

//_______set communication channel 0 to 7_______________________
// return: true: channel set, false: error
bool TCA9548::setChannel(int channel_) {
 if(channel_<0 ||channel_>7) return false;
 int mask_=1<<channel_;
 if(i2c_num==0) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(mask_);
  status=Wire.endTransmission();
 } else {
  Wire1.beginTransmission(i2cAddress);
  Wire1.write(mask_);
  status=Wire1.endTransmission();
 }
 if(status!=TCA9548_OK) return false;
 return true;
}

//_______set communication channel 0x00 to 0xFF_________________
// return: true: channel set, false: error
bool TCA9548::setMultiChannel(int channel_) {
 if(channel_<1 || channel_>0xFF) return false;
 if(i2c_num==0) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(channel_);
  status=Wire.endTransmission();
 } else {
  Wire1.beginTransmission(i2cAddress);
  Wire1.write(channel_);
  status=Wire1.endTransmission();
 }
 if(status!=TCA9548_OK) return false;
 return true;
}

//_______reset device___________________________________________
// return: true: OK, false: error
bool TCA9548::reset() {
 if(i2c_num==0) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(0);
  status=Wire.endTransmission();
 } else {
  Wire1.beginTransmission(i2cAddress);
  Wire1.write(0);
  status=Wire1.endTransmission();
 }
 if(status!=TCA9548_OK) return false;
 return true;
}

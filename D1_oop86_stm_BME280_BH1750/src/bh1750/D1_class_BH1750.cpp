//_____D1_class_BH1750.cpp____________________170414-210207_____
// D1 mini class for i2c digital light sensor BH1750.
// Default i2c address is 0x23 (other option 0x5C).
// Created by Karl Hartinger, April 14, 2017
// Updates
// 2018-10-20 add measuring delay 200ms
// 2021-02-07 Many changes: add ...Address(), begin(),
//            measuringBegin(), wait4Result_ ...
// Last modified 2018-10-20: Add measuring delay 200ms
// Released into the public domain.
#include "D1_class_BH1750.h"

//**************************************************************
//       constructor & co
//**************************************************************

//_______constructor (default 0x23)_____________________________
BH1750::BH1750() { i2cAddress=BH1750_ADDR; setup(); }

//_______constructor with i2c address___________________________
BH1750::BH1750(int i2c_address) {
 if((i2c_address==0x23)||(i2c_address==0x5C)) 
  i2cAddress=i2c_address; 
 setup();
}

//_____setup properties_________________________________________
void BH1750::setup()
{
 bMeasuringRunning_=false;        // not in measurement
 bNewResult=false;                // no measurement until now
 lastMeasuring_=0;                // no last measurement time
 waitMeasuring_=BH1750_MEASURING_DELAY_MS;
 wait4Result_=120;                // data sheet
}

//**************************************************************
//       setter methods
//**************************************************************

//_______set i2c address________________________________________
bool BH1750::setAddress(int i2c_address)
{
 if((i2c_address!=0x23)&&(i2c_address!=0x5C)) return false;
 i2cAddress=i2c_address; 
 setup();
 return true;
}

//_____set mesuring delay_______________________________________
// if measuringDelay_<MIN you get allways the same value...
bool BH1750::setWaitMeasuring(int delay_ms)
{
 if(delay_ms<BH1750_MEASURING_DELAY_MIN) return false;
 waitMeasuring_=delay_ms;
 return true;
}

//**************************************************************
//       getter methods
//**************************************************************

//_______return i2c address_____________________________________
int BH1750::getAddress() { return i2cAddress; }

//_______return measuring delay in ms___________________________
int BH1750::getWaitMeasuring() { return waitMeasuring_; }

//_______system status as integer_______________________________
// Meaning of the numbers see getsStatus()
int BH1750::getStatus() { return status; }

//_______system status as text__________________________________
String BH1750::getsStatus() {
  switch(status)
 {
  case BH1750_OK:
   return String("OK");
  case BH1750_ERR_TOO_LONG:
   return String("I2C send buffer is too small for the data");
  case BH1750_ERR_NACK_ADDR:
   return String("No acknowledge (ACK) after address byte");
  case BH1750_ERR_NACK_DATA:
   return String("No acknowledge (ACK) after data byte");
  case BH1750_ERR_OTHER:
   return String("I2C Error");
  case BH1750_ERR_NUM_BYTES:
   return String("Wrong number of bytes");
  case BH1750_ERR_NO_BYTE:
   return String("No byte received");
  default:
   return String("Unknown error number ")+String(status);
 }
 return String("Serious, unknown error");
}

//_______get brightness as double_______________________________
double BH1750::getBrightness() { return(getBi()); }

//_______get brightness as long (integer)_______________________
long BH1750::getBi() { 
 bNewResult=false;
 measuring();
 return brightness; 
}

//**************************************************************
//       working methods
//**************************************************************

//_______check sensor, start I2C and first measuring____________
bool BH1750::begin() { return begin(true); }

//_______check sensor, start first measuring____________________
// set startI2C=false if I2C has already been started
// return: true=OK, false on error
bool BH1750::begin(bool startI2C) {
 //------start I2C?---------------------------------------------
 if(startI2C) {
  Wire.begin();                   // call Wire.begin only once
 }
 //------wake up sensor-----------------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x01);                // power on, waiting for cmd
 status=Wire.endTransmission();
 if(status!=BH1750_OK) return false;
 //------Reset Data register------------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x07);                // reset data register
 status=Wire.endTransmission();
 if(status!=BH1750_OK) return false;
 return true;
}

//_______start measuring________________________________________
// return: true = measurement could be started
//         false = no new measurement process could be started
bool BH1750::measuringBegin()
{
 if(bMeasuringRunning_) {
  measuring();
  return false;
 }
 bNewResult=false;               // no measurement result yet
 bMeasuringRunning_=false;       // no measurement running
 if((millis()-lastMeasuring_) < waitMeasuring_) return false;
 measuring();             // start measuring
 return true;
}

//_______is/are new measurement value(s) available?_____________
// return: true = measurement finished, no new value has been 
//         read yet
//         false = new value was read 
//         or a new measuring process was started.
bool BH1750::newResult() { return bNewResult; }

//**************************************************************
//       rarely used directly, public methods
//**************************************************************

//_____read brightness from sensor______________________________
// return: false=no new result (yet)
bool BH1750::measuring()
{
 //------check the delay time between two measurements----------
 if((millis()-lastMeasuring_) < waitMeasuring_) 
 {//.....too little time between two measurements...............
  bMeasuringRunning_=false;       // no measurement running
  return false;                   // no new result
 }
 if(!bMeasuringRunning_)
 {//.....no measurement is running yet: start measurement.......
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x10);               // Continuously H-Resol. Mode
  status=Wire.endTransmission();
  bMeasuringRunning_=true;        // measurement started
  lastMeasuringStart_=millis();
  return false;
 }
 if((millis()-lastMeasuringStart_) < wait4Result_) return false;

 //------read 2 bytes from sensor-------------------------------
 Wire.requestFrom(i2cAddress, 2); // read two byte
 if(Wire.available()!=2)          // (read) data bytes ready?
 {
  Wire.endTransmission();         // stopp transmision
  status=BH1750_ERR_NUM_BYTES;    // no bytes available
  return false;                   // no result
 }
 int temp_=Wire.read()<<8;        // 1st byte
 temp_|=Wire.read();              // 2nd byte
 brightness=temp_;                // result as long int
 bNewResult=true;                 //new measurement result there
 bMeasuringRunning_=false;        // no measurement running
 lastMeasuring_=millis();         // current time
 return true;
}

/*
 //-----set Continuously H-Resolution Mode, Resolution 1 lx-----
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x10);                // Continuously H-Resol. Mode
 status=Wire.endTransmission();
 if(status!=BH1750_OK)            // try a second time
 {
  delay(120);                     //Measurement Time(data sheet)
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x10);               // Continuously H-Resol. Mode
  status=Wire.endTransmission();
  if(status!=BH1750_OK) return;
 }
 //------read 2 bytes from sensor-------------------------------
 Wire.requestFrom(i2cAddress, 2);
 if(Wire.available()!=2)          // (read) data bytes ready?
 {
  Wire.endTransmission();
  status=BH1750_ERR_NUM_BYTES;
  return false;
 }
 int temp_=Wire.read()<<8;
 temp_|=Wire.read();
 brightness=temp_;
 bNewResult=true;
 lastMeasuring_=millis();
 return true;
}
*/
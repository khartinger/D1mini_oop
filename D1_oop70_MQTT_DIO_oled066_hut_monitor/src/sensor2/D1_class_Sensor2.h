//_____D1_class_Sensor2.h_____________________181027-200124_____
// D1 mini class to manage two each BME280 and BH1750 sensors.
// Hardware: (1) WeMos D1 mini
//           BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76 and 0x77)
//           BH1750 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x23 and 0x5C)
//
// Created by Karl Hartinger, October 27, 2018.
// Update: 2020-01-24 add getsStatus()
// Released into the public domain.

#ifndef D1_CLASS_SENSOR2_H
#define D1_CLASS_SENSOR2_H
#include "Arduino.h"                   // 
#include "../bme280/D1_class_BME280.h" //
#include "../bh1750/D1_class_BH1750.h" //
#define SENSOR2_DEG String((char)248)  // degree symbol

class Sensor2 {
 //-----properties----------------------------------------------
 protected:
  int     status;
  BME280  bme280i;                     // weather in  0x76
  //BME280  bme280o (0x77);              // does not work
  BME280  bme280o;                     // weather out 0x77
  BH1750  bh1750i;                     // brightness in  0x23
  BH1750  bh1750o;                     // brightness out 0x5C
  //BH1750  bh1750o (0x5C);              // does not work
  
 //-----constructor & co----------------------------------------
 public:
  Sensor2();
 //protected:
  void setup();                        // 
 //-----setter, getter, working methods-------------------------
 public: 
  int    getStatus();                  // sensor status
  String getsStatus();                 // sensor status string
  String getsTiOled();                 // indoor temperature 5.1
  String getsToOled();                 // outdoor temperature 5.1
  String getsJson();                   //
  String f2S(float f, int len, int decimals); // float to String
};
#endif

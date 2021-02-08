//_____D1_oop86_stm_BME280_BH1750.ino________________210208_____
// This program shows, that the sensor classes BME280 and BH1750
// are non blocking.
// Therefore a statemachine with step time 100ms is used.
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. BME280 connected to I2C (I2C-address 0x76 or 0x77)
// 3. BH1750 light sensor on i2c bus (0x23, 0x5C)
//
// Created by Karl Hartinger, February 08, 2021.
// Changes: 
// 2021-02-08 First release
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/statemachine/D1_class_Statemachine.h"
#include "./src/bme280a/D1_class_BME280a.h"
#include "src/bh1750/D1_class_BH1750.h"

//_______sensor objects_________________________________________
BME280 bme280;                         // t,p,H sensor object
BH1750 bh1750;                         // brightness

//_______state machine__________________________________________
#define STATE_MAX            36000     // 36000*100ms = 1 hour
#define STATE_DELAY            100     // state delay in ms
Statemachine stm(STATE_MAX, STATE_DELAY); //1..36000

//_____SETUP____________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
  //------search for sensor--------------------------------------
 bool status1, status2;
 bme280.setAddress(0x76);                   // 0x76 or 0x77
 status1 = bme280.begin();                  // start I2C
 if (!status1) {
  bme280.setAddress(0x77);                  // 0x76 or 0x77
  status1 = bme280.begin(false);             // NO start I2C
  if (!status1) {
   Serial.println("Could not find BME280 sensor: "+bme280.getsStatus());
  }
 }
 //------info about the sensor chip-----------------------------
 Serial.print(bme280.getSensorName()+" found! ");
 Serial.printf("I2C address is 0x%02X\n",bme280.getAddress());

 //------search for sensor--------------------------------------
 bh1750.setAddress(0x23);                   // 0x23 or 0x5C
 status2 = bh1750.begin(false);             // start I2C
 if (!status2) {
  bh1750.setAddress(0x5C);                  // 0x23 or 0x5C
  status2 = bh1750.begin(false);             // NO start I2C
  if (!status2) {
   Serial.println("Could not find BH1750 sensor: "+bh1750.getsStatus());
  }
 } 
 //------stopp if sensor error----------------------------------
 if (!(status1 & status2)) {
  Serial.println("* check wiring");
  Serial.println("* check whether Wire.begin() has been called");
  Serial.println("END!");
  while (1) delay(100);
 }
 //------info about the sensor chip-----------------------------
 Serial.printf("BH1750 found! I2C address is 0x%02X\n",bh1750.getAddress());
 Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 int state=stm.loopBegin();                 // state begin 
 String s1;
 String sSerial="";
 //------do every state-----------------------------------------
 s1=String(state);
 sSerial=("     "+s1).substring(s1.length())+": ";

//------start temperature measuring-----------------------------
 if(bme280.measuringBegin()) sSerial+="BME280 measurement started! ";
 if(bme280.newResult()) sSerial+=bme280.getsJson();

 //------start brightness measuring-----------------------------
 if(bh1750.measuringBegin()) sSerial+="BH1750 measurement started! ";
 //------Is there a result?-------------------------------------
 if(bh1750.newResult())
 {//.....yes: get the result and display it.....................
  String s1=String(bh1750.getBi());         // get brightness
  sSerial+=" Brightness: "+s1+" lx";         // display value
 }

 //======(.) do at the end of the loop ...======================
 uint32_t ms=stm.loopEnd();                 // state end
 //------show only non empty states-----------------------------
 Serial.print(sSerial+" | "); Serial.print(ms); Serial.println("ms");
}
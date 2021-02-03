//_____D1_oop85_oled66_I2c_detect.ino_________210202_210202_____
// This programme for a D1 mini (or ESP32 D1mini) scans the
// I2C bus from address 0x03 to 0x77 every 5 seconds and 
// shows the components found on the 0.66" display and 
// via the serial interface (115200Bd).
//
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. OLED Shield: 0.66", SSD1306, 64x48 pixel, I2C @ 0x3C
// 3. Any other I2C device (e.g. 0x3C = OLED Shield)
//
// Created by Karl Hartinger, February 02, 2021.
// Changes:
// 2021-02-02 First release
// Released into the public domain.
//#define D1MINI          1              // ESP8266 D1mini +pro
#define ESP32D1         2              // ESP32 D1mini
#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>                      // i2c bus
#include "src/screen1b/D1_class_Screen1b.h"

//_______sensors, actors, global variables______________________
Screen1 display_;                      // OLED 0.66"

//_______search for i2C devices (7bit addess!!)_________________
// reserved addresses: 0-2, 78-7F
// return: String with hex numbers of devices xx xx xx ...
String i2cdetect(int begin_, int end_)
{
 String sRet="";
 char c1[3];
 int  iRet;                            // i2c return value
 if((begin_<0x03)||(end_>0x77)||(begin_>end_)) return "X";
 for(int i=begin_; i<=end_; i++)       // for all addresses
 {
  Wire.beginTransmission(i);           // start transmission
  iRet = Wire.endTransmission();       // end transmission
  if(iRet==0) { //-----ACK received------------------------------
   sprintf(c1,"%02X",i);               // address to hex
   sRet+=String(c1);                   // add address
   sRet+=" ";                          // add 1 space
  }
 }
 return sRet;
}

//_____setup routine____________________________________________
void setup(void){
 //------Serial-------------------------------------------------
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 //------I2C begin, init 0.66" OLED display---------------------
 if(!display_.begin())                 // start I2C and display
  Serial.println("I2C not started or display not found!");
 else 
  Serial.println("I2C, display OK");
 //------setup finish-------------------------------------------
 Serial.println("setup(): --Finished--\n");
}

//_____main loop________________________________________________
void loop(void){
 //------start displaying data for this run---------------------
 Serial.println("Scanning i2c 7-bit-addresses 0x03 to 0x77...");
 display_.screen6iClear(1,"I2Caddress");
 //------get i2c addresses--------------------------------------
 String sAddresses = i2cdetect(0x03, 0x77);
 //------depending on the number of parts...--------------------
 int num_=sAddresses.length()/3;
 if(num_<1) {//--"no device found", will never be displayed ;)--
  Serial.println("No i2c devices connected!");
  display_.screen6(3,"No devices");
  display_.screen6(4,"found!");
 }
 else
 {//-----display i2c address(es)--------------------------------
  Serial.println(sAddresses);
  for(int i=0; i<num_; i++)
  {//----3 addresses per line-----------------------------------
   display_.screen6(3+i," "+sAddresses.substring(i*9,i*9+9));
  }
 }
 delay(5000);
}

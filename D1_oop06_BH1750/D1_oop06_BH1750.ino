//_____D1_oop06_BH1750.ino____________________170414-180519_____
// Send value of i2c light sensor BH1750 to Serial (115200Bd)
// every 0,5s
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. BH1750 light sensor on i2c bus (0x23, 0x5C)
// Created by Karl Hartinger, April 14, 2017
// Updates:
// 2018-05-19 Move class files to /src/...
// 2021-02-07 update class BH1750, many changes in setup and loop
// Released into the public domain.
#include "src/bh1750/D1_class_BH1750.h"
BH1750 bh1750;                    // brightness

//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 //------search for sensor--------------------------------------
 bh1750.setAddress(0x23);                   // 0x23 or 0x5C
 bool status = bh1750.begin();              // start I2C
 if (!status) {
  bh1750.setAddress(0x5C);                  // 0x23 or 0x5C
  status = bh1750.begin(false);             // NO start I2C
  if (!status) {
   Serial.println("Error: "+bh1750.getsStatus());
   Serial.println("Could not find BH1750 sensor:");
   Serial.println("* check wiring");
   Serial.println("* check whether Wire.begin() has been called");
   Serial.println("* check I2C address (0x23 or 0x5C)\nEND!");
   while (1) delay(100);
  }
 }
 Serial.print("BH1750 found! I2C address is 0x");
 Serial.println(bh1750.getAddress(),16);
 Serial.println("setup(): --Finished--\n");
}

unsigned long i=0;
//_______LOOP___________________________________________________
// this is a non blocking version for the loop()
void loop() {
 Serial.print(++i); Serial.print(": ");     //display run number
 //------start measuring----------------------------------------
 if(bh1750.measuringBegin()) Serial.print("measurement started! ");
 //------Is there a result?-------------------------------------
 if(bh1750.newResult())
 {//.....yes: get the result and display it.....................
  String s1=String(bh1750.getBi());         // get brightness
  Serial.print("Brightness: "+s1+" lx");    // display value
 }
 Serial.println();                          // next line
 delay(100);                                // wait 0.1s
}

/*
//.......another simple option for loop() with a long delay.....
//_______LOOP___________________________________________________
void loop() {
 int status=bh1750.getStatus();
 if(status==0)
 {
  String s1=String(bh1750.getBi());
  Serial.println("Brightness: "+s1+" lx");
 }
 else
 {
  Serial.println(bh1750.getsStatus());
  bh1750.begin(false);                     // reset
 }
 delay(500);
}
*/

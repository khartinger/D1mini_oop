//_____D1_oop100_PCF8574_1.ino___________________khartinger_____
// Send running light to PCF8574 via i2c.
// Hardware:
// (1) ESP32 D1 mini or WeMos D1 mini
// (2) PCF8574 board with (default) address 0x20 connected via i2c:
//     SCL=D1=GPIO5, SDA=D2=GPIO4, 3V3, GND
// (3) 8 LEDs (or LED bar) connected to PCF8574 board:
//     Pin0 o------+                 3V3  o------+
//                 |                             |
//           LED  \|/                          |   |
//                -+-                      330 |   |
//                 |                           |   |
//               |   |                           |
//           330 |   |                     LED  \|/
//               |   |                          -+-
//     GND  o------+                 Pin0 o------+
//     LED light up only a little    LEDs shine brightly
//     common GND                    common 3V3
//     doInvert=false                doInvert=true
// Created by Karl Hartinger, April 06, 2023
// Updates:
// 
// Released into the public domain.
#include "src/pcf8574/D1_class_PCF8574.h"

PCF8574 pcf8574(0x20, 0xAA);                // pcf8574 object

//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------init remote 8-Bit I/O Expander PCF8574-----------------
 pcf8574.setInvertOutput(true);             // if LEDs common 3V3
 while (!pcf8574.begin()) {                 // start I2C
   Serial.println("Error: "+pcf8574.getsStatus());
   Serial.print("Could not find PCF8574 at 0x");
   Serial.print(pcf8574.getAddress(), 16);
   Serial.println(": check wiring");
   delay(5000);                             // wait 5s
 }
 Serial.print("PCF8574 found at 0x");
 Serial.println(pcf8574.getAddress(), 16);
 //------send test byte to io pins (just for fun)---------------
 delay(1000);
 pcf8574.setByte((byte)(~0xAA));
 delay(1000);
 pcf8574.setByte(0);
 Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 //------running light at address 0x20 (default)----------------
 for(int i=0; i<8; i++)
 {
  //-----set bit at PCF8574 #1----------------------------------
  int j=7;                                  // last bit if i=0
  if(i>0) j=i-1;                            // last bit
  pcf8574.setBit(j, 0);                     // clear last bit
  pcf8574.setBit(i, 1);                     // set new bit
  //-----show status and wait-----------------------------------
  Serial.printf("Bit %i is active\r\n", i);
  delay(1000);                              // wait 1s
 }
}
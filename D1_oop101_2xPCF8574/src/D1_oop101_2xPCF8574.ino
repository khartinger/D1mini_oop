//_____D1_oop101_2xpcf8574_1.ino___________________khartinger_____
// Send running light to two PCF8574 via i2c.
// Demo to show the functions setBit() and setByte()
// Hardware:
// (1) ESP32 D1 mini or WeMos D1 mini
// (2) PCF8574 board #1 with (default) address 0x20
//     connected via i2c:
//     SCL=D1=GPIO5, SDA=D2=GPIO4, 3V3, GND
// (3) 8 LEDs or LED bar or relays connected to PCF8574 board #1
// (4) PCF8574 board #2 with address 0x21 connected via i2c:
//     SCL=D1=GPIO5, SDA=D2=GPIO4, 3V3, GND
// (5) 8 LEDs or LED bar or relays connected to PCF8574 board #2
//     8x relay module: +5V, GND!
// Created by Karl Hartinger, April 06, 2023
// Updates:
// 
// Released into the public domain.
#include "src/pcf8574/D1_class_pcf8574.h"

PCF8574 pcf8574_1(0x20, 0xAA);              // pcf8574 board #1
PCF8574 pcf8574_2(0x21);                    // pcf8574 board #2
//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------init remote 8-Bit I/O Expander PCF8574 #1--------------
 pcf8574_1.setInvertOutput(true);
 while (!pcf8574_1.begin()) {               // start I2C
   Serial.println("Error: "+pcf8574_1.getsStatus());
   Serial.print(" - Could not find PCF8574 at 0x");
   Serial.print(pcf8574_1.getAddress(), 16);
   Serial.println(": check wiring");
   delay(5000);                             // wait 5s
 }
 Serial.print("PCF8574 found at 0x");
 Serial.println(pcf8574_1.getAddress(), 16);
 //------init remote 8-Bit I/O Expander PCF8574 #2--------------
 if(pcf8574_2.begin(false)) {               // No 2nd start I2C!
  Serial.print("PCF8574 #2 found at 0x");
  Serial.println(pcf8574_2.getAddress(), 16);
 } else {
  Serial.print("PCF8574 #2 NOT found at 0x");
  Serial.println(pcf8574_2.getAddress(), 16);
 }
 Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 //------running lights-----------------------------------------
 for(int i=0; i<8; i++)
 {
  //-----set bit at PCF8574 #1----------------------------------
  int j=7;                                  // last bit if i=0
  if(i>0) j=i-1;                            // last bit
  pcf8574_1.setBit(j, 0);                   // clear last bit
  pcf8574_1.setBit(i, 1);                   // set new bit
  //-----set bit at PCF8574 #2----------------------------------
  int out2=~(1<<i);                         // relay active low
  pcf8574_2.setByte(out2);
  //-----show status and wait-----------------------------------
  Serial.printf("Bit %i is active\r\n", i);
  delay(1500);                              // wait 1.5s
 }
}
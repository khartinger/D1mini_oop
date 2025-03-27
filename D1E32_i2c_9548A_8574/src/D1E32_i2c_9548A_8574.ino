//_____D1E32_i2c_9548A_8574.ino_________________K.Hartinger_____
// Blinking light @ 9548A-Channel 2 and 7 with 2x PCF8574 and 
//   the same address 0x20
// Hardware: ESP32 (WeMos D1 Mini)
//           1x TCA 9548A: I2C-Multiplexer
//           2x PCF 8574 : 8-bit I/O expander for I2C-bus
// Tip: Always use pull-up resistors (10 kOhm) vor SCL and SDA!
// Created by Karl Hartinger, March 27, 2025
// Updates:
// 2025-03-27 New
// Released into the public domain.
//#include <Wire.h>                      // i2c bus
//#include <Arduino.h>
//#define D1MINI          1              // ESP8266 D1mini +pro
#define ESP32D1         2              // ESP32 D1mini
#define DEBUG_          true
#include "src/tca9548/D1_class_TCA9548.h"
#include "src/pcf8574/D1_class_PCF8574.h"

TCA9548 tca9548(0x70);                 // I2C-Multiplexer
PCF8574 pcf8574_2(0x20);               // pcf8574 board @ channel 2
PCF8574 pcf8574_7(0x20);               // pcf8574 board @ channel 7

#define PCF8574_NUM     2              //
PCF8574 pcf8574[PCF8574_NUM]={pcf8574_2, pcf8574_7};
int     pcf8574Channel[PCF8574_NUM]={2,7};

// *************************************************************
// Standard Functions
// *************************************************************

//_____SETUP____________________________________________________
void setup() {
 String sTemp="";                           // Help string
 int iRet;                                  // Return value
 //------Serial, just for debug---------------------------------
 //if(DEBUG_)
 { Serial.begin(115200); Serial.flush(); 
  Serial.println("\nsetup(): --Start--"); }
 //------init I2C-Multiplexer TCA 9548A-------------------------
 while (!tca9548.begin()) {               // start I2C
  Serial.println("Error: "+tca9548.getsStatus());
  Serial.print(" - Could not find TCA9548 at 0x");
  Serial.print(tca9548.getAddress(), 16);
  Serial.println(": check wiring");
  delay(5000);                             // wait 5s
 }
 Serial.print("TCA9548 found at 0x");
 Serial.println(tca9548.getAddress(), 16);

 //------init remote 8-Bit I/O Expander(s) PCF8574--------------
 for(int i=0; i<PCF8574_NUM; i++) {
  tca9548.setChannel(pcf8574Channel[i]);
  while (!pcf8574[i].begin(false)) {         // No 2nd start I2C!
   Serial.println("Error: "+pcf8574[i].getsStatus());
   Serial.print(" - Could not find PCF8574 at 0x");
   Serial.print(pcf8574[i].getAddress(), 16);
   Serial.println(": check wiring");
   delay(5000);                             // wait 5s
  }
  Serial.print("PCF8574 found at 0x");
  Serial.println(pcf8574[i].getAddress(), 16);
 } //----END OF init remote 8-Bit I/O Expander(s) PCF8574-------
 Serial.println("setup(): --Finished--\n");
}

long delay_=125;
int err=0;

//_______LOOP___________________________________________________
void loop() {
 int out1, out2;
 //------set 1st value(s)---------------------------------------
 out1=0x55, out2=0x70;
 tca9548.setChannel(pcf8574Channel[0]);
 Serial.print(" | channel "+String(pcf8574Channel[0])+"=0x"+String(out1,16));
 if(!pcf8574[0].setByte(out1)) { err++; Serial.print(" ERR "); }
 else Serial.print(" OK  ");
 tca9548.setChannel(pcf8574Channel[1]);
 Serial.print(" | channel "+String(pcf8574Channel[1])+"=0x"+String(out2,16));
 if(!pcf8574[1].setByte(out2)) { err++; Serial.print(" ERR "); }
 else Serial.print(" OK  ");
 delay(delay_);

 //------set 2st value(s)---------------------------------------
 out1=0xAA, out2=0x38;
 tca9548.setChannel(pcf8574Channel[0]);
 Serial.print(" | channel "+String(pcf8574Channel[0])+"=0x"+String(out1,16));
 if(!pcf8574[0].setByte(out1)) { err++; Serial.print(" ERR "); }
 else Serial.print(" OK  ");
 tca9548.setChannel(pcf8574Channel[1]);
 Serial.print(" | channel "+String(pcf8574Channel[1])+"=0x"+String(out2,16));
 if(!pcf8574[1].setByte(out2)) { err++; Serial.print(" ERR "); }
 else Serial.print(" OK  ");
 Serial.println("Error: "+String(err));
 delay(delay_);
}

//_____D1_oop21_ain2oled_asm010.ino___________171008-181013_____
// Show analog input value every second on oled and Serial,
// convert it to current value
// Use classes Screen1 and Ain
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
//           (3) Current transformer ASM-010 and INA122 amplifier
// Created by Karl Hartinger, October 13, 2018
// Modified: -
// Released into the public domain.
#include "src/ain/D1_class_ain.h"
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0              // OLED_RESET=GPIO 0
Ain ain;                                    // object
Screen1 display_;                 // display object


//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(115200); Serial.println("");
 //-----init screen---------------------------------------------
 display_.screen112Clear(1,"Analog In", 'c');// line 1 (inverted)
 display_.screen112(2,"?",'c');             // line 2 (big)
 display_.screen112(3,"?",'c');             // line 3 (ampere)
 display_.screen112(4,"V181013 KH");        // line 4 (version)
 //-----init analog in------------------------------------------
 //ain.setRefPoints(0, 0.0, 1000, 2.967);     // volts
 ain.setRefPoints(0, 0.0, 1000, 500);         // milliampere
}

//_____main loop________________________________________________
void loop() 
{
 //-----do 16 measurements in 1 second, calc average------------
 int ain1023=0;
 for(int i=0; i<16; i++)
 {
 	ain1023+=ain.getAin1023();                // 0..1023
 	delay(62);
 }
 ain1023=ain1023>>4;                        // calc average
 //-----convert Ain (0...1023) to ampere (0...511,5A) and String
 float  current=ain.getVoltage(ain1023);
 String sCurrent=String(current, 0);
 String sAin1023=String(ain1023);
 String s1=sAin1023+" = "+sCurrent+" V";
 //-----print out the value to serial---------------------------
 Serial.println(s1);
 //-----display oled--------------------------------------------
 display_.screen112(2,sAin1023,'c');        // line 2 = value
 display_.screen112(3,sCurrent+" mA",'c');   // line 3 = volt
}

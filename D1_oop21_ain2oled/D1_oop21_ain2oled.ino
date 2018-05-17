//_____D1_oop21_ain2oled.ino__________________171008-180517_____
// Show analog input value every second on oled and Serial.
// Use class Screen1, but NOT Ain (to show coding of ain)
// Hardware: WeMos D1 mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, October 10, 2017
// Modified 2018-05-17: Use class Screen1 (NOT Ain)
//                      Move class files to /src/...
// Released into the public domain.
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0              // OLED_RESET=GPIO 0
Screen1 display_;                 // display object
const int pinAin=A0;              // analog in pin
const float ain_x2=1012, ain_y2=3.277, ain_x1=0, ain_y1=0;
const float ain_k=(ain_y2-ain_y1)/(ain_x2-ain_x1);
const float ain_d=(ain_x2*ain_y1-ain_x1*ain_y2)/(ain_x2-ain_x1);

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(115200); Serial.println("");
 //-----init screen---------------------------------------------
 display_.screen112Clear(1,"Analog In", 'c');// line 1 (inverted)
 display_.screen112(2,"?",'c');             // line 2 (big)
 display_.screen112(3,"?",'c');             // line 3 (volt)
 display_.screen112(4,"V180517 KH");        // line 4 (version)
}

//_____main loop________________________________________________
void loop() 
{
 int ain1023 = analogRead(pinAin); // analog in value (0..1023)
 //-----Convert Ain (0...1023) to voltage (0...3.2V) and String-
 float voltage=ain_k*ain1023 + ain_d;
 String sAin1023=String(ain1023);
 String sVoltage=String(voltage);
 String s1=sAin1023+" = "+sVoltage+" V";
 //-----print out the value to serial---------------------------
 Serial.println(s1);
 //-----display oled--------------------------------------------
 display_.screen112(2,sAin1023,'c');        // line 2 = value
 display_.screen112(3,sVoltage+" V",'c');   // line 3 = volt
 delay(1000);  
}

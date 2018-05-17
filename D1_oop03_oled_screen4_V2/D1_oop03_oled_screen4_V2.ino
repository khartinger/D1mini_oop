//_____D1_oop03_oled_screen4_V2.ino___________170411-171231_____
// Display text on OLED shield using an easy to use 
// display class Screen1 for displaying text
//   "temperature" 20..40 (increased by 0,1° every second)
//   "humidity"    double value of temperature
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, April 11, 2017
// Last modified 2018-05-17: Move class files to /src/...
// Released into the public domain.
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0                        //OLED_RESET=D3=GPIO0
Screen1 display_;                           // display object

float f=20;                                 //dummy value 20..40
char carray[10];                            // convert helper
String s1;                                  // help string
String sDeg=String((char)248);              // degree symbol

void setup() {
 display_.screen4(1,"Temperature");         // line 1 (inverted)
 display_.screen4(3,"humidity",'c');        // line 3
}

void loop() {
 dtostrf(f,6,1,carray);                     // format float 6.1
 s1=String(carray);                         // array to string
 s1.trim();                                 // remove blanks
 display_.screen4(2, s1+sDeg+"C", 'c');     // print temperature
 dtostrf(2*f,6,1,carray);                   // format float 6.1
 s1=String(carray);                         // array to string
 s1.trim();                                 // remove blanks
 display_.screen4(4, s1+"%", 'c');          // print humidity
 delay(1000);                               // wait 1 sec
 f=f+0.1; if(f>40) f=20;                    // inc value
}


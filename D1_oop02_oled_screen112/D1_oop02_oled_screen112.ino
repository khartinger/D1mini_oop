//_____D1_oop02_oled_screen112.ino____________170411-180515_____
// Display text on OLED shield using the easy to use display 
// class Screen1 for displaying text
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: SSD1306, 64x48 pixel, I2C
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Function: Show the "temperature" 20,5° on OLED
// Created by Karl Hartinger, April 04, 2017,
// Last modified 2018-05-15: Move class files to /src/...
// Released into the public domain.
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
Screen1 display_;

void setup() {
 String deg_=String((char)248);             // degree sign
 display_.screen112(1,"Temperatur");        // line 1 (inverted)
 display_.screen112(2,"20,5"+deg_);         // line 2
 display_.screen112(3,"Demo for",'L');      // line 3
 display_.screen112(4,"screen112");         // line 4
}

void loop() {
}

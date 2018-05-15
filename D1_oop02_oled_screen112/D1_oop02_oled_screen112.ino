//_____D1_oop02_oled_screen112.ino____________170411-180515_____
// Display text on OLED shield using the easy to use display 
//class Screen1 for displaying text
// Hardware: WeMos D1 Mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
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

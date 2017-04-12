//_____D1_oop02_oled_screen112.ino____________170411-170412_____
// Display text on OLED shield using an easy to use extended
//   display class for displaying text (11 different "screens")
// Hardware: WeMos D1 Mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
#include "D1_class_DisplayKH.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
DisplayKH display_;

void setup() {
 String deg_=String((char)247);
 display_.screen112(1,"Temperatur");        // line 1 (inverted)
 display_.screen112(2,"20,5"+deg_);         // line 2
 display_.screen112(3,"Demo for",'L');      // line 3
 display_.screen112(4,"screen112");         // line 4
}

void loop() {
}


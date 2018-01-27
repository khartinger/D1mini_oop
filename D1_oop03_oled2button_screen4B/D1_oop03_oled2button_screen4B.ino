//_____D1_oop03_oled2button_screen4B.ino______180127-180127_____
// Display text on OLED shield with 2 integrated buttons
//  using screen4B from class Screen1
// Hardware: WeMos D1 Mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, button D3, D4
#include "D1_class_Screen1.h"
#define OLED_RESET 0             // OLED_RESET=D0
Screen1 display_;
const int pinA=D3;               // button A: D3
const int pinB=D4;               // button B: D4

void setup() {
 pinMode(pinA, INPUT);
 pinMode(pinB, INPUT);
 display_.screen4BiClear(1,"Buttontest");     // line 1 (inverted)
 display_.screen4B(4,"Press...",'c');         // line 4
 display_.screen4B(5,"A");
 display_.screen4B(6,"B");
}

void loop() {
 if(digitalRead(pinA)==0) display_.screen4B(2,"A pressed");
                     else display_.screen4B(2,"");
 if(digitalRead(pinB)==0) display_.screen4B(3,"B pressed"); 
                     else display_.screen4B(3,"");
}


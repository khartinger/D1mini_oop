//_____D1_oop03_oled2button_screen4B.ino______180127-180127_____
// Display text on OLED shield with 2 integrated buttons
//  using screen4B from class Screen1
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield V2.0.0: SSD1306, 64x48px, I2C(0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, January 27, 2018
// Last modified 2018-05-17: Move class files to /src/...
// Released into the public domain.
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0             // OLED_RESET=GPIO 0
Screen1 display_;
const int buttonA=D3;            // button A: D3
const int buttonB=D4;            // button B: D4

void setup() {
 pinMode(buttonA, INPUT);
 pinMode(buttonB, INPUT);
 display_.screen4BiClear(1,"Buttontest");     // line 1 (inverted)
 display_.screen4B(4,"Press...",'c');         // line 4
 display_.screen4B(5,"A");
 display_.screen4B(6,"B");
}

void loop() {
 if(digitalRead(buttonA)==0) display_.screen4B(2,"A pressed");
                        else display_.screen4B(2,"");
 if(digitalRead(buttonB)==0) display_.screen4B(3,"B pressed"); 
                        else display_.screen4B(3,"");
}


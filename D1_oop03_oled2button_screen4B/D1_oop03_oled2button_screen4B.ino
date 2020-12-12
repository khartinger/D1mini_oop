//_____D1_oop03_oled2button_screen4B.ino______180127-180127_____
// Display text on a 0.66" OLED shield with 2 integrated buttons
// using the screen4B of class Screen1.
// Note: The display does NOT contain pull-up resistors for the
//       buttons. These resistors must (especially when using an
//       ESP32Dimini!) be provided externally (2x10kOhm to 3V3).
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware:
// (1) WeMos D1 Mini or ESP32D1mini
// (2) OLED Shield: 0.66" SSD1306, 64x48 pixel, I2C (0x3C)
//     SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, January 27, 2018
// Updates:
// 2018-05-17 Move class files to /src/...
// 2020-12-12 Screen1a instead of Screen1, add display_.begin();
//            Now suitable for D1 mini or ESP32Dimini 
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/screen1a/D1_class_Screen1a.h"

Screen1 display_;
#if defined(ESP32D1)
 const int buttonA=17;                     // button A: D3=17
 const int buttonB=16;                     // button B: D4=16
#else
 const int buttonA=D3;                      // button A: D3
 const int buttonB=D4;                      // button B: D4
#endif

void setup() {
 pinMode(buttonA, INPUT);
 pinMode(buttonB, INPUT);
 display_.begin();                          // MUST be called
 display_.screen4BiClear(1,"Buttontest");   // line 1 (inverted)
 display_.screen4B(4,"Press...",'c');       // line 4
 display_.screen4B(5,"A");
 display_.screen4B(6,"B");
}

void loop() {
 String s1="";
 if(digitalRead(buttonA)==0) display_.screen4B(2,"A pressed");
                        else display_.screen4B(2,"");
 if(digitalRead(buttonB)==0) display_.screen4B(3,"B pressed"); 
                        else display_.screen4B(3,"");
 delay(20);
}

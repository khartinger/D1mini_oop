//_____D1_oop02_oled_screen6_specials.ino_____180515-180515_____
// Display text with special characters on OLED shield using 
// the easy to use display class Screen1 for displaying text.
// Screen1: Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Special characters in a String must be translated from UTF8
// to OLED by calling the method s1=display_.utf8ToOled(s1);
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: SSD1306, 64x48 pixel, I2C
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Function: Show this special chars on OLED (HTML notation)
// &auml;&ouml;&uuml; &Auml;Ouml;&Uuml; &szlig;
// ...
// Created by Karl Hartinger, April 04, 2017,
// Last modified 2018-05-15: Move class files to /src/...
// Released into the public domain.
#include "src/screen1/D1_class_Screen1.h"
#define S(x) String((char)(x))
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
Screen1 display_;

String _pi=String((char)227);
String _root=String((char)251);

void setup() {
 String s1="Specials";
 display_.screen6iClear(1,s1);    // line 1 (inverted)
 s1="äöü ÄÖÜ ß";                  // ae oe ue Ae Oe Ue &szlig;
 s1=display_.utf8ToOled(s1);
 display_.screen6(2,s1);          // line 2
 s1=S(241)+"5° 2µA 1€";           // +-5grad 2microA 1Euro
 s1=display_.utf8ToOled(s1);
 display_.screen6(3,s1);          // line 3
 s1="V="+_pi+"r²h"+S(250)+_root+"3"; // point
 s1=display_.utf8ToOled(s1);
 display_.screen6(4,s1);          // line 4
 s1="C2"+S(0xC2)+"C3"+S(0xC3)+"E2"+S(0xE2)+S(0x82);
 display_.screen6(5,s1);          // line 5
 s1="Mag."+S(166)+" X";
 display_.screen6(6,s1);          // line 6
}

void loop() {
}

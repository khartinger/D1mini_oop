#include <Arduino.h>
#line 1 "e:\\!Skript\\!WeMos\\sketchbook7\\D1_oop66_oled096_screen16_1\\D1_oop66_oled096_screen16_1.ino"
//_____D1_oop66_oled096_screen16_1.ino________191217-191217_____
// * Show text on an oled 0,96" (128x64 pipel):
//   1 line "title": max. 16 (or 18) characters, 16 px high
//   6 lines "text": max. 21 characters, 8 px high
// * If you want to use codepage 437, add the following line
//   display_.useCP437(); // after line display_.begin();
// * Special using CP437 0x80 = Euro instead of C,
// Hardware: (1) WeMos D1 Mini
//           (2) one color OLED 0.96" mounted on a proto shield 
//               (128x64 pixel, I2C-address 0x3C)       
//               
// See also: https://blog.hau.me/2018/12/26/i2c-oled-display-on-a-wemos-d1-mini/
//           http://oleddisplay.squix.ch/#/home (Font Converter)
// Created by Karl Hartinger, December 17, 2019
// Update: -
// Released into the public domain.

#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;

String sAlpha=display_.CP437ToString(0xE0);
String title="*** Screen16 ***";
String text="Long txt+special charAlpha="+sAlpha+"Euro=€|ÄÖÜäöüß|deg=°| !\"#$%&'()*+,-.//0123456789:;<=>?@A{[\\]}~^_abcdefghijklmnopqrstuvwxyz";
String line2="Line 2, left";
String line3="Line 3, center";
String line4="Line 4, right";
String line5="Line 5, written by KH";
String line6="{[(|Line 6|)]}qkmwmw";
String line7="{[(|Line 7|)]}qkmwmw";

//_____SETUP____________________________________________________
#line 32 "e:\\!Skript\\!WeMos\\sketchbook7\\D1_oop66_oled096_screen16_1\\D1_oop66_oled096_screen16_1.ino"
void setup();
#line 39 "e:\\!Skript\\!WeMos\\sketchbook7\\D1_oop66_oled096_screen16_1\\D1_oop66_oled096_screen16_1.ino"
void loop();
#line 32 "e:\\!Skript\\!WeMos\\sketchbook7\\D1_oop66_oled096_screen16_1\\D1_oop66_oled096_screen16_1.ino"
void setup()   {
 Serial.begin(115200); Serial.println();
 display_.begin();
 display_.useCP437(); // after line display_.begin();
}

//_____MAIN LOOP________________________________________________
void loop() {
 display_.screen16Clear(1,title,'c');  // centered title
 display_.screen16(2,text);            // write long text
 display_.drawRect(0,0,128,16);        // draw a rectangle...
 display_.display();                   // ...and show it
 delay(6000);                          // wait 6 secs
 display_.screen16Clear(1,title,'c');  // centered title
 display_.screen16(2,line2);           // 1x line, assign left
 display_.screen16(3,line3,'c');       // 1x line, centered
 display_.screen16(4,line4,'r');       // 1x line, assign left
 display_.screen16(5,line5);           // 1x line, assign left
 display_.screen16(6,line6,'c');       // 1x line, centered
 display_.screen16(7,line7);           // 1x line, assign left
 delay(6000);                          // wait 6 secs
}


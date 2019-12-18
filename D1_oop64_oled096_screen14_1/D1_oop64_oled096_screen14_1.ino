//_____D1_oop64_oled096_screen14_1.ino________191017-191024_____
//* Turn on all pixel (for test), then use screen14 to
//* display a long text on an oled 0,96" (128x64 pixel)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield (128x64 pixel, I2C-addr 0x3C)       
// See also: https://blog.hau.me/2018/12/26/i2c-oled-display-on-a-wemos-d1-mini/
//           http://oleddisplay.squix.ch/#/home (Font Converter)
// Created by Karl Hartinger, October 17, 2019
// Update 2019-10-24: turn on all pixel on start
// Released into the public domain.

#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;
const String s1="Screen14";
const String s2="A very long text...äöü|pqgjyµ|-abcdefghijkl|ÄÖÜÄÖÜ|mnopqrstuvwxyz1234567890ABCDEFGHIJK";

//_____SETUP____________________________________________________
void setup() {
 display_.begin();                          // init display
 display_.fillRect(0,0,128,64);             // all pixel on
 display_.display();                        // show buffer
 delay(2000);                               // look at screen
 display_.setColor(BLACK);                  // 
 display_.drawRect(1,1,126,62);             // black line
 display_.fillRect(3,3,122,58);             // black rectangle
 display_.setColor(WHITE);                  // 
 display_.display();                        // show buffer
 delay(3000);                               //
 display_.screen14Clear(1, s1, 'c');        // write title
 display_.screen14(2,s2);                   // write long text
}

//_____LOOP_____________________________________________________
void loop() {
}

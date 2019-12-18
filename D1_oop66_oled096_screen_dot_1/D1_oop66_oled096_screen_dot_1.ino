//_____D1_oop66_oled096_screen_dot_1.ino______191217-191217_____
// * Plot single dots on an oled 0,96" (128x64 pipel)
// (1) Write Title "Waiting..." on oled
// (2) Plot 50 dots on screen
// (3) Write Title "Half time!" on oled
// (4) Plot 50 dots on screen
// (5) Write Title "Ready" on oled and wait 2 secs
// Hardware: (1) WeMos D1 Mini
//           (2) one color OLED 0.96" mounted on a proto shield 
//               (128x64 pixel, I2C-address 0x3C)       
//               
// Created by Karl Hartinger, December 17, 2019
// Update: -
// Released into the public domain.

#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;

//_____SETUP____________________________________________________
void setup()   {
 Serial.begin(115200); Serial.println();
 display_.begin();
 display_.useCP437(); // after line display_.begin();
}

//_____MAIN LOOP________________________________________________
void loop() {
 //-----screen for waiting purposes (e.g. network connection)---
 display_.dotClear("Waiting...");      // clear dot screen
 for(int i=1; i<=100; i++)             // 100 dots
 {
  if(i==50) display_.dotTitle("Half time!"); // plot title + dot
  else display_.dot();                 // plot only a dot
  delay(100);                          // wait
 }       
 display_.dotTitleOnly("Ready");       // plot only the title
 delay(2000);                          // and wait 2 sec
}

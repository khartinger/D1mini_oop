//_____D1_oop66_oled096_screen16Dot_1.ino_____191221-191221_____
// * Show text and dots on an oled 0,96" (128x64 pixel):
//   line 2 and line 3 with text: max. 21 characters, 8 px high
//   16..18 dots in line 1 with speed 0,25s/dot
//   [max. 16 (or 18) characters, 16 px high]
//   dotes in line 4...7 with different speeds
// Hardware: (1) WeMos D1 Mini
//           (2) one color OLED 0.96" mounted on a proto shield 
//               (128x64 pixel, I2C-address 0x3C)       
//               
// See also: https://blog.hau.me/2018/12/26/i2c-oled-display-on-a-wemos-d1-mini/
//           http://oleddisplay.squix.ch/#/home (Font Converter)
// Created by Karl Hartinger, December 21, 2019
// Update: -
// Released into the public domain.

#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;

String line2="** Screen16Dot **";
String line3="Written by KH 191221";

//_____SETUP____________________________________________________
void setup()   {
 Serial.begin(115200); Serial.println();
 display_.begin();
 display_.useCP437(); // after line display_.begin();
}

//_____MAIN LOOP________________________________________________
void loop() {
 display_.screen16Clear(2,line2,'c');  // 1x line, centered
 display_.screen16(3,line3,'c');       // 1x line, centered
 display_.screen16Dot(1,4);             // dot line 1
 display_.screen16Dot(4,4);             // dot line 4
 display_.screen16Dot(5,4);             // dot line 5
 display_.screen16Dot(6,4);             // dot line 6
 display_.screen16Dot(7,4);             // dot line 7
 delay(2000);
 for(int i=0; i<240; i++)
 {
  display_.screen16Dot(1);             // dot line 1
  if(i%2==0) display_.screen16Dot(4);  // dot line 4
  if(i%4==0) display_.screen16Dot(5);  // dot line 5
  if(i%2==0) display_.screen16Dot(6);  // dot line 6
  if(i%4==0) display_.screen16Dot(7);  // dot line 7
  delay(500);                          // wait 0,5 sec
 }
}

/*
//_____MAIN LOOP________________________________________________
void loop() {
 display_.screen15Clear(2,line2,'c');  // 1x line, centered
 display_.screen15(3,line3,'c');       // 1x line, centered
 display_.screen15Dot(1,4);             // dot line 1
 display_.screen15Dot(4,4);             // dot line 4
 display_.screen15Dot(5,4);             // dot line 5
 display_.screen15Dot(6,4);             // dot line 6
 delay(2000);
 for(int i=0; i<240; i++)
 {
  display_.screen15Dot(1);             // dot line 1
  if(i%2==0) display_.screen15Dot(4);  // dot line 4
  if(i%4==0) display_.screen15Dot(5);  // dot line 5
  if(i%2==0) display_.screen15Dot(6);  // dot line 6
  delay(500);                          // wait 0,5 sec
 }
}
*/
/*
//_____MAIN LOOP________________________________________________
void loop() {
 display_.screen14Clear(2,line2,'c');  // 1x line, centered
 display_.screen14(3,line3,'c');       // 1x line, centered
 display_.screen14Dot(1,4);             // dot line 1
 display_.screen14Dot(4,4);             // dot line 4
 display_.screen14Dot(5,4);             // dot line 5
 delay(2000);
 for(int i=0; i<240; i++)
 {
  display_.screen14Dot(1);             // dot line 1
  if(i%2==0) display_.screen14Dot(4);  // dot line 4
  if(i%4==0) display_.screen14Dot(5);  // dot line 5
  delay(500);                          // wait 0,5 sec
 }
}
*/
/*
//_____MAIN LOOP________________________________________________
void loop() {
 display_.screen8Clear(2,line2,'c');  // 1x line, centered
 display_.screen8(3,line3,'c');       // 1x line, centered
 display_.screen8Dot(1,4);            // dot line 1
 display_.screen8Dot(4,4);            // dot line 4
 display_.screen8Dot(5,4);            // dot line 5
 display_.screen8Dot(6,4);            // dot line 6
 display_.screen8Dot(7,4);            // dot line 7
 display_.screen8Dot(8,4);            // dot line 8
 delay(2000);
 for(int i=0; i<240; i++)
 {
  display_.screen8Dot(1);              // dot line 1
  if(i%2==0) display_.screen8Dot(4);   // dot line 4
  if(i%4==0) display_.screen8Dot(5);   // dot line 5
  if(i%2==0) display_.screen8Dot(6);   // dot line 6
  if(i%4==0) display_.screen8Dot(7);   // dot line 7
  if(i%2==0) display_.screen8Dot(8);   // dot line 8
  delay(500);                          // wait 0,5 sec
 }
}
*/
/*
//_____MAIN LOOP________________________________________________
void loop() {
 display_.screen7Clear(2,line2,'c');  // 1x line, centered
 display_.screen7(3,line3,'c');       // 1x line, centered
 display_.screen7Dot(1,4);            // dot line 1
 display_.screen7Dot(4,4);            // dot line 4
 display_.screen7Dot(5,4);            // dot line 5
 display_.screen7Dot(6,4);            // dot line 6
 display_.screen7Dot(7,4);            // dot line 7
 delay(2000);
 for(int i=0; i<240; i++)
 {
  display_.screen7Dot(1);              // dot line 1
  if(i%2==0) display_.screen7Dot(4);   // dot line 4
  if(i%4==0) display_.screen7Dot(5);   // dot line 5
  if(i%2==0) display_.screen7Dot(6);   // dot line 6
  if(i%4==0) display_.screen7Dot(7);   // dot line 7
  delay(500);                          // wait 0,5 sec
 }
}
*/
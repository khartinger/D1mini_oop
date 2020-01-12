//_____D1_oop65_oled096_cp437_allchar16.ino___191203-191216_____
// * Show all characters 32..255 of the built in codepage CP437
//   fontKH_cp437_16x7 (16 pixel high) on an oled 0,96" 
//   (128x64 pixel).
// * Copy 16 (CP437-) characters into an array, convert chars 
//   to an Utf8-array and this to a (Utf8-)string s.
// * Write 4 pages with 4 lines using method 
//   drawStringUsingCP437(x,y,s);
// * Special: cp850 0x80 = Euro instead of C,
// Hardware: (1) WeMos D1 Mini
//           (2) one color OLED 0.96" mounted on a proto shield 
//               (128x64 pixel, I2C-address 0x3C)       
//               
// See also: https://blog.hau.me/2018/12/26/i2c-oled-display-on-a-wemos-d1-mini/
//           http://oleddisplay.squix.ch/#/home (Font Converter)
// Created by Karl Hartinger, December 03, 2019
// Update: -
// Released into the public domain.

#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;

//_____SETUP____________________________________________________
void setup()   {
 Serial.begin(115200); Serial.println("\nStart");
 display_.begin();
 display_.setFont((uint8_t *) fontKH_cp437_16x7); // char 14x7
 //display_.setFont((uint8_t *) Monospaced_plain_13x6);
 //display_.setFont((uint8_t *) fontKH_cp437_8x6); //char: 5x7
}

//_____MAIN LOOP________________________________________________
void loop() {
 int    row_;                               // row on oled
 int    i,j;                                // index for ca
 unsigned char acp437[16+1]={0};            // help array
 unsigned char autf8[4*16+1]={0};           // help array
 unsigned char c1=0;                        // start char
 String s1;                                 // utf8 string
 String sHex;                               // num of first char
 int    dx=display_.getFontWidth();         // width of 1 char   
 int    dy=display_.getFontHeight();        // height of 1 line
 int    x1=2*dx+2;                          // text upper left
 int    y1;                                 // text upper left
 int    lines=64/dy;                        // lines per screen
 //-------------------------------------------------------------
 for(int iscreen=0; iscreen<(16/lines); iscreen++)
 {
  display_.clear();                         // clear diaplay
  c1=0+iscreen*16*lines;                    // start char
  y1=0;                                     // text line (upper)
  for(row_=0; row_<lines; row_++)           // build 8 lines
  {
   sHex=String(c1,16);                      // # of line start char
   for(i=0; i<16; i++) acp437[i]=c1++;      // fill array
   acp437[i]=0;                             // end of array
   j=display_.aCP437ToUtf8(16, acp437, autf8); // utf8 array
   autf8[j]=0;                              // end of array
   s1=String((char*) autf8);                // array to string
   //.....prepare display line 1 to 4...........................
   display_.fillRect(0,y1-1,1+2*dx,dy);     // white box
   display_.setColor(BLACK);                // draw in black
   display_.drawStringUsingCP437(1,y1,sHex);// black text
   display_.setColor(WHITE);                // draw white again
   display_.drawStringUsingCP437(x1,y1,s1); // draw string
   y1+=dy;                                  // position next row
  }
  display_.display();                       // show buffer
  delay(6000);                              // wait
 }
}

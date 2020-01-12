//_____D1_oop65_oled096_cp437_allchar8.ino____191111-200107_____
// * Show all characters 32..255 of the built in codepage CP437
//   font fontKH_cp437_8x6 (8 pixel high) on an oled 0,96" 
//   (128x64 pixel).
// * Copy 16 (CP437-) characters into an array, convert chars 
//   to an Utf8-array and this to a (Utf8-)string s.
// * Write 2 pages with 7 lines using method 
//   drawStringUsingCP437(x,y,s);
// * Special: CP437 0x80 = Euro instead of C,
// Hardware: (1) WeMos D1 Mini
//           (2) one color OLED 0.96" mounted on a proto shield 
//               (128x64 pixel, I2C-addr 0x3C)       
//               
// See also: https://blog.hau.me/2018/12/26/i2c-oled-display-on-a-wemos-d1-mini/
//           http://oleddisplay.squix.ch/#/home (Font Converter)
// Created by Karl Hartinger, November 11, 2019
// Update: -
// Released into the public domain.

#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;

void setup()   {
 Serial.begin(115200); Serial.println("\nStart");
 display_.begin();
 display_.setFont((uint8_t *) fontKH_cp437_8x6); //char: 5x7
}

//_____MAIN LOOP________________________________________________
void loop() {
 int    row_;                               // row on oled
 int    i,j;                                // index for ca
 unsigned char acp437[16+1]={0};            // help array
 unsigned char autf8[4*16+1]={0};           // help array
 unsigned char c1=32;                       // start char
 String s1;                                 // utf8 string
 String sHex;                               // num of first char
 int    dx=display_.getFontWidth();         // width of 1 char   
 int    dy=1+display_.getFontHeight();      // height of 1 line
 int    x1=3*dx,y1;                         // text upper left
 //-------------------------------------------------------------
 // screen 0: chars 0x20= 32 to 0x8F=143
 // screen 1: chars 0x90=144 to 0xFF=255
 for(int iscreen=0; iscreen<2; iscreen++)
 {
  display_.clear();                         // clear diaplay
  c1=32+iscreen*16*7;                       // start char
  y1=0;                                     // text line (upper)
  for(row_=0; row_<7; row_++)               // build 8 lines
  {
   sHex=String(c1,16);                      // # of line start char
   for(i=0; i<16; i++) acp437[i]=c1++;      // fill array
   acp437[i]=0;                             // end of array
   j=display_.aCP437ToUtf8(16, acp437, autf8); // utf8 array
   autf8[j]=0;                              // end of array
   s1=String((char*) autf8);                // array to string
   //.....display line 1 to 6....................................
   display_.fillRect(0,y1-1,2+2*dx,dy);     // white box
   display_.setColor(BLACK);                // draw in black
   display_.drawString(1,y1,sHex);          // black text
   display_.setColor(WHITE);                // draw white again
   display_.drawStringUsingCP437(x1,y1,s1); // draw string
   y1+=dy;                                  // position next row
  }
  display_.display();                       // show buffer
  delay(5000);                              // wait
 }
}

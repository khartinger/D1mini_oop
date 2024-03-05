//_____D1_oop108_oled154_cp437_V1________________khartinger_____
// This program for a D1mini (or ESP32) shows
// all ascii characters from code page 437 on a 1.54" OLED shield 
// using class Screen154
// See also:
//   https://github.com/olikraus/u8g2

// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: 1.54" SSD1309, 128x64 pixel, I2C
// Created by Karl Hartinger, March 05, 2024
// Changes:
// 2024-03-05 New
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini

#include "src/screen154/D1_class_Screen154.h"

Screen154 x;

//_____SETUP____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 x.begin();
 //------prepare output-----------------------------------------
 x.setFontText(u8g2_font_KH_cp437_6x8_mf);  //
 x.useFontText();
 x.setFontRefHeightText();                  // (default)
 x.setFontPosTop();
 Serial.println("-----font properties-------------");
 Serial.println("setFontRefHeightText()");
 Serial.println("setFontPosTop()");
 Serial.println("getMaxCharHeight(): "+String(x.getMaxCharHeight()));
 Serial.println("getMaxCharWidth() : "+String(x.getMaxCharWidth()));
 Serial.println("getFontHeight()   : "+String(x.getFontHeight()));
 Serial.println("getFontAscent()   : "+String(x.getFontAscent()));
 Serial.println("getFontDescent()  : "+String(x.getFontDescent()));
}

//_____LOOP_____________________________________________________
void loop() {

 //======code page 437: all chars 32...255======================
 int w0=x.getMaxCharWidth();
 int h0=x.getMaxCharHeight();
 int x0=4*w0;                               // x drawing start
 int y0=2*h0;                               // y drawing start
 int x_, y_;                                // cursor position
 uchar c0=32;                               // start character
 //------for all screens (0, 1, 2)------------------------------
 for(int iScr=0; iScr<3; iScr++)
 {
  x.clearBuffer();
  //.....draw a frame with marks................................
  x.drawFrame(0,0,128,64);
  for(int ix=1; ix<21; ix++)
  { x.drawPixel(ix*w0, 1); x.drawPixel(ix*w0, 62); };
  for(int iy=1; iy<21; iy++)
  { x.drawPixel(1, iy*h0); x.drawPixel(126,iy*h0); };
 
  //.....draw screen the labelling..............................
  x.drawBox(7,6,113,9);
  x.drawBox(7,16,3*w0-3,5*h0+1);
  int cStart=c0+iScr*80;
  int cEnd=cStart+79;
  if(cEnd>255) cEnd=255;
  String s_= String(cStart);
  s_+=" - "+String((cEnd));
  x.setDrawColor(BLACK);
  x.drawString(42,6,s_);
  x.setDrawColor(WHITE);
  //-----for all rows in one screen-----------------------------
  for(int iRow=0; iRow<5; iRow++)
  {
   x_=x0;                                   // x draw line start
   y_=y0+h0*iRow;                           // y draw line start
   //----draw start value of this row---------------------------
   if(cStart+iRow*16<256) {
    s_=String(cStart+iRow*16, 16);
    x.setDrawColor(BLACK);
    x.drawString(8,y_,s_);
    x.setDrawColor(WHITE);
   }
   //----for all characters in one line-------------------------
   for(int iCol=0; iCol<16; iCol++)
   {
    int iTemp=cStart+iRow*16+iCol;
    if(iTemp<256) {x.drawGlyph(x_, y_, char(iTemp)); } 
    else {iCol=99; iRow=99; iScr=99; break;}
    x_+=w0;
   } //--END OF for all characters in one line------------------
  } //---END OF for all rows in one screen----------------------
  x.sendBuffer();
  delay(4000);
 } //----END OF for all screens (0, 1, 2)-----------------------
}

//_____D1_oop02_oled_ascii.ino________________171230-201212_____
// Show all ASCII characters 32-255 on 0.66" OLED screen
// Screen1 : Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: WeMos D1 Mini or ESP32D1mini
//           OLED Shield: SSD1306, 0.66" 64x48 pixel, I2C
// Special: char(158) = Euro instead of Pt (Pesetas) (158=9E)
// Created by Karl Hartinger, May 14, 2018
// Updates:
// 2018-05-18 Move class files to /src/...
// 2020-12-12 Screen1a instead of Screen1, add display_.begin();
// Released into the public domain.

#include "src/screen1a/D1_class_Screen1a.h"
Screen1   display_(D0);           // OLED_RESET=D0=GPIO16
String X=String((char)177);       // perfect test character

//_____show max. 32 ascii characters beginning with begin_ _____
void screenC(int begin_)
{
 int end_;                 // last char printed(included)
 int x0;                   // start position
 int i,j;                  // counter
 String s1;                // string to print
 //-----check limits--------------------------------------------
 if(begin_<32) begin_=32;
 if(begin_>255) return;
 end_=begin_+31; if(end_>255) end_=255;
 //-----print title line (ASCII number from - to)---------------
 s1=String(begin_)+" - "+String(end_);
 x0=(64-s1.length()*6)/2;
 display_.clearDisplay();
 display_.fillRect(0,0,64,8,WHITE);
 display_.setCursor(x0,0); 
 display_.setTextColor(BLACK, WHITE);
 display_.print(s1);
 display_.setTextColor(WHITE, BLACK);
 //-----write lines---------------------------------------------
 i=0;
 while(begin_<=end_)
 {
  //.....print hex ascii start value of line....................
  i++;
  s1=String(begin_,16);
  display_.fillRect(0,i*10-2,13,10,WHITE);
  display_.setCursor(1,i*10);
  display_.setTextColor(BLACK, WHITE);  
  display_.print(s1);
  display_.setTextColor(WHITE, BLACK);
  //.....build string (max 8 chars).............................
  s1=""; j=0;
  while( (begin_<=end_) && (j<8))
  {
   s1=s1+String((char)begin_);
   begin_++; j++;
  }
  //.....print string with 1...8 ascii values...................
  display_.setCursor(17,i*10);  
  display_.print(s1);
 }
 display_.display();
}

void setup() {
 display_.begin();                          // MUST be called
}

void loop() {
 for(int i=1; i<8; i++)
 {
  screenC(32*i);
  delay(3000);
 }
}

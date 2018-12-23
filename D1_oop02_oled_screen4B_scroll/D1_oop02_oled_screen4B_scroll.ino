//_____D1_oop02_oled_screen4B_scroll.ino____181215-181215_____
// Screen4B: Demo for vertical scrolling of text.
// 1. Scroll a long text
// 2. Display text "Line #" cyclic left, middle and right and 
//    invert every 5th line.
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: 0.66", SSD1306, 64x48 pixel, I2C
// Created by Karl Hartinger, December 15, 2018
// Last modified: -
// Released into the public domain.
#include "src/screen1a/D1_class_Screen1a.h"
#define  VERSION "2018-12-15 D1_oop02_oled_screen4B_scroll"
#define  OLED_RESET 16                 // D3=GPIO0, D0=GPIO16
Screen1  display_;

String sL="The quick brown fox jumps over the lazy dog.";
char align_;
String s1;
int textcounter=1;

void setup() {
 Serial.begin(9600); Serial.println("\r\n"+String(VERSION));
 display_.screen4BClearAll(5,"BACK");
 display_.screen4B(6,"FWD",'c');
 delay(2000);
}

void loop() {
 //-----scroll a longer text----------------------------------
 //.....title.................................................
 display_.screen4BiClear(3,"Text "+String(textcounter),'c');
 if((++textcounter)>99) textcounter=1;
 delay(1000);
 //.....show first line of text to scroll.....................
 display_.screen4B(7,sL,'l');
 //.....scroll all other text lines...........................
 int scrolllines=4+int(sL.length()/10);
 for(int j=0; j<scrolllines; j++) 
 { delay(400); display_.screen4B(7,""); }
 //-----test align and inverted lines-------------------------
 display_.screen4BiClear(3,"align|inv",'c');
 delay(1000);
 for(int i=1; i<18; i++)
 {
  s1="Line "+String(i);
  if(i%3==0) align_='l';
  if(i%3==1) align_='c';
  if(i%3==2) align_='r';
  if(i%5==0) display_.screen4Bi(7,s1,align_);
         else display_.screen4B(7,s1,align_);
  delay(400);
 }
}

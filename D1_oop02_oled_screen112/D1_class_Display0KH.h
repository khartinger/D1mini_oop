//_____D1_class_Display0KH.h__________________170412-170616_____
// Hardware: OLED Shield: SSD1306, 64x48 pixel, I2C
// Uses fonts/font2_5x8.h
// Uses Adafruit libs Adafruit_SSD1306-esp8266-64x48.zip and
//                    Adafruit-GFX-Library-1.1.5.zip
#ifndef D1_CLASS_DISPLAYKH_H
#define D1_CLASS_DISPLAYKH_H
#include <Wire.h>
//#include <Adafruit_SSD1306.h>          // use with Adafruit-lib
//#include <Adafruit_GFX.h>              // use with Adafruit-lib
#include "libs/Adafruit_SSD1306.h"     // use with /libs
#include "libs/Adafruit_GFX.h"         // use with /libs
#include "fonts/font2_5x8.h"
#ifndef OLED_RESET
 #define OLED_RESET D3            // OLED_RESET=D3=GPIO0
#endif

class Display0KH : public Adafruit_SSD1306 {
public:
 Display0KH();
 void screen6 (int line_, String text_, char align_, bool cls_);
 void screen6i(int line_, String text_, char align_, bool cls_);
 void screen5 (int line_, String text_, char align_, bool cls_);
 void screen5i(int line_, String text_, char align_, bool cls_);
 void screen4 (int line_, String text_, char align_, bool cls_);
 void screen4i(int line_, String text_, char align_, bool cls_);
 void screen4ii(int line_,String text_, char align_, bool cls_);
 void screen221(int line_,String text_, char align_, bool cls_);
 //-----screens with big character-lines------------------------
 void screen112(int line_,String text_, char align_, bool cls_);
 void screen13(int line_, String text_, char align_, bool cls_);
 void screen12(int line_, String text_, char align_, bool cls_);
 void screen2 (int line_, String text_, char align_, bool cls_); 
private:
 String mytrim(int max_, String txt_, char align);
 void scr6(int line_, String text_, char align_, bool cls_, bool invert_);
 void scr5(int line_, String text_, char align_, bool cls_, bool invert_);
 void scr4(int line_, String text_, char align_, bool cls_, bool invert_);
};

//_____constructor______________________________________________
Display0KH::Display0KH() {
 Adafruit_SSD1306(OLED_RESET);
 this->begin(SSD1306_SWITCHCAPVCC, 0x3C);   //I2C addr 0x3C
 this->clearDisplay();                      //Clear display-buff
 this->setTextColor(WHITE);
 this->setFont(&font2_5x8);
 //this->setFont(NULL);
 this->setTextSize(1);
}

//**************************************************************
// screens with only single size lines
//**************************************************************

//_____display: 1 area, 6 lines, 10 chars/line, no border_______
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen6(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 scr6(line_, text_, align_, cls_, false);
}

//_____display: 1 area, 6 lines, 10 chars/line, no border_______
// first line inverted
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen6i(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 scr6(line_, text_, align_, cls_, true);
}

//_____display: 1 area, 5 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen5(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 scr5(line_, text_, align_, cls_, false);
}

//_____display: 1 area, 5 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen5i(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 scr5(line_, text_, align_, cls_, true);
}

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen4(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 scr4(line_, text_, align_, cls_, false);
}

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen4i(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 scr4(line_, text_, align_, cls_, true);
}

//_____display: 3 areas, 5 lines, 10 chars/line, borders________
// border around area 1 and 2
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen221(int line_, String text_, 
  char align_='l', bool cls_=false)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);   //clear area before writing
 align_=tolower(align_);
 if(cls_) this->clearDisplay();
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, line 1 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,1,62,9,BLACK); //clear line 1
   this->setCursor(dx+2,2);                 //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
   case 2: //-----area 1, line 2--------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,10,62,8,BLACK);//clear line 2
   this->setCursor(dx+2,10);                //
   this->print(mytrim(max_, s_, align_));   //write text
   break;
  case 3: //-----area 2, line 3 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,19,62,9,BLACK);//clear line 3
   this->setCursor(dx+2,20);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
   case 4: //-----area 2, line 4--------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,28,62,10,BLACK);//clear line 4
   this->setCursor(dx+2,29);                //
   this->print(mytrim(max_, s_, align_));   //write text
   break;  
  case 5: //-----area 3, one line out of the rectangle----------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,39,64,9,BLACK);//clear line 5
   this->setCursor(dx,40);                  //dx only!
   this->print(mytrim(max_, s_, align_));   //write text
   break;
  default: break;
 }
 //-----draw lines----------------------------------------------
 this->drawRect(0,0,64,39,WHITE);      // border around line 1-4
 this->drawLine(1,18,62,18,WHITE);     // horizontal line
 this->display();                      // show buffer
}

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// line 1 and 3 inverted
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen4ii(int line_, String text_, 
  char align_='c', bool cls_=false)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);             //clear area before
 align_=tolower(align_);                    //only lower chars
 if(cls_) this->clearDisplay();             //clear buffer
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, line 1 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) {
    this->fillRect(1,1,62,10,WHITE);       //clear line 1
   }
   this->setCursor(dx+3,2);                //
   this->setTextColor(BLACK,WHITE);
   this->print(mytrim(max_, s_, align_));  //write text
   this->setTextColor(WHITE,BLACK);
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 2: //-----area 1, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,11,62,13,BLACK);//clear line 2
   this->setCursor(dx+3,13);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 3: //-----area 1, line 3 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) {
    this->fillRect(1,24,62,11,WHITE);      //clear line 1
   }
   this->setCursor(dx+3,26);               //
   this->setTextColor(BLACK,WHITE);
   this->print(mytrim(max_, s_, align_));  //write text
   this->setTextColor(WHITE,BLACK);
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 4: //-----area 1, line 4 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,35,62,12,BLACK);//clear line 4
   this->setCursor(dx+3,37);                //
   this->print(mytrim(max_, s_, align_));   //write text
   break;
  default: break;
 }
 this->drawRect(0,0,64,48,WHITE);           // border
 this->display();                           //show buffer
}

//**************************************************************
// screens with single and double size lines
//**************************************************************

//_____display: 2 areas, 1+2 lines, 5/10 chars/line, border_____
// area 1: double text size
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen12(int line_, String text_, 
  char align_='c', bool cls_=false)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);   // clear area before writing
 align_=tolower(align_);
 if(cls_) this->clearDisplay();
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, double text size-----------------------
   max_=5;
   if(align_=='c'){ if((len_<max_) &&((len_&1)==0)) dx=6; }
   if(clr_) this->fillRect(1,1,62,27,BLACK);     //clear area 1 
   this->setCursor(dx+3,7);
   this->setTextSize(2);                    //charsize 10x16 pix
   this->println(mytrim(max_, text_, align_));   //write line 1  
   this->setTextSize(1);                    //charsize 5x8 pixel
   break;
  case 2: //-----area 2, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,29,64,19,BLACK);//clear line 2
   this->setCursor(dx,31);                   //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
   case 3: //-----area 2, line 3--------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,40,64,8,BLACK);//clear line 4
   this->setCursor(dx,40);                   //
   this->print(mytrim(max_, s_, align_));   //write text
   break;  
  default: break;
 }
 //-----draw lines----------------------------------------------
 this->drawRect(0,0,64,29,WHITE);      // border around area 1
 this->display();                      // show buffer
}

//_____display: 2 areas: 1/3 lines, 5/10 chars/line_____________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen13(int line_, String text_, 
  char align_='c', bool cls_=false)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);   // clear area before writing
 align_=tolower(align_);
 if(cls_) this->clearDisplay();
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, double text size-----------------------
   max_=5;
   if(align_=='c'){ if((len_<max_) &&((len_&1)==0)) dx=6; }
   if(clr_) this->fillRect(1,1,62,19,BLACK);     //clear area 1 
   this->setCursor(dx+3,3);
   this->setTextSize(2);                    //charsize 10x16 pix
   this->println(mytrim(max_, text_, align_));   //write line 1  
   this->setTextSize(1);                    //charsize 5x8 pixel
   break;
  case 2: //-----area 2, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(0,21,64,10,BLACK);//clear line 2
   this->setCursor(dx,22);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();  
  case 3: //-----area 2, line 3 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(0,30,64,10,BLACK);//clear line 3
   this->setCursor(dx,31);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();  
  case 4: //-----area 2, line 4---------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,40,64,8,BLACK);//clear line 4
   this->setCursor(dx,40);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   break; 
  default: break;
 }
 //-----draw lines----------------------------------------------
 this->drawRect(0,0,64,21,WHITE);      // border around area 1
 this->display();                      // show buffer
}

//_____display: 3 areas, 1+1+2 lines, 10/5/10 chars/line________
// area 1: inverted, area 2: double text size
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen112(int line_, String text_, 
  char align_='c', bool cls_=false)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);   //clear area before writing
 align_=tolower(align_);
 if(cls_) this->clearDisplay();
 //-----write text----------------------------------------------
 switch(line_)
 {
   case 1: //-----area 1, line 1, inverted----------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,0,64,10,WHITE);//clear line 1
   this->setCursor(dx+2,1);                 //
   this->setTextColor(BLACK,WHITE);
   this->print(mytrim(max_, s_, align_));   //write text
   this->setTextColor(WHITE,BLACK);
   break; 
  case 2: //-----area 2, double text size-----------------------
   max_=5;
   if(align_=='c'){ if((len_<max_) &&((len_&1)==0)) dx=6; }
   if(clr_)this->fillRect(1,10,62,19,BLACK);//clear area 2 
   this->setCursor(dx+3,12);
   this->setTextSize(2);                    //charsize 10x16 pix
   this->println(mytrim(max_, text_, align_));   //write line 1  
   this->setTextSize(1);                    //charsize 5x8 pixel
   break;
  case 3: //-----area 3, line 1 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(0,29,64,11,BLACK);//clear line 3
   this->setCursor(dx,31);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
   case 4: //-----area 3, line 2--------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,40,64,8,BLACK);//clear line 4
   this->setCursor(dx,40);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   break;  
  default: break;
 }
 //-----draw lines----------------------------------------------
 this->drawRect(0,0,64,30,WHITE);      // border around area 1
 this->display();                      // show buffer
}

//_____display: 1 area, 2 lines, 5 chars/line, border___________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::screen2(int line_, String text_, 
  char align_='c', bool cls_=false)
{
 int dx=0;
 int max_=5;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);             //clear area before
 align_=tolower(align_);                    //only lower chars
 if(cls_) this->clearDisplay();             //clear buffer
 this->setTextSize(2);                      //char size 10x16px
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, line 1, double text size---------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)==0)) dx=6; }
   if(clr_) this->fillRect(1,1,62,25,BLACK);//clear line 1
   this->setCursor(dx+3+dx,7);              //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 2: //-----area 1, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,26,62,22,BLACK);//clear line 2
   this->setCursor(dx+3+dx,26);             //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
  default: break;
 }
 this->setTextSize(1);                      //charsize 5x8 pixel
 this->drawRect(0,0,64,48,WHITE);           // border
 this->display();                           //show buffer
}

//**************************************************************
// helper functions
//**************************************************************

//_____make string with max chars (add blank before/after)______
String Display0KH::mytrim(int max_, String txt_, char align_='l')
{
 String s=txt_;
 //-----add leading blanks, if align_ needs them----------------
 int blank_=0;
 int len_=s.length();
 if(align_=='c') blank_=(max_-len_)/2;
 if(align_=='r') { if(len_<max_) blank_=max_-len_; }
 //-----add blanks before text----------------------------------
 for(int i=0; i<blank_; i++) s=" "+s;
 if(len_>max_) 
 { //-----text too long, truncate text--------------------------
  s=s.substring(0,max_);
 }
 else //-----text too short, add blank--------------------------
 {
  for(int i=len_; i<max_; i++) s+=" ";
 }
 return s;
}

//_____display: 1 area, 6 lines, 10 chars/line, no border_______
// first line may be inverted
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::scr6(int line_, String text_, 
  char align_, bool cls_, bool invert_)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);             //clear area before
 align_=tolower(align_);                    //only lower chars
 if(cls_) this->clearDisplay();             //clear buffer
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, line 1 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) {
    if(invert_) this->fillRect(0,0,64,8,WHITE);  //clear line 1
           else this->fillRect(0,0,64,8,BLACK);  //clear line 1
   }
   this->setCursor(dx,0);                   //
   if(invert_) this->setTextColor(BLACK,WHITE);
   this->print(mytrim(max_, s_, align_));   //write text
   if(invert_) this->setTextColor(WHITE,BLACK);
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 2: //-----area 1, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,8,64,8,BLACK); //clear line 2
   this->setCursor(dx,8);                   //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 3: //-----area 1, line 3 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,16,64,8,BLACK);//clear line 3
   this->setCursor(dx,16);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 4: //-----area 1, line 4 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,24,64,8,BLACK);//clear line 4
   this->setCursor(dx,24);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();  
  case 5: //-----area 1, line 5 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,32,64,8,BLACK);//clear line 5
   this->setCursor(dx,32);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();  
  case 6: //-----area 1, line 6---------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(0,40,64,8,BLACK);//clear line 6
   this->setCursor(dx,40);                  //
   this->print(mytrim(max_, s_, align_));   //write text
   break;
  default: break;
 }
 this->display();                           //show buffer
}

//_____display: 1 area, 5 lines, 10 chars/line, border__________
// first line may be inverted
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::scr5(int line_, String text_, 
  char align_, bool cls_, bool invert_)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);             //clear area before
 align_=tolower(align_);                    //only lower chars
 if(cls_) this->clearDisplay();             //clear buffer
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, line 1 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) {
    if(invert_) this->fillRect(1,1,62,10,WHITE); //clear line 1
           else this->fillRect(1,1,62,10,BLACK); //clear line 1
   }
   this->setCursor(dx+3,2);                 //
   if(invert_) this->setTextColor(BLACK,WHITE);
   this->print(mytrim(max_, s_, align_));   //write text
   if(invert_) this->setTextColor(WHITE,BLACK);
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 2: //-----area 1, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,11,62,9,BLACK);//clear line 2
   this->setCursor(dx+3,11);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 3: //-----area 1, line 3 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,20,62,9,BLACK);//clear line 3
   this->setCursor(dx+3,20);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 4: //-----area 1, line 4 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,29,62,9,BLACK);//clear line 4
   this->setCursor(dx+3,29);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();  
  case 5: //-----area 1, line 5---------------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_) this->fillRect(1,38,62,9,BLACK);//clear line 5
   this->setCursor(dx+3,38);                //
   this->print(mytrim(max_, s_, align_));   //write text
   break;
  default: break;
 }
 this->drawRect(0,0,64,48,WHITE);           // border
 this->display();                           //show buffer
}

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Display0KH::scr4(int line_, String text_, 
  char align_, bool cls_, bool invert_)
{
 int dx=0;
 int max_=10;
 String s_=text_;
 int len_=s_.length();
 bool clr_=isLowerCase(align_);             //clear area before
 align_=tolower(align_);                    //only lower chars
 if(cls_) this->clearDisplay();             //clear buffer
 //-----write text----------------------------------------------
 switch(line_)
 {
  case 1: //-----area 1, line 1 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(invert_)
   {
    if(clr_) {
     this->fillRect(1,1,62,12,WHITE);       //clear line 1
     this->drawLine(1,13,62,13,BLACK);      // horizontal line
    }
    this->setCursor(dx+3,2);                //
    this->setTextColor(BLACK,WHITE);
    this->print(mytrim(max_, s_, align_));  //write text
    this->setTextColor(WHITE,BLACK);
   }
   else
   {
    if(clr_)this->fillRect(1,1,62,13,BLACK);//clear line 1
    this->setCursor(dx+3,4);                //
    this->print(mytrim(max_, s_, align_));  //write text
   }
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 2: //-----area 1, line 2 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,14,62,12,BLACK);//clear line 2
   this->setCursor(dx+3,15);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 3: //-----area 1, line 3 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,26,62,11,BLACK);//clear line 3
   this->setCursor(dx+3,26);                //
   this->print(mytrim(max_, s_, align_));   //write text
   if(len_<=max_) break;
   s_=s_.substring(max_);
   len_=s_.length();
  case 4: //-----area 1, line 4 or more-------------------------
   if(align_=='c'){ if((len_<max_) &&((len_&1)>0)) dx=3; }
   if(clr_)this->fillRect(1,37,62,11,BLACK);//clear line 4
   this->setCursor(dx+3,37);                //
   this->print(mytrim(max_, s_, align_));   //write text
   break;
  default: break;
 }
 this->drawRect(0,0,64,48,WHITE);           // border
 this->display();                           //show buffer
}
#endif

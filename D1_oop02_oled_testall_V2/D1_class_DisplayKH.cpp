//_____D1_class_DisplayKH.cpp_________________170412-171218_____
// The class DisplayKH extends the class Adafruit_SSD1306 
// with screen methods to write text on the display. Also a
// 5x8 pixel font is included in this class.
// Additionally you can use all commands from Adafruit class.
//
// The Adafruit-files in directory /libs are copied from libs
//  * Adafruit_SSD1306-esp8266-64x48.zip and
//  * Adafruit-GFX-Library-1.1.5.zip
// When you have Adafruit libs installed, delete directory /libs
//
// Hardware: (1) WeMos D1 mini
//           (2) OLED Shield: SSD1306, 64x48 pixel, I2C
//                            default reset pin D3 = GPIO0
//
// Changes 2017-12-17 by Karl Hartinger, December 18, 2017.
// + devide (old) D1_class_DisplayKH.h into *.h and *.cpp
// + constuctor(oled_resetpin) added
// + dot.. and screenClear methods added
// Released into the public domain.

#include "D1_class_DisplayKH.h"        //

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor 1____________________________________________
DisplayKH::DisplayKH() {setup(OLED_RESET);}

//_____constructor 2____________________________________________
DisplayKH::DisplayKH(int oled_resetpin) {setup(oled_resetpin);}

//_____setup properties_________________________________________
void DisplayKH::setup(int pin_num) {
 Adafruit_SSD1306((int8_t)pin_num);
 this->begin(SSD1306_SWITCHCAPVCC, OLED_I2C);    //I2C addr 0x3C
 this->clearDisplay();                      //clear display-buff
 this->setTextColor(WHITE);
 this->setFont(&fontKH_5x8);
 //this->setFont(NULL);
 this->setTextSize(1);
 this->display();                           //show display-buff
 dotCounter=0;
}

//**************************************************************
// screens with only single size lines
//**************************************************************

//_____display: 1 area, 6 lines, 10 chars/line, no border_______
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen6(int line_, String text_)
{ scr6(line_, text_, 'l', false, false); }

void DisplayKH::screen6(int line_, String text_, char align_)
{ scr6(line_, text_, align_, false, false); }

void DisplayKH::screen6Clear(int line_, String text_, char align_)
{ scr6(line_, text_, align_, true, false); }

//_____display: 1 area, 6 lines, 10 chars/line, no border_______
// first line inverted
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen6i(int line_, String text_)
{ scr6(line_, text_, 'l', false, true); }

void DisplayKH::screen6i(int line_, String text_, char align_)
{ scr6(line_, text_, align_, false, true); }

void DisplayKH::screen6iClear(int line_, String text_, char align_)
{ scr6(line_, text_, align_, true, true); }

//_____display: 1 area, 5 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen5(int line_, String text_)
{ scr5(line_, text_, 'l', false, false); }

void DisplayKH::screen5(int line_, String text_, char align_)
{ scr5(line_, text_, align_, false, false); }

void DisplayKH::screen5Clear(int line_, String text_, char align_)
{ scr5(line_, text_, align_, true, false); }

//_____display: 1 area, 5 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen5i(int line_, String text_) 
{ scr5(line_, text_, 'l', false, true); }

void DisplayKH::screen5i(int line_, String text_, char align_)
{ scr5(line_, text_, align_, false, true); }

void DisplayKH::screen5iClear(int line_, String text_, char align_)
{ scr5(line_, text_, align_, true, true); }

//_____display: 3 areas, 5 lines, 10 chars/line, borders________
// border around area 1 and 2
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen221(int line_, String text_, 
  char align_, bool cls_)
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

void DisplayKH::screen221(int line_, String text_)
{ screen221(line_, text_, 'l', false); }

void DisplayKH::screen221(int line_, String text_, char align_)
{ screen221(line_, text_, align_, false); }

void DisplayKH::screen221Clear(int line_, String text_, char align_)
{ screen221(line_, text_, align_, true); }

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen4(int line_, String text_)
{ scr4(line_, text_, 'l', false, false); }

void DisplayKH::screen4(int line_, String text_, char align_)
{ scr4(line_, text_, align_, false, false); }

void DisplayKH::screen4Clear(int line_, String text_, char align_)
{ scr4(line_, text_, align_, true, false); }

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen4i(int line_, String text_) 
{ scr4(line_, text_, 'l', false, true); }

void DisplayKH::screen4i(int line_, String text_, char align_)
{ scr4(line_, text_, align_, false, true); }

void DisplayKH::screen4iClear(int line_, String text_, char align_)
{ scr4(line_, text_, align_, true, true); }

//_____display: 1 area, 4 lines, 10 chars/line, border__________
// line 1 and 3 inverted
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen4ii(int line_, String text_, 
  char align_, bool cls_)
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
   if(clr_)this->fillRect(1,11,62,12,BLACK);//clear line 2
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

void DisplayKH::screen4ii(int line_, String text_, char align_)
{ screen4ii(line_, text_, align_, false); }

void DisplayKH::screen4ii(int line_, String text_)
{ screen4ii(line_, text_, 'c', false); }

void DisplayKH::screen4iiClear(int line_, String text_, char align_)
{ screen4ii(line_, text_, align_, true); }


//**************************************************************
// screens with single and double size lines
//**************************************************************

//_____display: 3 areas, 1+1+2 lines, 10/5/10 chars/line________
// area 1: inverted, area 2: double text size
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen112(int line_, String text_, 
  char align_, bool cls_)
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
   this->println(mytrim(max_, text_, align_));   //write line 2  
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

void DisplayKH::screen112(int line_, String text_)
{ screen112(line_, text_, 'c', false); }

void DisplayKH::screen112(int line_, String text_, char align_)
{ screen112(line_, text_, align_, false); }

void DisplayKH::screen112Clear(int line_, String text_, char align_)
{ screen112(line_, text_, align_, true); }


//_____display: 2 areas: 1/3 lines, 5/10 chars/line_____________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen13(int line_, String text_, 
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

void DisplayKH::screen13(int line_, String text_)
{ screen13(line_, text_, 'c', false); }

void DisplayKH::screen13(int line_, String text_, char align_)
{ screen13(line_, text_, align_, false); }

void DisplayKH::screen13Clear(int line_, String text_, char align_)
{ screen13(line_, text_, align_, true); }

//_____display: 2 areas, 1+2 lines, 5/10 chars/line, border_____
// area 1: double text size
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen12(int line_, String text_, 
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

void DisplayKH::screen12(int line_, String text_)
{ screen12(line_, text_, 'c', false); }

void DisplayKH::screen12(int line_, String text_, char align_)
{ screen12(line_, text_, align_, false); }

void DisplayKH::screen12Clear(int line_, String text_, char align_)
{ screen12(line_, text_, align_, true); }

//_____display: 1 area, 2 lines, 5 chars/line, border___________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void DisplayKH::screen2(int line_, String text_, 
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

void DisplayKH::screen2(int line_, String text_)
{ screen2(line_, text_, 'c', false); }

void DisplayKH::screen2(int line_, String text_, char align_)
{ screen2(line_, text_, align_, false); }

void DisplayKH::screen2Clear(int line_, String text_, char align_)
{ screen2(line_, text_, align_, true); }

//**************************************************************
// special screens
//**************************************************************

//_____clear display and draw a border__________________________
void DisplayKH::dotClear()
{
 this->dotClear("");
}

//_____clear display, write line 6 and draw a border____________
void DisplayKH::dotClear(String line6)
{
 dotCounter=0;
 this->clearDisplay();
 this->fillRect(1,1,63,38,BLACK);      // clear rectangle
 this->drawRect(0,0,64,39,WHITE);      // border around
 this->dotLine(line6);                 // set line 6, display
}

//_____draw a dot_______________________________________________
void DisplayKH::dot()
{
 if(dotCounter<50)
 {
  if(dotCounter%10==0)
   this->setCursor(2,(dotCounter/10)*8);
  dotCounter++;
  this->print(DOT);                    // print dot
  this->display();                     // show buffer
 }  
 else
 {
  dotCounter=0;
  this->fillRect(1,1,63,38,BLACK);     // clear rectangle
  this->drawRect(0,0,64,39,WHITE);     // border around
  this->setCursor(0,0);                // set cursor start
 }
}

//_____dot screen: write line 6_________________________________
void DisplayKH::dotLine(String line6)
{
 this->fillRect(0,39,64,9,BLACK);     // clear text area
 this->setCursor(0,40);                // set cursor start
 this->print(line6);                   // print line 6
 int posx = 2 + 6*(dotCounter%10);     // old cursor pos x
 int posy = 0 + 8*(dotCounter/10);     // old cursor pos y
 this->setCursor(posx, posy);          // set cursor
 this->display();                      // show buffer
}

//**************************************************************
// helper functions
//**************************************************************

//_____make string with max chars (add blank before/after)______
String DisplayKH::mytrim(int max_, String txt_, char align_='l')
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
void DisplayKH::scr6(int line_, String text_, 
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
void DisplayKH::scr5(int line_, String text_, 
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
void DisplayKH::scr4(int line_, String text_, 
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
     this->drawLine(1,12,62,12,BLACK);      // horizontal line
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

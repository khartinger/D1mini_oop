//_____D1_class_Screen096.cpp_________________191217-191223_____
// The class Screen096 extends the classes SSD1306Wire and
// OLEDDisplay with screen methods to write text on the display.
// * line number <0: display line inverted
// * line 0 = line 1 with border
// Also included in this file:
// * a table to convert codepage 437 to utf8 and vice versa
//   (table_cp437_utf8)
// * four fonts
//   fontKH_cp437_16x7, fontKH_cp437_8x6, 
//   fontKH_utf8_16x8,  fontKH_utf8_12x6
// Hardware: (1) WeMos D1 mini
//           (2) OLED Shield: SSD1306, 128x64 pixel, I2C
// Created by Karl Hartinger, December 17, 2019.
// Last modified: 2019-12-23 add screenXXDot lines
// Released into the public domain.

#include "D1_class_Screen096.h"         //

void Screen096::begin()
{
 setGeometry(GEOMETRY_128_64);
 this->_address = OLED_I2C;
 this->_sda = SDA;
 this->_scl = SCL;
 _cp437=false;
 for(int i=0; i<DOT_COUNTER_MAX; i++) dotCounter[i]=0;
 init();
 flipScreenVertically();
 if(_cp437)
 {
  setFontTitle(FONT_CP437_TITLE);
  setFontText(FONT_CP437_TEXT);
  setFont(FONT_CP437_TEXT);
 }
 else
 {
  setFontTitle(FONT_UTF8_TITLE);
  setFontText(FONT_UTF8_TEXT);
  setFont(FONT_UTF8_TEXT);
 }
}

//_____switch screen output mode to codepage 437________________
void Screen096::useCP437(bool do_it) { 
 _cp437=do_it;  
 setFontTitle(FONT_CP437_TITLE);
 setFontText(FONT_CP437_TEXT);
 setFont(FONT_CP437_TEXT);
}

// *************************************************************
// two-color-screens with title and text
// *************************************************************

//_____display: title (16char) + 4 lines (21char), no border____
// align: l=left, c=center, r=right, L=left+overwrite, C, R...

void Screen096::screen14Clear(int line_, const String &text_)
{ screen14(line_, text_, 'l', true); }

void Screen096::screen14Clear(int line_, const String &text_, char align_)
{ screen14(line_, text_, align_, true); }

void Screen096::screen14(int line_, const String &text_)
{ screen14(line_, text_, 'l', false); }

void Screen096::screen14(int line_, const String &text_, char align_)
{ screen14(line_, text_, align_, false); }

void Screen096::screen14(int line_, const String &text_, 
  char align_, bool cls_)
{ screenXY(line_, text_, align_, cls_, 16, 12); }

void Screen096::screen14Dot(int line_)
{ 
  if(line_==1) screenXYDot(0,16,line_);
  else screenXYDot(16-12,12,line_); 
}

void Screen096::screen14Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen14Dot(line_);
  }
 }
 screen14Dot(line_);
}

//_____display: title (16char) + 5 lines (21char), no border____
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Screen096::screen15Clear(int line_, const String &text_)
{ screen15(line_, text_, 'l', true); }

void Screen096::screen15Clear(int line_, const String &text_, char align_)
{ screen15(line_, text_, align_, true); }

void Screen096::screen15(int line_, const String &text_)
{ screen15(line_, text_, 'l', false); }

void Screen096::screen15(int line_, const String &text_, char align_)
{ screen15(line_, text_, align_, false); }

void Screen096::screen15(int line_, const String &text_, 
  char align_, bool cls_)
{ screenXY(line_, text_, align_, cls_, 18, 9); }

void Screen096::screen15Dot(int line_)
{ 
  if(line_==1) screenXYDot(0,16,line_);
  else screenXYDot(18-9,9,line_); 
}

void Screen096::screen15Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen15Dot(line_);
  }
 }
 screen15Dot(line_);
}

//_____display: title + 6 lines, no border______________________
void Screen096::screen16Clear(int line_, const String &text_)
{ screen16(line_, text_, 'l', true); }

void Screen096::screen16Clear(int line_, const String &text_, char align_)
{ screen16(line_, text_, align_, true); }

void Screen096::screen16(int line_, const String &text_)
{ screen16(line_, text_, 'l', false); }

void Screen096::screen16(int line_, const String &text_, char align_)
{ screen16(line_, text_, align_, false); }

void Screen096::screen16(int line_, const String &text_, 
  char align_, bool cls_)
{ screenXY(line_, text_, align_, cls_, 16, 8); }

void Screen096::screen16Dot(int line_)
{ 
  if(line_==1) screenXYDot(0,16,line_);
  else screenXYDot(16-8,8,line_); 
}

void Screen096::screen16Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen16Dot(line_);
  }
 }
 screen16Dot(line_);
}

// *************************************************************
// one-color-screen only with text (no title)
// *************************************************************

//_____display: 7 lines, no border______________________________
void Screen096::screen7Clear(int line_, const String &text_)
{ screen7(line_, text_, 'l', true); }

void Screen096::screen7Clear(int line_, const String &text_, char align_)
{ screen7(line_, text_, align_, true); }

void Screen096::screen7(int line_, const String &text_)
{ screen7(line_, text_, 'l', false); }

void Screen096::screen7(int line_, const String &text_, char align_)
{ screen7(line_, text_, align_, false); }

void Screen096::screen7(int line_, const String &text_, 
  char align_, bool cls_)
{ screenX(line_, text_, align_, cls_, 0, 9); }

void Screen096::screen7Dot(int line_)
{ screenXDot(0,9,line_); }

void Screen096::screen7Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen7Dot(line_);
  }
 }
 screen7Dot(line_);
}

//_____display: 8 lines, no border______________________________
void Screen096::screen8Clear(int line_, const String &text_)
{ screen8(line_, text_, 'l', true); }

void Screen096::screen8Clear(int line_, const String &text_, char align_)
{ screen8(line_, text_, align_, true); }

void Screen096::screen8(int line_, const String &text_)
{ screen8(line_, text_, 'l', false); }

void Screen096::screen8(int line_, const String &text_, char align_)
{ screen8(line_, text_, align_, false); }

void Screen096::screen8(int line_, const String &text_, 
  char align_, bool cls_)
{ screenX(line_, text_, align_, cls_, 0, 8); }


void Screen096::screen8Dot(int line_)
{ screenXDot(0,8,line_); }

void Screen096::screen8Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen8Dot(line_);
  }
 }
 screen8Dot(line_);
}


//**************************************************************
// special screens
//**************************************************************

//_____clear display and draw a border__________________________
void Screen096::dotClear()
{
 dotClear("");
}

//_____clear display, write title and draw a border_____________
// dotCounter is set to 0, NO dot is plotted
void Screen096::dotClear(String title_)
{
 dotCounter[0]=0;                      // new start
 clear();                              // clear display buffer
 setColor(BLACK);                      // background=black
 fillRect(0,0,128,64);                 // clear all
 setColor(WHITE);                      // draw white
 drawRect(0,16,128,48);                // draw a rectangle
 setFont(fontTitle);                   // set title font
 drawString(0,0,title_);               // write title
 setFont(fontText);                    // set text font
}

//_____draw a dot_______________________________________________
void Screen096::dot()
{
 if(dotCounter[0]>=100) 
 {
  dotCounter[0]=0;
  setColor(BLACK);
  fillRect(0,16,128,48);               // clear title
  setColor(WHITE);
  drawRect(0,16,128,48);               // draw a rectangle...
 }
 int xpos =  4 + 6*(dotCounter[0]%20); // cursor pos x
 int ypos = 20 + 8*int(dotCounter[0]/20); // cursor pos y
 drawString(xpos,ypos,DOT);            // draw dot
 dotCounter[0]++;                      // next vlue
 display();                            // show buffer
}

//_____dot screen: write title and plot a dot___________________
void Screen096::dotTitle(String title_)
{
 dotTitleOnly(title_);                 // write title
 dot();                                // draw a dot
}

//_____dot screen: write title__________________________________
void Screen096::dotTitleOnly(String title_)
{
 setColor(BLACK);                      // background=black
 fillRect(0,0,128,16);                 // clear title area
 setColor(WHITE);                      // draw white
 setFont(fontTitle);                   // set title font
 drawString(0,0,title_);               // write title
 setFont(fontText);                    // set text font
 display();                            // show buffer
}

//**************************************************************
// helper functions
//**************************************************************

//_____display lines (21char)___________________________________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Screen096::screenX(int line_, const String &text_, 
 char align_, bool cls_, int y0_, int dline_)
{
 int dx=0;                             // left space
 int dy=0;                             // char height
 int dx2=0;                            // half char width
 int dy1=0;                            // start pixel of line
 int maxc_;                            // max. chars in one line
 OLEDDISPLAY_COLOR colorf=WHITE;       // front color
 OLEDDISPLAY_COLOR colorb=BLACK;       // background color
 String s_=text_;                      // remaining chars of text
 String s1;                            // text in this line
 int lenc_;                            // string length in chars
 lenc_=utf8length(s_);                 //  string length in chars
 bool clr_=isLowerCase(align_);        // clear area before writing
 align_=tolower(align_);               // c,r,l
 if(cls_) {                            // do clear screen
  clear();                             // clear screen
  for(int i=0; i<DOT_COUNTER_MAX; i++) dotCounter[i]=0;
 }
 if(line_<0)
 {
  colorf=BLACK;
  colorb=WHITE;
  line_=-line_;
 }
 setFont(fontText);                    // set text font
 dy=getFontHeight();                   // char height
 dx2=getFontWidth()/2;                 // half char
 maxc_=int(getWidth()/getFontWidth()); // max. chars in one line
 //-----write text----------------------------------------------
 setColor(colorf);
 for(int l1=line_; l1<9; l1++)
 {
  dy1=y0_+(l1-1)*dline_;
  if(dy1+dy>64) break;
  if(align_=='c'){ if((lenc_<maxc_) &&((lenc_&1)>0)) dx=dx2; }
  if(clr_) { 
   setColor(colorb);
   fillRect(0,dy1,128,dline_);         // clear line
  }
  setColor(colorf);
  s1=mytrim(maxc_, s_, align_);
  drawString(dx,dy1,s1);
  if(lenc_<=maxc_) break;
  s_=utf8substring(s_,maxc_,0); 
  lenc_=utf8length(s_);
 }
 //-----draw lines, show display--------------------------------
 display();                            // show buffer
 setColor(WHITE);
}

//_____display lines (21char)___________________________________
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
// line number <0: invert line
void Screen096::screenXY(int line_, const String &text_, 
 char align_, bool cls_, int y0_, int dline_)
{
 int dx=0;                             // left space
 int dx2=0;                            // half char width
 int maxc_;                            // max. chars in one line
 OLEDDISPLAY_COLOR colorf=WHITE;       // front color
 OLEDDISPLAY_COLOR colorb=BLACK;       // background color
 String s_=text_;                      // remaining chars of text
 String s1;                            // text in this line
 int lenc_;                            // string length in chars
 lenc_=utf8length(s_);                 //  string length in chars
 bool clr_=isLowerCase(align_);        // clear area before writing
 align_=tolower(align_);               // c,r,l
 if(cls_) {                            // do clear screen
  clear();                             // clear screen
  for(int i=0; i<DOT_COUNTER_MAX; i++) dotCounter[i]=0;
 }
 if(line_==1 || line_==0 || line_==-1)
 {//-----write title--------------------------------------------
  if(line_==-1)
  {
   colorf=BLACK;
   colorb=WHITE;
  }
  setFont(fontTitle);                  // set title font
  dx2=getFontWidth()/2;                // half char
  maxc_=int(getWidth()/getFontWidth());// chars at 1 line
  if(align_=='c'){ if((lenc_<maxc_) &&((lenc_&1)>0)) dx=dx2; }
  if(clr_) { 
   setColor(colorb);
   if(line_==0) fillRect(1,1,getWidth()-2,14); //clear line 1
   else fillRect(0,0,getWidth(),16);        //clear line 1
  }
  setColor(colorf);
  s1=mytrim(maxc_, s_, align_);
  drawString(dx,0,s1);
  if(line_==0) drawRect(0,0,getWidth(),16); // border line 1
  //-----draw lines, show display-------------------------------
  display();                           // show buffer
  setColor(WHITE);
 }
 else
 {//-----write text---------------------------------------------
  if(line_>0) line_--; else line_++; 
  screenX(line_, text_, align_, cls_, y0_, dline_);
 }
}

//_____plot dots in a text line_________________________________
void Screen096::screenXDot(int y0_, int dline_, int line_)
{
 int dx=0;                             // left space
 int dx1;                              // width of 1 char
 int xpos;                             // DOT x-position
 int ypos;                             // DOT y-position
 int maxc_;                            // max. chars in one line
 if((line_<1)||(line_>DOT_COUNTER_MAX)) return;
 setFont(fontText);                    // set text font
 dx1=getFontWidth();                   // width of 1 char
 maxc_=int(getWidth()/dx1);            // chars at 1 line
 if((dotCounter[line_]>=maxc_)||(dotCounter[line_]<1))
  dotCounter[line_]=0;
 ypos=y0_+dline_*(line_-1);            // y position of dot
 xpos=dx+dx1*dotCounter[line_];        // x position of dot
 if(dotCounter[line_]==0)
 {
  setColor(BLACK);
  fillRect(0,ypos,getWidth(),dline_);  // clear line
  setColor(WHITE);
 }
 else
 {
  drawString(xpos,ypos,DOT);           // draw dot
 }
 //-----show display--------------------------------------------
 dotCounter[line_]++;                  // next value
 display();                            // show buffer
}

//_____plot dots in a title or text line________________________
void Screen096::screenXYDot(int y0_, int dline_, int line_)
{
 int dx=0;                             // left space
 int dx1;                              // width of 1 char
 int xpos=0;                           // DOT x-position
 int ypos=y0_;                         // DOT y-position
 int maxc_;                            // max. chars in one line
 if(line_==1)
 {//-----write title--------------------------------------------
  setFont(fontTitle);                  // set title font
  dx1=getFontWidth();                  // width of 1 char
  xpos = dx + dx1*dotCounter[1];       // cursor pos x
  ypos = y0_;                          // cursor pos y
  maxc_=int(getWidth()/dx1);           // chars at 1 line
   if(dotCounter[line_]>=maxc_ ||dotCounter[line_]<1)
  dotCounter[line_]=0;
 if(dotCounter[line_]==0)
  {
   dotCounter[1]=0;
   setColor(BLACK);
   fillRect(0,0,getWidth(),16);        // clear title
   setColor(WHITE);
   drawRect(0,0,getWidth(),16);        // draw a rectangle
  }
  else
  {
   drawString(xpos,ypos,DOT);          // draw dot
  }
  dotCounter[1]++;                    // next value
  //-----show display line 1------------------------------------
  display();                           // show buffer
 }
 else
 {//-----write line >1------------------------------------------
  screenXDot(y0_, dline_, line_);
 }
}

//_____make string with max chars (add blank before/after)______
String Screen096::mytrim(int max_, String txt_, char align_='l')
{
 String s=txt_;
 //-----add leading blanks, if align_ needs them----------------
 int j, z;
 char ca[10];
 int blank_=0;
 int len_;
 if(_cp437) len_=txt_.length();
 else len_=utf8length(txt_);
 if(align_=='c') blank_=(max_-len_)/2;
 if(align_=='r') { if(len_<max_) blank_=max_-len_; }
 //-----add blanks before text----------------------------------
 for(int i=0; i<blank_; i++) s=" "+s;
 if(len_>max_) 
 { //-----text too long, truncate text--------------------------
  s=utf8substring(s,0,max_);
 }
 else //-----text too short, add blank--------------------------
 {
  for(int i=len_; i<max_; i++) s+=" ";
 }
 return s;
}

// *************************************************************
//      font specific methods
// *************************************************************

 //_____get width of text (-1 on error)_________________________
 int Screen096::getFontWidth() {
 if(fontData) return pgm_read_byte(fontData); 
 return -1;
}

 //_____get height of text (-1 on error)________________________
int Screen096::getFontHeight() {
 if(fontData) return pgm_read_byte(fontData + HEIGHT_POS); 
 return -1;
}

//_____set font for title when using methods screen...___________
void Screen096::setFontTitle(uint8_t *pfont1) {fontTitle=pfont1;}

//_____set font for text when using methods screen...___________
void Screen096::setFontText(uint8_t *pfont1) {fontText=pfont1;}

// *************************************************************
//      draw methods
// *************************************************************

//_____clear screen buffer______________________________________
void Screen096::clear()
{
 int x=getWidth();
 int y=getHeight();
 setColor(BLACK);
 fillRect(0,0,x,y);
 setColor(WHITE);
}

//_____draw character array_____________________________________
void Screen096::drawChars(int16_t xMove, int16_t yMove, char *text, int16_t length)
{
 uint16_t lineHeight = pgm_read_byte(fontData + HEIGHT_POS);
 char text_[length+1];
 strncpy(text_, text, length);
 text_[length]=0;  //..ToDo..
 uint16_t yOffset = 0;
 // If the string should be centered vertically too
 // we need to now how high the string is.
 if (textAlignment == TEXT_ALIGN_CENTER_BOTH) {
   uint16_t lb = 0;
   // Find number of linebreaks in text
   for (uint16_t i=0;text_[i] != 0; i++) {
     lb += (text_[i] == 10);
   }
   // Calculate center
   yOffset = (lb * lineHeight) >> 1;
 }
 uint16_t line = 0;
 char* textPart = strtok(text_,"\n");
 while (textPart != NULL) {
  uint16_t length = strlen(textPart);
  drawStringInternal(xMove, yMove - yOffset + (line++) * lineHeight, textPart, length, getStringWidth(textPart, length));
  textPart = strtok(NULL, "\n");
 }
}

//_____write a string using different fonts_____________________
void Screen096::drawString(int16_t x, int16_t y, String strUser)
{
 if(_cp437) drawStringUsingCP437(x,y,strUser);
 else
  SSD1306Wire::drawString(x,y,strUser);
}

//______draw a string (using chars from code page 437)__________
void Screen096::drawStringUsingCP437(int16_t xMove, int16_t yMove, String strUser) {
 drawStringUsingCPxxx(xMove, yMove, strUser, 437);
}

//______draw a string (using chars from code page xxx)__________
void Screen096::drawStringUsingCPxxx(int16_t xMove,int16_t yMove,String strUser,int cp) {
 uint16_t lineHeight = pgm_read_byte(fontData + HEIGHT_POS);
 char text[1+strUser.length()];
 char* ptext=text;
 int len1=0;
 switch(cp)
 {
  case 437: len1=stringToCP437(strUser, ptext); break;
  //case 850: len1=string2cp850(strUser, ptext); break;
  default: return;
 }
 text[len1]=0;
 uint16_t yOffset = 0;
 if (textAlignment == TEXT_ALIGN_CENTER_BOTH) {
  uint16_t lb = 0;
  // Find number of linebreaks in text
  for (uint16_t i=0;text[i] != 0; i++) {
    lb += (text[i] == 10);
  }
  // Calculate center
  yOffset = (lb * lineHeight) >> 1;
 }
 uint16_t line = 0;
 char* textPart = strtok(text,"\n");
 while (textPart != NULL) {
  len1 = strlen(textPart);
  drawStringInternal(xMove, yMove - yOffset + (line++) * lineHeight, textPart, len1, getStringWidth(textPart, len1));
  textPart = strtok(NULL, "\n");
 }
}

// *************************************************************
//      utf8 specific methods
// *************************************************************

//_____number of characters in a UTF-8-string___________________
int Screen096::utf8length(String s1)
{
 int len=s1.length();                  // str length in bytes(!)
 int chars=0;                          // number of characters
 unsigned char byt;                    // help byte
 for(int i=0; i<len; i++)              // for all bytes in string
 {
  byt=s1.charAt(i);                    // 1 byte of given string
  chars++;                             // one more new character
  //.....is the byte a following utf8-byte?.....................
  if(((byt&0x80)==0x80) && ((byt|0xBF)==0xBF)) 
   chars--;                            // yes: do new character
 }
 return chars;                         // # of chars (NOT bytes)
}

//_____substring with len2 chars starting at from1______________
// index from1 starts with 0, len2<=0: until end of string
String Screen096::utf8substring(String source, int from1, int len2)
{
 char ca[20];
 int lenbytes=source.length();         // string length in bytes
 int i1;                               // byte index of source
 int chars=0;                          // number of utf8 chars
 int chars2=0;                         // utf8 chars in result
 unsigned char c1;                     // byte in string
 bool bcopy=false;                     // copy part of string
 String s2((char *)0);                 // result string
 s2.reserve(lenbytes);                 // maximum result length
 s2="";                                // start result
 //-----for all bytes in source string--------------------------
 for(int i1=0; i1<lenbytes; i1++)
 {
  if(chars==from1) bcopy=true;         // number of start char
  c1=source.charAt(i1);                // get byte of source
  if(bcopy)                            // if copy...
  { s2+=String((char)c1); chars2++; }  // add byte to result
  chars++;                             // next char in source
  c1=source.charAt(++i1);              // next byte in source
  //.....consists utf8-character of more than one byte?.........
  while( ((c1&0x80)==0x80) && ((c1|0xBF)==0xBF) ) {
   if(bcopy) s2+=String((char)c1);     // add byte to resulr
   c1=source.charAt(++i1);             // next byte of this char
  }
  --i1;                                // prepare next byte
  //.....number of chars to copy reached?....................... 
  if(len2>0) { if(chars2>=len2) return s2; }
 }
 return s2;                            // return result string
}

//_____number of bytes of utf8-char_____________________________
// return 1..4, 0 on error
int Screen096::utf8NumberOfBytes(uchar utf8[])
{
 uchar c0=utf8[0];
 if(c0==0) return 1;                             // 1x null byte
 if(c0<=0x7F) return 1;                          // 1 byte value
 if((c0&0xF0)==0xF0)
 {
  if((c0|0xF7)==0xF7) return 4;                  // 4 byte value
  else return 0;                                 //11111 = error
 }
 if(((c0&0xE0)==0xE0)&&((c0|0xEF)==0xEF)) return 3;// 3 byte value
 else
 {
  if(((c0&0xC0)==0xC0)&&((c0|0xDF)==0xDF)) return 2;
 }
 return 0;
}

//_____is given byte a first of n bytes_________________________
// return: 1..4=first of .. bytes or 0 if following byte (10xx)
// 0xxxxxxx                             00 to 7F, 1 byte
// 110xxxxx 10xxxxxx                    CX or DX, 2 byte
// 1110xxxx 10xxxxxx 10xxxxxx           EX      , 3 byte
// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  F0 to F7, 4 byte
int Screen096::utf8FirstOfNBytes(uchar c0)
{
 if((c0|0x7F)==0x7F) 
 {
  //if(c0==0x7F) return 3;                    // special char
  return 1;                                 // 1 byte value
 }
 if((c0&0xF0)==0xF0)
 {
  if((c0|0xF7)==0xF7) return 4;             // 4 byte value
  else return 0;                            // 11111xxx = error
 }
 if(((c0&0xE0)==0xE0)&&((c0|0xEF)==0xEF)) return 3;// 3 byte value
 else
 {
  if(((c0&0xC0)==0xC0)&&((c0|0xDF)==0xDF)) return 2;
 }
 return 0;
}

// *************************************************************
//     utf8 <-> cp437 methods
// *************************************************************

//_____convert a hole utf8-string to cp437 array________________
// ptext should point to char array with length 1+strUser.length()
// return: number of cp437 characters
// Called by drawString1()
// UTF-8:
// 0xxxxxxx                             00 to 7F, 1 byte
// 110xxxxx 10xxxxxx                    CX or DX, 2 byte
// 1110xxxx 10xxxxxx 10xxxxxx           EX      , 3 byte
// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  F0 to F7, 4 byte
int Screen096::stringToCP437(String strUser, char* ptext)
{
 int  len437=0;
 int  lenutf8=strUser.length();
 uchar ca1[1+lenutf8];
 for(int i=0; i<lenutf8;i++) ca1[i]=(uchar)strUser.charAt(i);
 ca1[lenutf8]=0;
 len437=aUtf8ToCP437(lenutf8, ca1, (uchar*) ptext);
 return len437;
}

//_____convert utf8 character to code page 437 char (= 1 byte)__
// return: cp850 character (0 on error)
uchar Screen096::utf8ToCP437(uchar utf8[])
{
 int   bytes=0;                   //number of bytes of utf8 char
 uchar c0=utf8[0];                // 1st byte
 bytes=utf8NumberOfBytes(utf8);
if(c0==0x7F) { Serial.print("bytes=");Serial.print(bytes); }
 if(bytes<1) return 0;                      // no valid value
 if(bytes>3) return 0;                      // no cp850 char
 if(bytes==1) return c0;                    // 1 byte value
 //-----search 2 or 3 byte value--------------------------------
 for(int i=0; i<128; i++)
 {
  if(pgm_read_byte(table_cp437_utf8+4*i+1)==c0)
  {
   if(pgm_read_byte(table_cp437_utf8+4*i+2)==utf8[1])
   {
    if(bytes==2) return pgm_read_byte(table_cp437_utf8+4*i);
    if(pgm_read_byte(table_cp437_utf8+4*i+3)==utf8[2])
    {
     if(bytes==3) return pgm_read_byte(table_cp437_utf8+4*i);
    }
   }
  }
 }
 return 0;
}

//_____convert cp437 char to utf8 char (1..3 bytes)_____________
// return: number of bytes of utf8 char (1..3), 0 on error
int Screen096::CP437ToUtf8(uchar cp437, uchar utf8[4])
{
 int i;
 if(cp437<=0x7F)
 {//-----one byte character-------------------------------------
  utf8[0]=cp437;
  utf8[1]=0;
  utf8[2]=0;
  utf8[3]=0;
  return 1;
 }
 //-----more than 1 byte for 1 utf8 character-------------------
 for(i=0; i<128; i++)                  // 80..FF=128 chars
 {
  if(cp437==pgm_read_byte(table_cp437_utf8+4*i))
  {
   utf8[0]=pgm_read_byte(table_cp437_utf8+4*i+1);
   utf8[1]=pgm_read_byte(table_cp437_utf8+4*i+2);
   utf8[2]=pgm_read_byte(table_cp437_utf8+4*i+3);
   utf8[3]=0;
   if(utf8[2]==0) return 2;
   return 3;
  }
 }
 return 0;
}

//_____convert cp437 char to String_____________________________
// return: (utf8-)string for given char
String Screen096::CP437ToString(uchar cp437)
{
 uchar utf8[4];
 int imax=CP437ToUtf8(cp437, utf8);
 String s1((char *)0);
 s1.reserve(imax+1);
 for(int i=0; i<imax; i++)
 {
  s1+=String((char)utf8[i]);
 }
 return s1;
}

//_____convert utf8 array to code page 437 array________________
// return: number of bytes of cp437 array
int Screen096::aUtf8ToCP437(int num, uchar autf8[], uchar acp437[])
{
 int i=0;
 int num437=0;
 int numutf8;
 uchar c0;
 uchar utf8[4];
 for(i=0; i<num; i++)
 {
  c0=autf8[i];
  if(c0==0) return num437;
  numutf8=utf8FirstOfNBytes(c0);
  if(i+numutf8>num) return num437;
  //-----prepare conversion-------------------------------------
  utf8[0]=c0;
  if(numutf8>1) utf8[1]=autf8[++i]; else utf8[1]=0;
  if(numutf8>2) utf8[2]=autf8[++i]; else utf8[2]=0;
  if(numutf8>3) utf8[3]=autf8[++i]; else utf8[3]=0;
  acp437[num437++]=utf8ToCP437(utf8);
 }
 return num437;
}

//_____convert cp850 array to utf8 array________________________
// return: number of bytes of utf8 array
int Screen096::aCP437ToUtf8(int num, uchar acp437[], uchar autf8[])
{
 int numutf8=0;
 int num1=0;
 uchar c0;
 uchar utf8[4];
 int i;
 //-----for all cp850 characters--------------------------------
 for(i=0; i<num; i++)
 {
  c0=acp437[i];
  if(c0==0) return numutf8;
  num1=CP437ToUtf8(acp437[i],utf8);
  if(num1==0) return numutf8;
  autf8[numutf8++]=utf8[0];
  if(num1>1) autf8[numutf8++]=utf8[1];
  if(num1>2) autf8[numutf8++]=utf8[2];
  if(num1>3) autf8[numutf8++]=utf8[3];
 }
 return numutf8;
}

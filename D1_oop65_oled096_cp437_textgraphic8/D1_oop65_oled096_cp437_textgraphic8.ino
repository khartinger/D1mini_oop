//_____D1_oop65_oled096_cp437_textgraphic8____191111-200108_____
// * Just for fun:
//   Show text graphic characters of codepage CP437 on an
//   oled 0,96" (128x64 pixel)
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

//_____SETUP____________________________________________________
void setup()   {
 Serial.begin(115200); Serial.println("\nStart");
 //-----init 0.96" screen---------------------------------------
 display_.begin();
 display_.setFont((uint8_t *) fontKH_cp437_8x6); //char: 7x5
 int  dx=display_.getFontWidth();           // width of 1 char   
 int  dy=display_.getFontHeight();          // height of 1 line
 //-----prepair text graphic lines------------------------------
 char line1_cp437[]={0xC9,0xCD,0xD1,0xCD,0xCB,0xCD,0xBB, ' ',0xDA,0xC4,0xC2,0xC4,0xD2,0xC4,0xBF, ' ',0xDE,0xDB,0xDD,0};
 char line2_cp437[]={0xBA, '1',0xB3, '2',0xBA, '3',0xBA, ' ',0xB3, 'A',0xB3, 'B',0xBA, 'C',0xB3, ' ',0xDE,0xDB,0xDD,0};
 char line3_cp437[]={0xC7,0xC4,0xC5,0xC4,0xD7,0xC4,0xB6, ' ',0xC3,0xC4,0xC5,0xC4,0xD7,0xC4,0xB4, ' ',0xDE,0xDB,0xDD,0};
 char line4_cp437[]={0xCC,0xCD,0xD8,0xCD,0xCE,0xCD,0xB9, ' ',0xC6,0xCD,0xD8,0xCD,0xCE,0xCD,0xB5, ' ',0xDE,0xDB,0xDD,0};
 char line5_cp437[]={0xC8,0xCD,0xCF,0xCD,0xCA,0xCD,0xBC, ' ',0xC0,0xC4,0xC1,0xC4,0xD0,0xC4,0xD9, ' ',0xDE,0xDB,0xDD,0};
 
 char line6_cp437[]={0xD5,0xCD,0xB8, ' ',0xD6,0xC4,0xB7, ' ',0xB0,0xB0,0xB0, ' ',0xB1,0xB1,0xB1, ' ',0xB2,0xB2,0xB2,0};
 char line7_cp437[]={0xB3, 'X',0xB3, ' ',0xBA, 'Y',0xBA, ' ',0xB0,0xB0,0xB0, ' ',0xB1,0xB1,0xB1, ' ',0xB2,0xB2,0xB2,0};
 char line8_cp437[]={0xD4,0xCD,0xBE, ' ',0xD3,0xC4,0xBD, ' ',0xB0,0xB0,0xB0, ' ',0xB1,0xB1,0xB1, ' ',0xB2,0xB2,0xB2,0};
 //-----display text graphic lines-------------------------------
 display_.clear();                         // clear diaplay
 display_.drawChars(0,0*dy,line1_cp437,19);
 display_.drawChars(0,1*dy,line2_cp437,19);
 display_.drawChars(0,2*dy,line3_cp437,19);
 display_.drawChars(0,3*dy,line4_cp437,19);
 display_.drawChars(0,4*dy,line5_cp437,19);
 display_.drawChars(0,5*dy,line6_cp437,19);
 display_.drawChars(0,6*dy,line7_cp437,19);
 display_.drawChars(0,7*dy,line8_cp437,19);
 display_.display();                       // show buffer
}

//_____MAIN LOOP________________________________________________
void loop() {
}

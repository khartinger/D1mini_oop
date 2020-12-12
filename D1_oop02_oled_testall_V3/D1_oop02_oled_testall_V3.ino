//_____D1_oop02_oled_testall_V3.ino___________170411-201212_____
// Show text on different 0.66" OLED shield screen layouts
// using class Screen1
// Screen 1: Codepage 437, except 9E=158=Euro-sign (instead Pts)
// Hardware: (1) WeMos D1 Mini
//           (2) OLED Shield: 0.66" SSD1306, 64x48 pixel, I2C
// Created by Karl Hartinger, April 11, 2017,
// Updates:
// 2018-05-16 Move class files to /src/...
// 2020-12-12 Screen1a instead of Screen1, add display_.begin();
// Released into the public domain.
#include "src/screen1a/D1_class_Screen1a.h"
Screen1 display_(D0);             // OLED_RESET=D0=GPIO16
String X=String((char)178);       // perfect test character

int t1=500, t2=2000, t3=3000;
String s1=X+"Line2 qp"+X+X+"Line3 qp"+X+X+"Line4 qp"+X+X
       +"Line5 qp"+X+X+"Line6 qp"+X;
String s2;

void setup() {
 display_.begin();                     // MUST be called
}

void loop() {
 //-----screen for waiting purposes (e.g. network connection)---
 display_.dotClear("Waiting...");      // clear dot screen
 for(int i=0; i<24; i++)               // do 24x...
 { display_.dot(); delay(60); }        // ...print dot and wait
 display_.dotLine("Half time!");       // print text to line 6
 for(int i=0; i<24; i++)               // do 24x...
 { display_.dot(); delay(60); }        // ...print dot and wait
 display_.dotLine("  Ready");          // print text to line 6
 delay(1000);                          // and wait 1 sec
 //-----screen6 and screen6i------------------------------------
 display_.screen6iClear(1,"Screen6 i",'c');
 display_.screen6(2,s1);
 delay(t3);
 display_.screen6Clear(1,"Screen6 i",'c');
 display_.screen6i(2,s1);
 delay(t3);
 //-----screen5 and screen5i------------------------------------
 display_.screen5iClear(1,"Screen5 i",'c');
 display_.screen5(2,s1);
 delay(t3);
 display_.screen5Clear(1,"Screen5 i",'c');
 display_.screen5i(2,s1);
 delay(t3);
 //-----screen4 and screen4i------------------------------------
 display_.screen4Clear(1,"Screen4",'c');
 display_.screen4(2,s1);
 delay(t3);
 display_.screen4i(1,"Screen4 i",'c');
 display_.screen4i(2,s1);
 delay(t3);
 //-----screen4B and screen4Bi----------------------------------
 display_.screen4BiClear(1, "Screen4B i",'c'); 
 display_.screen4B(2, s1); 
 display_.screen4B(5,"BACK");
 display_.screen4B(6,"OK"); delay(t2);
 display_.screen4BClear(1, "Screen4B",'c'); 
 display_.screen4Bi(2, s1); 
 display_.screen4B(5,"");
 display_.screen4B(6,"OK"); delay(t2);
 display_.screen4B(5,"BACK");
 display_.screen4B(6,"OK"); delay(t2);
 //-----2x screen221--------------------------------------------
 display_.screen221Clear(1,"Demo",'c');          delay(t1);
 display_.screen221(2,"-Line 2-",'c');           delay(t1);
 display_.screen221(3," Demo for screen221",'c');delay(t1);
 display_.screen221(5,"--Line 5--");             delay(t1);
 delay(t2);
 display_.screen221Clear(1,"screen221",'c');
 display_.screen221(2,X+"Line2 qp"+X);
 display_.screen221(3,X+"Line3 qp"+X+X+"Line4 qp"+X);
 display_.screen221(5,X+"Line5 qp"+X);
 delay(t3);
 //-----2x screen112--------------------------------------------
 display_.screen112Clear(1,"Demo",'c');          delay(t1);
 display_.screen112(2,"Line2",'c');              delay(t1);
 display_.screen112(3," Demo for screen112",'c');delay(t1);
 delay(t2);
 display_.screen112Clear(1,"screen112",'c');
 display_.screen112(2,X+"L2"+X,'c');
 display_.screen112(3,X+"Line3 qp"+X+X+"Line4 qp"+X);
 delay(t3);
 //-----2x screen13---------------------------------------------
 display_.screen13Clear(1,"Demo",'c');           delay(t1);
 display_.screen13(2,"-Line 2-",'c');            delay(t1);
 display_.screen13(3," Demo for screen13",'c');  delay(t1);
 delay(t2);
 display_.screen13Clear(1,"scr13",'c');
 display_.screen13(2,s1);
 delay(t3);
 //-----2x screen12---------------------------------------------
 display_.screen12Clear(1,"Demo",'c');           delay(t1);
 display_.screen12(2," Demo for screen12",'c');  delay(t1);
 delay(t2);
 display_.screen12Clear(1,"scr12",'c');
 display_.screen12(2,s1);
 delay(t3);
 //-----screen2--------------------------------------------------
 display_.screen2Clear(1,"Demo",'c');
 display_.screen2(2,"scr2",'c');
 delay(t2);
 display_.screen2Clear(1,"X",'c');
 display_.screen2(2,"scr2",'c');
 delay(t2);
 display_.screen2Clear(1,"scree",'c');
 display_.screen2(2,X+"n 2"+X);
 delay(t2);
 display_.screen2Clear(1,"screenX",'c');
 delay(t2);
 //-----screen1--------------------------------------------------
 display_.screen1Clear("Demo",'c');
 delay(t2);
 display_.screen1Clear("Scr 1",'c');
 delay(t2);
}

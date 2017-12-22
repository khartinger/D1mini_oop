//_____D1_oop02_oled_testall_V2.ino___________170411-171218_____
// Display text on OLED shield using an extended display class
// Hardware: WeMos D1 Mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
#include "D1_class_DisplayKH.h"
DisplayKH display_(D0);           // OLED_RESET=D0=GPIO16
String X=String((char)177);       // perfect test character

int t1=500, t2=2000, t3=3000;
String s1=X+"Line2 qp"+X+X+"Line3 qp"+X+X+"Line4 qp"+X+X+"Line5 qp"+X+X+"Line6 qp"+X;
String s2;

void setup() {
}

void loop() {
 //-----for waiting purposes (e.g. network connection)----------
 display_.dotClear("Waiting...");      // clear dot screen
 for(int i=0; i<13; i++)               // do 13x...
 { display_.dot(); delay(100); }       // ...print dot and wait
 display_.dotLine("Half time!");       // print text to line 6
 for(int i=0; i<13; i++)               // do 13x...
 { display_.dot(); delay(100); }       // ...print dot and wait
 display_.dotLine("  Ready");          // print text to line 6
 delay(1000);                          // and wait 0,5 sec

 //-----2x screen6----------------------------------------------
 display_.screen6Clear(1,"Demo",'c');            delay(t1);
 display_.screen6(2,"-Line 2-",'c');             delay(t1);
 display_.screen6(3,"Demo for  screen6");        delay(t1);
 display_.screen6(5,"--Line 5--");               delay(t1);
 display_.screen6(6,"--Line 6--");               delay(t1);
 delay(t2);
 display_.screen6Clear(1,"screen6",'c');
 display_.screen6(2,s1);
 delay(t3);

 //-----2x screen6i---------------------------------------------
 display_.screen6iClear(1,"Demo",'c');           delay(t1);
 display_.screen6i(2,"-Line 2-",'c');            delay(t1);
 display_.screen6i(3,"Demo for  screen6i");      delay(t1);
 display_.screen6i(5,"--Line 5--");              delay(t1);
 display_.screen6i(6,"--Line 6--");              delay(t1);
 delay(t2);
 display_.screen6iClear(1,"screen6i",'c');
 display_.screen6i(2,s1);
 delay(t3);
 //-----2x screen5----------------------------------------------
 display_.screen5Clear(1,"Demo",'c');            delay(t1);
 display_.screen5(2,"-Line 2-",'c');             delay(t1);
 display_.screen5(3," Demo for screen5",'c');    delay(t1);
 display_.screen5(5,"--Line 5--");               delay(t1);
 delay(t2);
 display_.screen5Clear(1,"screen5",'c');
 display_.screen5(2,s1);
 delay(t3);
 //-----2x screen5i---------------------------------------------
 display_.screen5iClear(1,"Demo",'c');           delay(t1);
 display_.screen5i(2,"-Line 2-",'c');            delay(t1);
 display_.screen5i(3," Demo for screen5i",'c');  delay(t1);
 display_.screen5i(5,"--Line 5--");              delay(t1);
 delay(t2);
 display_.screen5iClear(1,"screen5i",'c');
 display_.screen5i(2,s1);
 delay(t3);

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
 //-----2x screen4----------------------------------------------
 display_.screen4Clear(1,"Demo",'c');            delay(t1);
 display_.screen4(2,"-Line 2-",'c');             delay(t1);
 display_.screen4(3," Demo for screen4",'c');    delay(t1);
 delay(t2);
 display_.screen4Clear(1,"screen4",'c');
 display_.screen4(2,s1);
 delay(t3);
 //-----2x screen4i---------------------------------------------
 display_.screen4iClear(1,"Demo",'c');           delay(t1);
 display_.screen4i(2,"-Line 2-",'c');            delay(t1);
 display_.screen4i(3," Demo for screen4i",'c');  delay(t1);
 delay(t2);
 display_.screen4iClear(1,"screen4i",'c');
 display_.screen4i(2,s1);
 delay(t3);
 //-----2x screen4ii--------------------------------------------
 display_.screen4iiClear(1,"Demo",'c');          delay(t1);
 display_.screen4ii(2,"-Line 2-",'c');           delay(t1);
 display_.screen4ii(3," Demo for screen4ii",'c');delay(t1);
 delay(t2);
 display_.screen4iiClear(1,"screen4ii",'c');
 display_.screen4ii(2,s1);
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
//-----2x screen2-----------------------------------------------
 display_.screen2Clear(1,"Demo",'c');            delay(t1);
 display_.screen2(2,"scr2",'c');                 delay(t1);
 delay(t2);
 display_.screen2Clear(1,"scree",'c');
 display_.screen2(2,X+"n 2"+X);
 delay(t3);


}

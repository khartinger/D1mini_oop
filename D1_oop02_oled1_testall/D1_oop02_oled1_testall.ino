//_____D1_oop02_oled1_testall.ino_____________170411-170411_____
// Display text on OLED shield using an extended display class
// Hardware: WeMos D1 Mini
//           OLED Shield: SSD1306, 64x48 pixel, I2C
//                        SCL GPIO5, SDA GPIO4, OLED_RESET GPIO0
#include "D1_class_Display0KH.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
Display0KH display_;
String X=String((char)177);       // super test char

void setup() {
}

String s1;
void loop() {
 //-----screen6-------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 s1=X+"Line2 qp"+X+X+"Line3 qp"+X+X+"Line4 qp"+X;
 s1+=X+"Line5 qp"+X+X+"Line6*";
 display_.screen6(1,"screen6",'c',true);
 display_.screen6(2,s1);
 delay(3000);
 display_.screen6(2,"");
 display_.screen6(4,X,'r');
 delay(3000);
 //-----screen6i------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 s1=X+"Line2 qp"+X+X+"Line3 qp"+X+X+"Line4 qp"+X;
 s1+=X+"Line5 qp"+X+X+"Line6*";
 display_.screen6i(1,"screen6",'c',true);
 display_.screen6i(2,s1);
 //-----screen5-------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 display_.drawLine(62,0,62,48,WHITE);   // vertical test line
 s1=X+"Line2 qp"+X+X+"Line3 qp"+X+X+"Line4 qp"+X+X+"Line5_qp"+X;
 display_.screen5(1,"screen5",'c',true);
 display_.screen5(2,s1);
 delay(2000);
 display_.screen5(2,"");
 display_.screen5(4,X+" ",'r');
 delay(2000); 
 //-----screen5i------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 display_.drawLine(62,0,62,48,WHITE);   // vertical test line
 s1=X+"Line2 qp"+X+X+"Line3 qp"+X+X+"Line4 qp"+X+X+"Line5_qp"+X;
 display_.screen5i(1,"screen5",'c',true);
 display_.screen5i(2,s1);
 delay(3000);
 //-----screen4-------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 s1=X+"pq_Line2"+X+X+"qp_Line3"+X+X+"qp_Line4"+X+X+"123";
 display_.screen4(1,X+"screen4",'l',true);
 display_.screen4(2,s1);
 delay(3000); 
 //-----screen4i------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 s1=X+"pq_Line2"+X+X+"qp_Line3"+X+X+"qp_Line4"+X+X+"123";
 display_.screen4i(1,X+"screen4",'l',true);
 display_.screen4i(2,s1);
 delay(3000); 
 //-----screen2-------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 s1="Scre2"+X+"OK!"+X;
 display_.screen2(1,s1,'c');
 delay(2000);
 display_.screen2(2,"",'l');
 delay(2000);
 //-----screen221-----------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 display_.drawLine(62,0,62,48,WHITE);   // vertical test line
 display_.screen221(1,"screen221",'c');
 display_.screen221(3,X+"Line3Xqp"+X+X+"gLine4Xg"+X);
 display_.screen221(5,"1234567890");
 display_.screen221(5,X+" q|pZ "+X,'c');
 delay(3000);
 display_.screen221(2,"Zeile_2",'c');
 display_.screen221(4,"l",'l');
 display_.screen221(4,"C",'C');
 display_.screen221(4,"R",'R');
 delay(3000);
 //-----screen112-----------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 display_.drawLine(62,0,62,48,WHITE);   // vertical test line
 display_.screen112(1,X+"screen112",'c');
 display_.screen112(2,X+"OK"+X,'c');
 display_.screen112(3,X+"line3"+X);
 display_.screen112(4,X+"line4"+X);
 delay(3000);
 //-----screen12------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 display_.clearDisplay();
 display_.screen12(1,X+"12"+X);
 display_.screen12(2,X+"screen12"+X+X+"Line3pqp"+X+"GgGg");
 delay(3000);
 //-----screen13------------------------------------------------
 display_.drawLine(1,0,1,48,WHITE);     // vertical test line
 display_.drawLine(62,0,62,48,WHITE);   // vertical test line
 display_.clearDisplay();
 display_.screen13(1,X+"13"+X);
 display_.screen13(2,X+"screen13"+X+X+"Line3_qp"+X+X+"Line4",'L');
 delay(3000);
 display_.screen13(3,"",'r');
 delay(3000);
}


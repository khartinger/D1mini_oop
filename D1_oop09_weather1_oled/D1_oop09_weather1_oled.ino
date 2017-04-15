//_____D1_oop09_weather1_oled.ino_____________170415-170415_____
// Show pressure, temperature, humidity and brightness on oled
// Hardware: WeMos D1 mini
//           AM2322, BH1750 and BMP180 sensor on i2c bus

#include "D1_class_AM2322.h"
#include "D1_class_BH1750.h"
#include "D1_class_BMP180.h"
#include "D1_class_DisplayKH.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
DisplayKH display_;               // display object
AM2322 am2322;                    // temperature T1, humidity H1
BMP180 bmp180;                    // temperature T0, pressure P0
BH1750 bh1750;                    // brightness  B1
String sDeg=String((char)247);    // degree symbol

void setup() {
 display_.clearDisplay();
}

void loop() {
 int status;
 double T1,H1,T0,P0,P1;
 long B_;
 String s1;
 //-----BMP180: temperature and pressure------------------------
 status=bmp180.getValues(T0,P0);
 if(status==0)
 {
  P1=bmp180.sealevel(P0, 355);    // Graz 355m
  s1="P="+String(P1,1)+"hP";
  display_.screen5(1,s1,'l');
  s1="T0="+String(T0,1)+sDeg+"C";
  display_.screen5(2,s1,'l');
 }
 //-----AM2322: temperature and humidity------------------------
 status=am2322.getValues(T1,H1);
 if(status==0)
 {
  s1="T1="+String(T1,1)+sDeg+"C";
  display_.screen5(3,s1,'l');
  s1="H="+String(H1,1)+"%";
  display_.screen5(4,s1,'l');
 } 
 //-----BH1750: brightness--------------------------------------
 B_=bh1750.getBi();
 status=bh1750.getStatus();
 if(status==0)
 {
  s1="B="+String(B_)+" lx";
  display_.screen5(5,s1,'l');
 }
 delay(1000);
}

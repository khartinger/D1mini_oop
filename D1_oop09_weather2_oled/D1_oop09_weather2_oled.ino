//_____D1_oop09_weather2_oled.ino_____________170501-170501_____
// Show pressure, temperature, humidity, brightness and
// wind speed (via PCF8583) on oled
// Hardware: WeMos D1 mini
//           AM2322, BH1750, BMP180 and wind sensor on i2c bus

#include "D1_class_AM2322.h"
#include "D1_class_BH1750.h"
#include "D1_class_BMP180.h"
#include "D1_class_Anemometer1.h"
#include "D1_class_DisplayKH.h"
DisplayKH display_;               // display object
AM2322 am2322;                    // temperature T1, humidity H1
BMP180 bmp180;                    // temperature T0, pressure P0
BH1750 bh1750;                    // brightness  B1
Anemometer1 anemometer;           // anemometer object
String sDeg=String((char)247);    // degree symbol
String proH=String((char)254);    // /h symbol (pro h)

void setup() {
 display_.clearDisplay();
}

void loop() {
 int status;
 double T1,RF,T0,P0,P1,speed;
 long E_;
 String s1;
 //-----BMP180: temperature and pressure------------------------
 status=bmp180.getValues(T0,P0);
 if(status==0)
 {
  P1=bmp180.sealevel(P0, 355);    // Graz 355m
  s1="p="+String(P1,1)+"hP";
  display_.screen6(1,s1,'l');
  s1="t="+String(T0,1)+sDeg+"C";
  display_.screen6(2,s1,'l');
 }
 //-----AM2322: temperature and humidity------------------------
 status=am2322.getValues(T1,RF);
 if(status==0)
 {
  s1="T="+String(T1,1)+sDeg+"C";
  display_.screen6(3,s1,'l');
  s1="R="+String(RF,1)+"%";
  display_.screen6(4,s1,'l');
 } 
 //-----BH1750: brightness--------------------------------------
 E_=bh1750.getBi();
 status=bh1750.getStatus();
 if(status==0)
 {
  s1="E="+String(E_)+" lx";
  display_.screen6(5,s1,'l');
 }
 //-----PCF8583: wind speed-------------------------------------
 speed=anemometer.getSpeed();  // read wind speed
 status=anemometer.getStatus();
 if(status==0)
 {
  s1="v="+String(speed,1)+"km/h";
  display_.screen6(6,s1,'l');
 }
 delay(1000);
}

//_____D1_class_Weathersensor1.h______________170611-170616_____
// D1 mini class, that encapsules the measuring of
//   pressure (p=BMP180), temperature (t=BMP180, T=AM2322),
//   humidity (H=AM2322), brightness (E=BH1750) and
//   wind speed (v via PCF8583 as counter)
// Hardware: WeMos D1 mini
//           i2c sensors: BMP180 (0x77), AM2322 (0x5C),
//                        BH1750 (0x23), and wind sensor (0x50)
#ifndef D1_CLASS_WEATHERSENSOR1_H
#define D1_CLASS_WEATHERSENSOR1_H
#define ALTITUDE   355.0
#ifndef SDEGREE
 #define SDEGREE String((char)247)
#endif

#include "D1_class_AM2322.h"
#include "D1_class_BH1750.h"
#include "D1_class_BMP180.h"
#include "D1_class_Anemometer1.h"
#include "D1_class_DisplayKH.h"

class Weathersensor1 {
 public:
  double p1,T1,T2,RF,speed;
  long   E_;
 protected:
  BMP180 bmp180;                  // pressure p1, temperature T1
  AM2322 am2322;                  // humidity RF, temperature T2 
  BH1750 bh1750;                  // brightness E_
  Anemometer1 anemometer;         // anemometer object
  double altitude;
  int    status[4];
  void   setup(double altitude);
 public:
  Weathersensor1();
  Weathersensor1(double altitude);
  void setAltitude(double altitude);
  void measure();
  String toString(String separator, String unit_);
  String values(String separator_);
  String json();
  void display(DisplayKH display_);
};

//_____constructor______________________________________________
Weathersensor1::Weathersensor1() { setup(ALTITUDE); }

//_____constructor with alitude of measure place________________
Weathersensor1::Weathersensor1(double altitude) 
{ setup(altitude); }

//_____setup objects and values_________________________________
// status:
// 0: success
// 1: data too long to fit in transmit buffer
// 2: received NACK on transmit of address
// 3: received NACK on transmit of data
// 4: other error 
// 5: no bytes received
// 7: wrong chip id
// 8: calibration data error
// 9: 
// 10..  : measuring error
// 128...: setup failed
void Weathersensor1::setup(double altitude)
{
 double p0;
 this->altitude=altitude;
 //-----try to get first values in any case---------------------
 status[0]=bmp180.getValues(T1,p0);    // BMP180: temperature T1 
 p1=bmp180.sealevel(p0, altitude);     //         and pressure
 status[1]=am2322.getValues(T2,RF);    // AM2322: T2, humidity
 E_=bh1750.getBi();                    // BH1750: brightness
 status[2]=bh1750.getStatus();         //         and status
 speed=anemometer.getSpeed();          // PCF8583 wind speed
 status[3]=anemometer.getStatus();     //         and status
}

//_____set alitude of measure place (Graz 355m)_________________
void Weathersensor1::setAltitude(double altitude)
{ this->altitude=altitude; }

//_____measure all values_______________________________________
void Weathersensor1::measure()
{ 
 double tmp1, tmp2;
 long   tmp3;
 //-----BMP180: temperature and pressure------------------------
 status[0]=bmp180.getValues(tmp1,tmp2);
 if(status[0]==0)
 {
  T1=tmp1;
  p1=bmp180.sealevel(tmp2, altitude);
 }
 //-----AM2322: temperature and humidity------------------------
 status[1]=am2322.getValues(tmp1,tmp2);
 if(status[1]==0) { T2=tmp1; RF=tmp2; }
 //-----BH1750: brightness--------------------------------------
 tmp3=bh1750.getBi();
 status[2]=bh1750.getStatus();
 if(status[2]==0) E_=tmp3;
 //-----PCF8583: wind speed-------------------------------------
 tmp1=anemometer.getSpeed();  // read wind speed
 status[3]=anemometer.getStatus();
 if(status[3]==0) speed=tmp1;
}

//_____weather sensor values as string__________________________
String Weathersensor1::toString(String separator_="\n",
                                String unit_="")
{
 String s1="", s2="";
 String sDeg=String((char)176);    // degree symbol or 247
 s1+="p="+String(p1,1)+unit_+"hPa"+separator_;
 s1+="t="+String(T1,1)+unit_+sDeg+"C"+separator_;
 s1+="T="+String(T2,1)+unit_+sDeg+"C"+separator_;
 s1+="R="+String(RF,1)+unit_+"%"+separator_;
 //s2=String("     "+String(E_));
 //s2=s2.substring(s2.length()-5);
 s2=String(E_);
 s1+="E="+s2+unit_+"Lx"+separator_;
 s1+="v="+String(speed,1)+unit_+"km/h";
 return s1;
}

//_____only weather sensor values as string_____________________
String Weathersensor1::values(String separator_="\n") 
{
 String s1="";
 s1+=String(p1,1)+separator_;
 s1+=String(T1,1)+separator_;
 s1+=String(T2,1)+separator_;
 s1+=String(RF,1)+separator_;
 s1+=String(E_)+separator_;
 s1+=String(speed,1);
 return s1;
}

//_____weather sensor values as jason string____________________
String Weathersensor1::json()
{
 String s1="{";
 s1+="\"p\":"+String(p1,1)+",";
 s1+="\"t\":"+String(T1,1)+",";
 s1+="\"T\":"+String(T2,1)+",";
 s1+="\"R\":"+String(RF,1)+",";
 s1+="\"E\":"+String(E_)+",";
 s1+="\"v\":"+String(speed,1)+"}\n";
 return s1;
}

//_____display weather sensor values on oled____________________
void Weathersensor1::display(DisplayKH display_)
{
 String s1;
 s1="p="+String(p1,1)+"hP";
 display_.screen6(1,s1,'l');
 s1="t="+String(T1,1)+SDEGREE+"C";
 display_.screen6(2,s1,'l');
 s1="T="+String(T2,1)+SDEGREE+"C";
 display_.screen6(3,s1,'l');
 s1="R="+String(RF,1)+"%";
 display_.screen6(4,s1,'l');
 s1="E="+String(E_)+" lx";
 display_.screen6(5,s1,'l');
 s1="v="+String(speed,1)+"km/h";
 display_.screen6(6,s1,'l');
}
#endif

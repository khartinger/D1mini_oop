//_____D1_class_Sensor2.cpp___________________181027-200124_____
// D1 mini class to manage two each BME280 and BH1750 sensors.
// Hardware: (1) WeMos D1 mini
//           BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76 and 0x77)
//           BH1750 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x23 and 0x5C)
//
// Created by Karl Hartinger, October 27, 2018.
// Update: 2020-01-24 add getsStatus()
// Released into the public domain.

#include "D1_class_Sensor2.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor______________________________________________
Sensor2::Sensor2() { setup(); }

//_____setup output pin_________________________________________
void Sensor2::setup()
{
 bme280o.setAddress(0x77);
 bh1750o.setAddress(0x5C);
 status=0;
 if(!bme280i.begin())      status=status|1;
 if(bh1750i.getStatus()>0) status=status|2;
 if(!bme280o.begin())      status=status|4;
 if(bh1750o.getStatus()>0) status=status|8;
}

//**************************************************************
//     setter, getter, working methods
//**************************************************************

//_____sensor status____________________________________________
int Sensor2::getStatus() { return status; }

//_____sensor status as string__________________________________
String Sensor2::getsStatus() {
 String s1="";
 if((status|1)>0) s1+="ERROR: BME280 indoor not ready. ";
 if((status|2)>0) s1+="ERROR: BH1750 indoor state #"+String(bh1750i.getStatus())+". ";
 if((status|4)>0) s1+="ERROR: BME280 outdoor not ready. ";
 if((status|8)>0) s1+="ERROR: BH1750 outdoor state #"+String(bh1750i.getStatus())+". ";
 if(s1=="") s1="OK";
 return s1;
}

//_____string for oled |xxx.x ° in | ___________________________
String Sensor2::getsTiOled()
{
 float t=bme280i.getTemperature();
 String s1=f2S(t,5,1);
 //s1+=SENSOR2_DEG+" in ";
 return s1;
}

//_____string for oled |xxx.x ° out| ___________________________
String Sensor2::getsToOled()
{
 float t=bme280o.getTemperature();
 String s1=f2S(t,5,1);
 //s1+=SENSOR2_DEG+" out";
 return s1;
}

//_____sensor values as json____________________________________
String Sensor2::getsJson()
{
 String s1;
 float  ti, hi, pi, ai;
 float  to, ho, po, ao;
 float  bi, bo;
 bme280i.getValues(ti, hi, pi, ai);
 bme280o.getValues(to, ho, po, ao);
 bi=bh1750i.getBrightness();
 bo=bh1750o.getBrightness();
 s1 ="{\"Ta\":"+f2S(to,5,1)+",\"Ha\":"+f2S(ho,5,1);
 s1+=",\"pa\":"+f2S(po,4,0)+",\"La\":"+f2S(bo,5,0);
 s1+=",\"Ti\":"+f2S(ti,5,1)+",\"Hi\":"+f2S(hi,5,1);
 s1+=",\"pi\":"+f2S(pi,4,0)+",\"Li\":"+f2S(bi,5,0)+"}";
 return s1;
}

//_____convert float to String with given decimals_______________
String Sensor2::f2S(float f, int len, int decimals)
{
 char carray[24];                           // convert helper
 String s1="";                              // help string
 if((len<0)||(len>24)||(decimals<0)) return s1;
 dtostrf(f,len,decimals,carray);            // format float l.d
 s1=String(carray);                         // array to string
 if(len<=decimals) s1.trim();               // remove blanks
 return s1;
}

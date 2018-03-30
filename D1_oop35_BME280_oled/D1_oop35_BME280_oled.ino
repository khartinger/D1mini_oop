//_____D1_oop35_BME280_oled.ino_______________180330-180330_____
// Measure temperature, humidity pressure/altitude with a BME280
// every 2 seconds and display values on OLED
// Hardware: WeMos D1 Mini
//           OLED  Shield (SSD1306, 64x48 pixel, I2C-addr 0x3C)
//           BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76)
//           (Dual Base Shield upon request)           
#include "D1_class_Screen1.h"
#include "D1_class_BME280.h"
Screen1 display_;                           // display object
BME280 bme280;                              // sensor object
String sDeg=String((char)248);              // degree symbol


//_____get part of the String s1, devided by sep________________
// num...number of part (starting with 0)
String partof(String s1, String sep, int num)
{
 String s_="";
 if(num<0) return s_;
 int i, iold=0, iadd=sep.length();
 int numsep=0;
 while((i=s1.indexOf(sep,iold))>=0)
 {
  if(num==numsep) { return s1.substring(iold,i); }
  numsep++;
  iold=i+iadd;
 }
 if(num==numsep) s_=s1.substring(iold);
 return s_;
}

void setup() {
 bool status = bme280.begin();  
 if (!status) {
  display_.screen5iClear(1,"ERROR",'c');    // line 1 inverted
  display_.screen5(2,"BME280",'l');         // line 2
  display_.drawLine(1,10,62,10,BLACK);      //for better reading
  display_.screen5(3,"missing :(");         //
  display_.screen5(4,"Check");              //
  display_.screen5(5,"wiring!");            //
  while (1) { yield(); };
 }
 display_.screen5iClear(1,"BME280",'c');    // title inverted
}

void loop() {
 float t,h,p,a;
 //-----get all values at once----------------------------------
 String values=bme280.getsValues("|",1,1,0,1);
 String s1="T="+partof(values, "|", 0)+sDeg+"C";
 display_.screen5(2,s1,'l');                // temperature
 display_.drawLine(1,10,62,10,BLACK);       //for better reading
 s1="H="+partof(values, "|", 1)+"%";
 display_.screen5(3,s1,'l');                // humidity
 s1="p="+partof(values, "|", 2)+" hPa";
 display_.screen5(4,s1,'l');                // pressure
 s1="a="+partof(values, "|", 3)+"m";
 display_.screen5(5,s1,'l');                // altitude
 delay(2000);
}


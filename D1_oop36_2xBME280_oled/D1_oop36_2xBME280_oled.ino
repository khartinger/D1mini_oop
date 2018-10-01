//_____D1_oop36_2xBME280_oled.ino_____________180926-180926_____
// Measure temperature, humidity pressure/altitude with 
// two BME280 every 2 seconds and display values on OLED
// Hardware: WeMos D1 Mini
//           OLED  Shield (SSD1306, 64x48 pixel, I2C-addr 0x3C)
//           2xBME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//           (I2C-address 0x76 and 0x77)
// Display example:   1:-12.8°C
//                    2:-12.6°C
//                    1:67.1%
//                    2:67.8%
//                    1:1003 hPa
//                    2:997 hPa
#include "src/screen1/D1_class_Screen1.h"
#include "src/bme280/D1_class_BME280.h"
Screen1 display_;                           // display object
BME280 bme280_1;                            // sensor object 1
BME280 bme280_2(0x77);                      // sensor object 2
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
 bool status1 = bme280_1.begin();  
 bool status2 = bme280_2.begin();  
 if (!status1 & (!status2)) {
  display_.screen5iClear(1,"ERROR",'c');    // line 1 inverted
  display_.screen5(2,"BME280",'l');         // line 2
  display_.drawLine(1,10,62,10,BLACK);      //for better reading
  display_.screen5(3,"missing :");          //
  display_.screen5(4,"Check");              //
  display_.screen5(5,"wiring!");            //
  while (1) { yield(); };
 }
 display_.screen6Clear(1," ");
}

void loop() {
 //-----get all values at once----------------------------------
 String values1=bme280_1.getsValues("|",1,1,0,1);
 String values2=bme280_2.getsValues("|",1,1,0,1);
 //-----split values1 and show them-----------------------------
 String s1="1:"+partof(values1, "|", 0)+sDeg+"C";
 display_.screen6(1,s1,'l');
 s1="1:"+partof(values1, "|", 1)+"%";
 display_.screen6i(3,s1,'l');
 s1="1:"+partof(values1, "|", 2)+" hPa";
 display_.screen6(5,s1,'l');
 //-----split values2 and show them-----------------------------
 s1="2:"+partof(values2, "|", 0)+sDeg+"C";
 display_.screen6(2,s1,'l');
 s1="2:"+partof(values2, "|", 1)+"%";
 display_.screen6i(4,s1,'l');
 s1="2:"+partof(values2, "|", 2)+" hPa";
 display_.screen6(6,s1,'l');
 delay(2000);
}

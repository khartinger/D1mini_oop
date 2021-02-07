//_____D1_oop35_BME280_oled.ino_______________180330-210205_____
// Measure temperature, humidity pressure/altitude with a BME280
// every 2 seconds and display values on OLED 
// and Serial (115200Bd)
//
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//    (I2C-address 0x76 or 0x77)
// 3. OLED Shield: 0.66", SSD1306, 64x48 pixel, I2C @ 0x3C
//
// Created by Karl Hartinger, March 30, 2018.
// Changes:
// 2021-02-07 new classes BME280 and Screen1
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/screen1b/D1_class_Screen1b.h"
#include "src/bme280a/D1_class_BME280a.h"
Screen1 display_;                      // display object
BME280 bme280;                         // sensor object
String sDeg=String((char)248);         // degree symbol

//_____get part of the String s1, devided by sep________________
// num...number of part (starting with 0)
String partof(String s1, String sep, int num)
{
 String s_="";                         //
 if(num<0) return s_;                  //
 int i, iold=0, iadd=sep.length();     //
 int numsep=0;                         //
 while((i=s1.indexOf(sep,iold))>=0)    //
 {
  if(num==numsep) { return s1.substring(iold,i); }
  numsep++;                            //
  iold=i+iadd;                         //
 }
 if(num==numsep) s_=s1.substring(iold);
 return s_;
}

//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 delay(100);
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 //------start I2C and 0.66" OLED-------------------------------
 display_.begin();                          // start I2C
 display_.screen6iClear(1,"");              // clear display
 //------start sensor-------------------------------------------
 bme280.setAddress(0x76);                   // 0x76 or 0x77
 bool status = bme280.begin(false);         // NO start I2C
 if (!status) {
  bme280.setAddress(0x77);                  // 0x76 or 0x77
  status = bme280.begin(false);             // NO start I2C
  if (!status) {
   Serial.println("Error: "+bme280.getsStatus());
   Serial.println("Could not find BME280 sensor:");
   Serial.println("* check wiring");
   Serial.println("* check whether Wire.begin() has been called");
   Serial.println("* check I2C address (0x76 or 0x77)\nEND!");
  display_.screen6iClear(1,"ERROR",'c');    // line 1 inverted
  display_.screen6(3,"BME280",'l');         // line 2
  display_.screen6(4,"missing:");           //
  display_.screen6(5,"Check");              //
  display_.screen6(6,"wiring!");            //
   while (1) delay(100);
  }
 }
 //------info about the sensor chip, wait a little bit----------
 String sensorname=bme280.getSensorName();  // sensorname
 char cAddress[5];                          // help array
 //sprintf(cAddress,"0x%02X",bme280.getAddress());
 sprintf(cAddress,"%02Xh",bme280.getAddress());
 String sAddress=String(cAddress);
 //......send info to Serial....................................
 Serial.println(sensorname+" found!");
 Serial.println("I2C address: "+sAddress);
 Serial.println("Wait until the first measure has been completed...");
 while(!bme280.measuring()) { delay(100); }
 //......display info on OLED...................................
 display_.screen5iClear(1,sensorname+" "+sAddress,'c');
 //display_.screen6(2,"Addr "+sAddress);
 Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 //------get all values at once---------------------------------
 String values=bme280.getsValues("|",1,1,0,1);
 //------display values-----------------------------------------
 String s1="T="+partof(values, "|", 0);
 display_.screen5(2,s1+sDeg+"C",'l');       // temperature
 Serial.print(s1+"°C, ");
 display_.drawLine(1,10,62,10,BLACK);       //for better reading
 s1="H="+partof(values, "|", 1)+"%";
 display_.screen5(3,s1,'l');                // humidity
 Serial.print(s1+", ");
 s1="p="+partof(values, "|", 2)+" hPa";
 display_.screen5(4,s1,'l');                // pressure
 Serial.print(s1+", ");
 s1="a="+partof(values, "|", 3)+"m";
 display_.screen5(5,s1,'l');                // altitude
 Serial.println(s1);
 delay(2000);
}


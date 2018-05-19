//_____D1_oop05_AM2322_oled.ino_______________170414-180519_____
// Show temperature and humidity on oled using a i2c sensor
// AM2322 ... every second
// Hardware: (1) WeMos D1 mini
//           (2) AM2322 sensor on i2c bus (0x5C 7-bit address)
//           (3) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, April 13, 2017
// Last modified 2018-05-19: Move class files to /src/...
// Released into the public domain.
#include "src/am2322/D1_class_AM2322.h"
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
AM2322 am2322;                    // temperature, humidity
Screen1 display_;                 // display object
String sDeg=String((char)248);    // degree symbol

void setup() {
 display_.screen4iClear(1,"Temperatur",'c');    // line 1 (inverted)
 display_.screen4i(3,"Humidity",'c');      // line 3
}

void loop() {
 double T,H;
 int status;
 String s1;                                 // help string
 am2322.getValues(T,H);                     // temperature+hum
 status=am2322.getStatus();                 // staus 0=OK
 if(status==0)
 {
  s1=String(T, 1);                          // 1 decimal places
  display_.screen4(2, s1+sDeg+"C", 'c');    // print temperature
  s1=String(H, 1);                          // 1 decimal places
  display_.screen4(4, s1+"%", 'c');         // print humidity
 }
 delay(1000);
}

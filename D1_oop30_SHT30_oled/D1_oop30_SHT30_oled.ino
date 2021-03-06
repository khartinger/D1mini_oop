//_____D1_oop30_SHT30_oled.ino________________180210-180210_____
// Measure temperature and humidity with a SHT30 and show it
// on OLED every second
// Hardware: (1) WeMos D1 Mini
//           (2) SHT30 Shield (uses I2C bus)
//           (3) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
//           (Dual Base Shield by request)
// Created by Karl Hartinger, February 10, 2018
// Last modified 2018-05-18: Move class files to /src/...
// Released into the public domain.
#include "src/screen1/D1_class_Screen1.h"
#include "src/sht30/D1_class_SHT30.h"

Screen1 display_;                           // display object
SHT30 sht30;                                // default 0x45
String sDeg=String((char)248);              // degree symbol

void setup() {
 display_.screen4i(1,"Temperatur");         // line 1 inverted
 display_.screen4i(3,"Feuchtigk.",'c');     // line 3 inverted
 sht30.begin(); 
}

void loop() {
 float T,H;                                 // temperature+hum
 int status;                                // sensor state
 String s1;                                 // help string
 sht30.getValues(T,H);                      // temperature+hum
 status=sht30.getStatus();                  // staus 0=OK
 if(status==SHT30_OK)
 {
  s1=String(T, 1);                          // 1 decimal places
  display_.screen4(2, s1+sDeg+"C", 'c');    // print temperature
  s1=String(H, 1);                          // 1 decimal places
  display_.screen4(4, s1+"%", 'c');         // print humidity
 }
 delay(1000);
}



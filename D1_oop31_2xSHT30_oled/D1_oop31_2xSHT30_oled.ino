//_____D1_oop30_2xSHT30_oled.ino______________180210-180210_____
// Measure temperature and humidity with two SHT30 and 
// show values on OLED
// Shows the temperature difference, when SHT30 shield is
// mounted directly above D1mini or away from it.
// Hardware: WeMos D1 Mini
//           2x SHT30 Shield (I2C addresses 0x44 and 0x45)
//           OLED  Shield (SSD1306, 64x48 pixel, I2C)
//           (Dual Base Shield by request)
#include "D1_class_Screen1.h"
#include "D1_class_SHT30.h"

Screen1 display_;                           // display object
SHT30 sht30_45;                             // default 0x45
SHT30 sht30_44(0x44);                       // address 0x44
String sDeg=String((char)248);              // degree symbol

void setup() {
 display_.screen5i(1,"SHT30 test");         // line 1 inverted
 sht30_44.begin();
 sht30_45.begin();
}

void loop() {
 float T44,H44,T45,H45;                     // temperature+hum
 int status;                                // sensor state
 String s1;                                 // help string
 sht30_44.getValues(T44,H44);               // temperature+hum
 sht30_45.getValues(T45,H45);               // temperature+hum
 //-----display values of SHT30 with address 0x44_______________
 status=sht30_44.getStatus();               // status 0=OK
 if(status==SHT30_OK)
 {
  s1=String(T44, 1);                        // 1 decimal places
  display_.screen5(2, "T44:"+s1+sDeg, 'l'); // print temperature
  s1=String(H44, 1);                        // 1 decimal places
  display_.screen5(4, "H44:"+s1+"%", 'l');  // print humidity
 }
 else
 {
  display_.screen5(2, "T44: ?");
  display_.screen5(4, "H44: ?");
 }
 //-----display values of SHT30 with address 0x45_______________
 status=sht30_45.getStatus();               // status 0=OK
 if(status==SHT30_OK)
 {
  s1=String(T45, 1);                        // 1 decimal places
  display_.screen5(3, "T45:"+s1+sDeg, 'l'); // print temperature
  s1=String(H45, 1);                        // 1 decimal places
  display_.screen5(5, "H45:"+s1+"%", 'l');  // print humidity
 }
 else
 {
  display_.screen5(3, "T45: ?");
  display_.screen5(5, "H45: ?");
 }
 delay(1000);
}



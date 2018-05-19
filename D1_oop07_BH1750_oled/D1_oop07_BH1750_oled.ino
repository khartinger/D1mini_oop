//_____D1_oop06_BH1750_oled.ino_______________170414-180519_____
// Send value of i2c light sensor BH1750 to oled display 
// every 0,5s
// Hardware: (1) WeMos D1 mini
//           (2) BH1750 light sensor on i2c bus (0x23, 0x5C)
//           (3) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, April 14, 2017
// Last modified 2018-05-19: Move class files to /src/...
// Released into the public domain.
#include "src/bh1750/D1_class_BH1750.h"
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
Screen1 display_;                 // display object
BH1750 bh1750;

void setup() {
 display_.screen112(1,"Helligkeit");
 display_.screen112(3,"Lux");
 display_.screen112(4,"HartingerK");
}

void loop() {
 String s1;
 unsigned long lux_=bh1750.getBi();
 if(lux_<100000L) s1=String(bh1750.getBi());
            else s1=">100k";
 display_.screen112(2,s1);
 delay(500);
}

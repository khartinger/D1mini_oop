//_____D1_oop37_2xBH1750_oled.ino_____________181001-181001_____
// Send value of two i2c light sensor BH1750 to oled display 
// every 0,5s
// Hardware: (1) WeMos D1 mini
//           (2) 2x BH1750 light sensor on i2c bus (0x23, 0x5C)
//           (3) OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//               SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, October 01, 2018
// Released into the public domain.
#include "src/bh1750/D1_class_BH1750.h"
#include "src/screen1/D1_class_Screen1.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
Screen1 display_;                 // display object
BH1750 bh1750a;                   // i2c address 0x23
BH1750 bh1750b(0x5C);             // i2c address 0x5C

void setup() {
 display_.screen5iClear(1,"2x BH1750",'c');
 display_.screen5(2,"---0x23---");
 display_.screen5(4,"---0x5C---");
}

void loop() {
 String s1;
 unsigned long lux_a=bh1750a.getBi();
 if(lux_a<100000L) s1=String(bh1750a.getBi());
              else s1=">100k";
 display_.screen5(3,s1+" lux");
 unsigned long lux_b=bh1750b.getBi();
 if(lux_b<100000L) s1=String(bh1750b.getBi());
              else s1=">100k";
 display_.screen5(5,s1+" lux");
 delay(500);
}

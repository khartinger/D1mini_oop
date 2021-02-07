//_____D1_oop06_BH1750_oled.ino_______________170414-210207_____
// Send value of i2c light sensor BH1750 to oled display 
// every 0,5s
// Hardware:
// 1. WeMos D1 mini
// 2. BH1750 light sensor on i2c bus (0x23, 0x5C)
// 3.3 OLED Shield: SSD1306, 64x48 pixel, I2C (0x3C)
//     SCL=D1=DGPIO5, SDA=D2=GPIO4, OLED_RESET GPIO0
// Created by Karl Hartinger, April 14, 2017
// Updates:
// 2018-05-19 Move class files to /src/...
// 2021-02-07 Change classes BH1750 and Screen1
// Released into the public domain.
#include "src/bh1750/D1_class_BH1750.h"
#include "src/screen1b/D1_class_Screen1b.h"
Screen1 display_;                 // display object
BH1750 bh1750;                    // brightness

//_______SETUP__________________________________________________
void setup() {
 delay(100);                                // wait a little bit
 //------start display------------------------------------------
 display_.begin();                          // start I2C
 display_.screen112(1,"Brightness");        // title
 display_.screen112(4,"KHartinger");        // author
 //------start sensor (check both i2c addresses)----------------
 bh1750.setAddress(0x23);                   // set i2c address
 if(!bh1750.begin()) {                      // start I2C
  bh1750.setAddress(0x5C);                  // set i2c address 2
  if(!bh1750.begin(false)) {                // NO start I2C
   display_.screen112(2,"NO IC");           // error message
   display_.screen112(3,"ERROR  END");      // end message
   while(true) delay(500);                  // endless loop
  }
 }
 //------display i2c address------------------------------------
 String s1=String(bh1750.getAddress(),16);
 display_.screen112(3,"Lux   0x"+s1);       // unit, i2c address
}

//_______LOOP___________________________________________________
void loop() {
 String s1;                                 // help string
 unsigned long lux_=bh1750.getBi();         // brightness value
 if(lux_<100000L) s1=String(bh1750.getBi());// value to string
            else s1=">100k";                // too large
 display_.screen112(2,s1);                  // display value
 delay(500);                                // wait 0.5s
}
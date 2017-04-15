//_____D1_oop06_BH1750_oled.ino_______________170414-170414_____
// Send value of digital light sensor to oled display
// Hardware: WeMos D1 mini
//           BH1750 light sensor on i2c bus
//           OLED Shield: SSD1306, 64x48 pixel, I2C
#include "D1_class_BH1750.h"
#include "D1_class_DisplayKH.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
DisplayKH display_;               // display object
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
 delay(250);
}

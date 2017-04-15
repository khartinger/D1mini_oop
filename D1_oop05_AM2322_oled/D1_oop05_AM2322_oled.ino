//_____D1_oop05_AM2322_oled.ino_______________170414-170414_____
// Show temperature and humidity on oled
// Hardware: WeMos D1 mini
//           AM2322 sensor on i2c bus
//           OLED Shield: SSD1306, 64x48 pixel, I2C
#include "D1_class_AM2322.h"
#include "D1_class_DisplayKH.h"
#define OLED_RESET 0              // OLED_RESET=D3=GPIO0
DisplayKH display_;               // display object
AM2322 am2322;                    // temperature, humidity
String sDeg=String((char)247);              // degree symbol

void setup() {
 display_.screen4ii(1,"Temperatur",'c');    // line 1 (inverted)
 display_.screen4ii(3,"Humidity",'c');      // line 3
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
  display_.screen4ii(2, s1+sDeg+"C", 'c');  // print temperature
  s1=String(H, 1);                          // 1 decimal places
  display_.screen4ii(4, s1+"%", 'c');       // print humidity
 }
 delay(1000);
}

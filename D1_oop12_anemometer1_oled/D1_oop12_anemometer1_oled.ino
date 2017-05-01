//_____D1_oop12_Anemometer1_oled.ino________170430-170430_____
// D1 mini class for an anemometer that outputs impulses.
// A PCF8583 counts these impulses, calculates wind speed
//  (in km/h) and writess value to Serial (9600Bd).
// Default time between 2 measurements is 2s
// Default i2c address for PCF8583 is 0x50 (other option 0x51).
// Hardware: WeMos D1 mini
//   anemometer @ PCF8583 @ i2c bus
#include "D1_class_Anemometer1.h"
#include "D1_class_DisplayKH.h"
DisplayKH display_;                    // display object
Anemometer1 anemometer;                // anemometer object

void setup() {
 //-----setup serial--------------------------------------------
 Serial.begin(9600);Serial.println("");// init serial comm
 //-----setup display-------------------------------------------
 display_.clearDisplay();
 display_.screen4(1,"Wind speed");
 display_.screen4(3,"Average",'l');
}

void loop() {
 String s1;
 double speed_=anemometer.getSpeed();  // read wind speed
 double speedA_=anemometer.getSpeedAverage();
 //-----output to oled------------------------------------------
 s1=String(speed_,1)+"km/h";
 display_.screen4(2,s1,'c');
 s1=String(speedA_,1)+"km/h";
 display_.screen4(4,s1,'c');
//-----output to serial----------------------------------------- 
 Serial.print("Wind speed: ");         // write speed
 Serial.print(speed_,1);               // to Serial
 Serial.print(" km/h, Average: ");     // write average
 Serial.print(speedA_,1);              // to Serial
 Serial.println(" km/h");              //
 delay(2000);
}

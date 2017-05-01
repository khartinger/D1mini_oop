//_____D1_oop12_Anemometer1_serial.ino________170430-170430_____
// D1 mini class for an anemometer that outputs impulses.
// A PCF8583 counts these impulses, calculates wind speed
//  (in km/h) and writess value to Serial (9600Bd).
// Default time between 2 measurements is 2s
// Default i2c address for PCF8583 is 0x50 (other option 0x51).
// Hardware: WeMos D1 mini
//   anemometer @ PCF8583 @ i2c bus
#include "D1_class_Anemometer1.h"
Anemometer1 anemometer;                // anemometer object

void setup() {
 Serial.begin(9600);Serial.println("");// init serial comm
}

void loop() {
 double speed_=anemometer.getSpeed();  // read wind speed
 double speedA_=anemometer.getSpeedAverage();
 Serial.print("Wind speed: ");         // write speed
 Serial.print(speed_,1);               // to Serial
 Serial.print(" km/h, Average: ");     // write average
 Serial.print(speedA_,1);              // to Serial
 Serial.println(" km/h");              //
 delay(2000);
}

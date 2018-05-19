//_____D1_oop04_AM2322.ino____________________170413-180519_____
// Show temperature and humidity on Serial using a i2c sensor
// AM2322 ... every second
// Value is 10*real value and integer
// Hardware: (1) WeMos D1 mini
//           (2) AM2322 sensor on i2c bus (0x5C 7-bit address)
// Created by Karl Hartinger, April 13, 2017
// Last modified 2018-05-19: Move class files to /src/...
// Released into the public domain.
#include "src/am2322/D1_class_AM2322.h"
AM2322 am2322;                    // temperature, humidity

void setup() {
 Serial.begin(115200);            // init serial comm 115200 bps
 Serial.println();
}

void loop() {
 int T10,H10,status;
 am2322.getValues10(T10, H10);
 status=am2322.getStatus();
 if(status==AM2322_OK)
 {
  Serial.print("Temperatur (*10): "); Serial.print(T10);
  Serial.print(", Feuchtigkeit (*10): "); Serial.println(H10);
 }
 else
 {
  Serial.print("Fehler! Status: "); Serial.println(status);
 }
 delay(1000);
}

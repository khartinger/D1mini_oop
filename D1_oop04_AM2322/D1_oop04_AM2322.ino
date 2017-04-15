//_____D1_oop04_AM2322.ino____________________170413-170415_____
// Show temperature and humidity on Serial
// Hardware: WeMos D1 mini
//           AM2322 sensor on i2c bus
#include "D1_class_AM2322.h"
AM2322 am2322;

void setup() {
 Serial.begin(9600);              // init serial comm 9600 bps
 Serial.println();
}

void loop() {
 int T10,H10,status;
 am2322.getValues10(T10, H10);
 status=am2322.getStatus();
 if(status==0)
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

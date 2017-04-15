//_____D1_oop08_BMP180.ino____________________170415-170415_____
// Show temperature and pressure on Serial
// Hardware: WeMos D1 mini
//           BMP180 sensor on i2c bus
#include "D1_class_BMP180.h"
BMP180 bmp180;

void setup() {
 Serial.begin(9600);              // init serial comm 9600 bps
 Serial.println();
}

void loop() {
 int status;
 double T,P,P355,A;
 status=bmp180.getValues(T,P);
 if(status==0)
 {
  P355=bmp180.sealevel(P, 355);   // Graz 355m
  A=bmp180.altitude(P, P355);     // altitude to P
  Serial.print("Temperatur: "); Serial.print(T,1);
  Serial.print(" C, Luftdruck: "); Serial.print(P,1);
  Serial.print(" hPa, P355: "); Serial.print(P355,1);
  Serial.print(" hPa, H="); Serial.print(A,1);
  Serial.println(" m");
 }
 else
 {
  Serial.print("Fehler! Status: "); Serial.println(status);
 }
 delay(1000);
}

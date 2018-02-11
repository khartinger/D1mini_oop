//_____D1_oop29_SHT30.ino_____________________180130-180130_____
// Measure temperature and humidity with a SHT30 and send it
// to Serial
// Hardware: WeMos D1 Mini
//           SHT30 Shield
#include "D1_class_SHT30.h"
SHT30 sht30;                                // default 0x45
String sDeg=String(char('°'));              // degree symbol

void setup() {
 Serial.begin(115200);
 Serial.println();
 sht30.begin(); 
}

void loop() {
 float T,H;
 int status;
 String s1;
 sht30.getValues(T,H);
 status=sht30.getStatus();
 if(status==SHT30_OK)
 {
  s1=String(T,1);
  Serial.print("Temperatur: "+s1+sDeg+"C");
  s1=String(H,1);
  Serial.println(", Feuchtigkeit: "+s1+"%");
 }
 else
 {
  Serial.print("Error "); Serial.print(status);
  Serial.print(": "); Serial.println(sht30.getsStatus());
 }
 delay(1000);
}

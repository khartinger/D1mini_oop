//_____D1_oop06_BH1750.ino____________________170414-170414_____
// Send value of digital light sensor to Serial
// Hardware: WeMos D1 mini
//           BH1750 light sensor on i2c bus
#include "D1_class_BH1750.h"
BH1750 bh1750;                    // brightness

void setup() {
 Serial.begin(9600);              // init serial comm 9600 bps
 Serial.println();
}

void loop() {
 int status=bh1750.getStatus();
 if(status==0)
 {
  String s1=String(bh1750.getBi());
  Serial.println("Helligkeit: "+s1+" lx");
 }
 else
 { Serial.print("Status: "); Serial.println(status); }
 delay(200);
}

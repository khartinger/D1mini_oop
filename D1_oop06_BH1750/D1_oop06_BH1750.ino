//_____D1_oop06_BH1750.ino____________________170414-180519_____
// Send value of i2c light sensor BH1750 to Serial every 0,5s
// Hardware: (1) WeMos D1 mini
//           (2) BH1750 light sensor on i2c bus (0x23, 0x5C)
// Created by Karl Hartinger, April 14, 2017
// Last modified 2018-05-19: Move class files to /src/...
// Released into the public domain.
#include "src/bh1750/D1_class_BH1750.h"
BH1750 bh1750;                    // brightness

void setup() {
 Serial.begin(115200);            // init serial comm 115200 bps
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
 delay(500);
}

//_____D1_oop25_Ain_demo.ino__________________171129-180518_____
// Demo program for class Ain:
// * set reference points
// * print reference points
// * read an analog value every second
// * show value as analog value, String and float
// Status and results are printed to Serial (115200 Baud).
// Hardware: (1) WeMos D1 mini
//           (2) Trimmer 10kOhm
// Created by Karl Hartinger, November 29, 2017
// Last modified 2018-05-18: Move class files to /src/...
// Released into the public domain.
#include "src/ain/D1_class_ain.h"
Ain ain;                                    // object

void setup() {
 Serial.begin(115200);Serial.println();     // init serial comm
 ain.setDecimals(3);                        // 3 decimal places
 ain.setRefPoints(0, 0.0, 997, 3.29);       // new ref values
 Serial.println("Reference points set to "+ain.getRefPoints());
}

void loop() {
 int    iAin=ain.getAin1023();              // 0..1023 (7..995)
 String sAin=ain.getLastsVoltage(4);        // 4 decimal places
 float  fAin=ain.getLastVoltage();          // float value
 Serial.print(iAin); Serial.print(" = ");   // 0..1023 (7..995)
 Serial.print(fAin); Serial.print(" = ");   // default 2 decim
 Serial.println(sAin+" V");                 //
 delay(1000);                               // wait
}

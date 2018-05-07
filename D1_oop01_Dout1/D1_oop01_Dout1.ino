//_____D1_oop01_Dout1.ino_____________________171203-180507_____
// Function: Demonstrate methods of class Dout:
// * on start: turn blue led on 2 sec, off 2 sec
// * loop: toggle led every 0,5s and print duration every 0.1s 
//   on Serial (115200 Baud).
//
// Hardware: (1) WeMos D1 mini
//
// Created by Karl Hartinger, December 03, 2017.
// Last modified 2018-05-07: Move class files to /src/...
// Released into the public domain.

#include "src/dout/D1_class_Dout.h"
#define BAUDRATE             115200    // for Serial
Dout led_blue(true);                   // blue LED (default D4)

void setup() {
 //-----begin Serial, read any existing Serial data-------------
 if(!Serial) Serial.begin(BAUDRATE);   // init serial comm
 while(!Serial) {;};                   // wait for Serial
 do{delay(10);} while(Serial.available() && Serial.read()>=0);
 Serial.println();                     // new line
 //-----start led output---------------------------------------- 
 led_blue.on();                        // turn blue LED on
 Serial.println("Blue LED on");        // note it on Serial
 delay(1000);                          // wait 1 sec
 led_blue.set(0);                      // blue LED further on(!)
 delay(1000);                          // wait 1 sec
 led_blue.off();                       // turn blue LED off
 Serial.println("Blue LED off");       // note it on Serial
 delay(2000);                          // wait 2 sec
}

int zsec=0, zsmax=10;
void loop() {
 //-----toggle led----------------------------------------------
 if((zsec==0)||(zsec==5)) {            // every 0 and 0,5 s
  led_blue.toggle();                   // toggle led
  Serial.print("*T* ");                // print toggle to Serial
 }
 //-----print duration------------------------------------------
 Serial.print(led_blue.getDuration()); // duration of led on/off
 Serial.print(" ");                    // blank between values
 //-----increment 0,1s and wait---------------------------------
 zsec++;                               // increment 0,1s counter
 if(zsec>=zsmax) { zsec=0;  Serial.println(); } // reset counter
 delay(100);
}

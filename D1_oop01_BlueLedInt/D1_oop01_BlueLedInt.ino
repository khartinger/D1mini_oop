//_____D1_oop01_BlueLedInt.ino________________181215-181215_____
// Function: Demonstrate methods of class BlueLedInt:
// * On start: (1) turn blue led on 2 sec, off 1 sec
//             (2) turn blue led slowly on 1x
//             (3) blinking while delay is done
//             (4) stop blinking after 2.5x (led remains on)
//             (5) start blinking again
// * Loop:     blink 0,1s on 0,9s off
// Hardware: (1) WeMos D1 mini
// Note    : Class BlueLedInt uses timer interrupt
// Created by Karl Hartinger, December 15, 2018.
// Changes : -
// Released into the public domain.

#include "src/blueledint/D1_class_BlueLedInt.h"
BlueLedInt bli;                        // blue LED (default D4)

void setup() {
 Serial.begin(115200); Serial.println();
 //-----led on/off----------------------------------------------
 bli.on();                             // blue led on
 delay(2000);                          // wait 2 sec
 bli.off();                            // blue LED off
 delay(1000);                          // wait 1 sec
 //-----turn blue led slowly on 2x------------------------------ 
 for(int i=0; i<=100; i=i+10) {        // turn led slowly on
  bli.brightness(i);                   // 
  delay(500);                          // wait 0.3 sec
 }
 //-----blink---------------------------------------------------
 bli.on();                             // led on
 delay(1000);                          // wait 1s
 bli.blink(800,200);                   // long pulses
 delay(2600);                          // blink while delay
 bli.blinkOff();                       // led remains on
 delay(2000);                          // see led on
 bli.blinkOn();                        // start long pulses again
 delay(2100);                          // 2x long pulses
 bli.off();                            // led off
 delay(1000);                          // for one second
 bli.blink(100,900);                   // start short pulses
}

void loop() {
 yield();
}

//_____D1_oop38_statemachineBlink.ino_________200305-200305_____
// Demo for classes Statemachine and StatemachineBlink:
// * Cycle time 8 seconds (80 states, every state lasts 100ms)
// * Starting with state 1 the blue LED blinks 5 times (= 5sec)
//   0.8s on - 0.2s off - 0.8s on - 0.2s off ... (= long blink)
//
//sec  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |
//on ---- ---- ---- ---- ----
//off    -    -    -    -    ----------------
//#  1                      51
// 
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, March 05, 2020
// Released into the public domain.

#include "src/statemachine/D1_class_Statemachine.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"

#define STATE_MAX               80     // max number of states
#define STATE_DELAY            100     // state delay in ms

#define LED1_PIN                D4     // pin number blue LED
#define LED1_INVERT           true     // 0=LED on, 1 =LED off
#define STATE_LED1_START         1     // 1. state: led on 
#define STATES_LED1_ON           8     // 8x100ms = 0.8s
#define STATES_LED1_OFF          2     // 2x100ms = 0.2s
#define LED1_BLINK_NUM           5     // blink 5 times

Statemachine statemachine(STATE_MAX, STATE_DELAY);
StatemachineBlink led1(LED1_PIN, LED1_INVERT, STATE_LED1_START,
 STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);
//_____setup____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println();    // init Serial
}

void loop() {
 statemachine.loopBegin();                  // state begin
 led1.doBlink(statemachine);                // blink LED
 statemachine.loopEnd();                    // state end
}

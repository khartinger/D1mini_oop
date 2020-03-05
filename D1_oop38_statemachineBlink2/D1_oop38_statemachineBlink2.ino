//_____D1_oop38_statemachineBlink.ino_________200305-200305_____
// Demo for classes Statemachine and StatemachineBlink:
// * Cycle time 12 seconds (201...320 states, every state 100ms)
// * Print every 10th counter value to Serial (115200Bd)
// * Timing of blue LED blinking:
// (1) State 201: start 3x 1sec blinking (=3s, long led on)
//                0.8s on - 0.2s off - 0.8s on - 0.2s off ...
// (2) State 241: start 1x 3 seconds blinking (=3s)
// (X) State 261: stopp blinking (LED was on for 2s, 1s missing)
// (X) State 271: go on blinking => finish 1 second "on" of (2)
// (3) State 291: start endless quick blinking (= +0.1s -0.1s)
// (Y) State 301: stopp quick blinking
// (Y) State 311: go on quick blinking
//
//sec  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 |
//on ---- ---- ----      ----------     -----     - - -      - -
//off    -    -    ------          +++++     ----- - - +++++- - -
//# 201                241       261  271       291  301  311
//    
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, March 05, 2020
// Released into the public domain.

#include "src/statemachine/D1_class_Statemachine.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"

#define STATE_MIN              201     // max number of states
#define STATE_MAX              320     // max number of states
#define STATE_DELAY            100     // state delay in ms

#define LED1_PIN                D4     // pin number blue LED
#define LED1_INVERT           true     // 0=LED on, 1 =LED off
#define LED1_1_STATE_START     201     // led on (3x blink, 1s)
#define LED1_1_STATES_ON         8     // 8x100ms = 0.8s on
#define LED1_1_STATES_OFF        2     // 2x100ms = 0.2s off
#define LED1_1_BLINK_NUM         3     // blink 3x
#define LED1_2_STATE_START     241     // led on (1x blink, 3s)
#define LED1_2_STATES_ON        30     // 30x100ms = 3.0s on
#define LED1_2_STATES_OFF        1     // 1x100ms = 0.1s off
#define LED1_2_BLINK_NUM         1     // blink 1x
#define LED1_3_STATE_START     291     // led on (endless, 0.2s)
#define LED1_3_STATES_ON         1     // 1x100ms = 0.1s on
#define LED1_3_STATES_OFF        1     // 1x100ms = 0.1s on
#define LED1_3_BLINK_NUM        -1     // blink endless
#define LED1_X_STATE_OFF       261     // turn led off
#define LED1_X_STATE_GOON      271     // continue
#define LED1_Y_STATE_OFF       301     // turn led off
#define LED1_Y_STATE_GOON      311     // continue

Statemachine statemachine(STATE_MIN, STATE_MAX, STATE_DELAY);
StatemachineBlink led1(LED1_PIN, LED1_INVERT, LED1_1_STATE_START,
 LED1_1_STATES_ON, LED1_1_STATES_OFF, LED1_1_BLINK_NUM);

//_____setup____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println();    // init Serial
 Serial.println("Start loop");
}

void loop() {
 int state=statemachine.loopBegin();        // state begin
 if(state==STATE_MIN)Serial.println("------------------------");
 if(state%10==1) Serial.print(state);       //print state number
 //-----prepare start 1x 3s blink-------------------------------
 if(state==LED1_2_STATE_START) {
  led1.setParams(state,LED1_2_STATES_ON,LED1_2_STATES_OFF,LED1_2_BLINK_NUM);
  Serial.print(": LED 1x 3s blink started");
 }
 //-----prepare start endless blink-----------------------------
 if(state==LED1_3_STATE_START) {
  led1.setParams(state,LED1_3_STATES_ON,LED1_3_STATES_OFF,LED1_3_BLINK_NUM);
  Serial.print(": LED endless blink started");
 }
 //-----prepare LED off and on #1-------------------------------
 if(state==LED1_X_STATE_OFF) {                // led off...
  led1.off(statemachine);
  Serial.print(": LED off #1 ");
 }
 if(state==LED1_X_STATE_GOON) {               // ...and on again
  if(led1.goOn(statemachine)) Serial.print(": LED go on #1 ");
 }
 //-----prepare LED off and on #2-------------------------------
 if(state==LED1_Y_STATE_OFF) {                // led off...
  led1.off(statemachine);
  Serial.print(": LED off #2 ");
 }
 if(state==LED1_Y_STATE_GOON) {               // ...and on again
  if(led1.goOn(statemachine)) Serial.println(": LED go on #2 "); 
 }
 //-----execute blinking----------------------------------------
 led1.doBlink(statemachine);                // blink LED
 if(state%10==1) Serial.println();          // next line
 statemachine.loopEnd();                    // state end
}

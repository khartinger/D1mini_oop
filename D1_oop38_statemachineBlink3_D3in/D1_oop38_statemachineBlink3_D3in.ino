//_____D1_oop38_statemachineBlink3_D3in.ino___200306-200306_____
// Demo for classes Statemachine and StatemachineBlink:
// * Press button D3 short turns the blue led on
// * Press button D3 long turns the blue led off
// * Press button D3 twice lets the blue led blink quickly
// Hardware: (1) WeMos D1 mini
//           (2) Button shield D3
// Created by Karl Hartinger, March 06, 2020
// Released into the public domain.

#include "src/statemachine/D1_class_StatemachineBlink.h"
#include "src/din/D1_class_Din.h"

#define STATE_MAX               10     // max number of states
#define STATE_DELAY            100     // state delay in ms

#define LED1_PIN                D4     // pin number blue LED
#define LED1_INVERT           true     // 0=LED on, 1 =LED off
#define STATE_LED1_START         1     // 1. state: led on 
#define STATES_LED1_ON           2     // 2x100ms = 0.2s
#define STATES_LED1_OFF          8     // 8x100ms = 0.8s
#define LED1_BLINK_NUM          -1     // blink endless

Statemachine stm(STATE_MAX, STATE_DELAY);
StatemachineBlink led1(LED1_PIN, LED1_INVERT, STATE_LED1_START,
 STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);
Din button;
 
//_____setup____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println();    // init Serial
}

//_____LOOP_____________________________________________________
void loop() {
 int state=stm.loopBegin();                 // state begin
 if(button.isPressed()) led1.off(stm);
 if(button.isClicked()) led1.on(stm);
 if(button.isDoubleClicked()) led1.setParams(state,
  STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);
 led1.doBlink(stm);                         // blink LED
 stm.loopEnd();                             // state end
}

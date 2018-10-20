//_____D1_oop38_statemachine_Serial1.ino______181019-181019_____
// Demo for class Statemachine:
// * Count states from A to B and print state value to Serial
//   (115200Bd!)
// * 
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, October 19, 2018
// Released into the public domain.
#include "src/statemachine/D1_class_Statemachine.h"
#define STATE_MIN                4     // minimum state
#define STATE_MAX                9     // maximum state
#define STATE_DELAY            200     // state delay in ms

Statemachine statemachine(STATE_MIN, STATE_MAX, STATE_DELAY);

//_____setup____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println();    // init Serial
}

void loop() {
 int state=statemachine.loopBegin();        // state begin
/* 
 //-----begin: state machine - change state from 6 to 8---------
 if(state==6) {
  statemachine.setState(statemachine.add(2));
  state=statemachine.getState();
 }
 //-----end: state machine - change state-----------------------
*/
 Serial.println(String(state));
 statemachine.loopEnd();                    // state end
 if(state>=STATE_MAX) Serial.println("---");
}

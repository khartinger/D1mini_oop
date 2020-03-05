//_____D1_oop38_statemachine3.ino_____________200303-200305_____
// Demo for class Statemachine: Add and subtract values 
// from actual state, even if the number range does not fit.
// Results are printed to Serial (115200Bd)
// For better understanding:
// * e.g. state range: 11...14
// 14 11 12 13 14 11 12 13 14 11 12 13 14 11 12 13 14 11 12 13 14 11 12
// -2 -1  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20
//   a (wrong) state -2 means state 14, state -1 means 11, ...
// * e.g. state range: -5...-2
//  -2 -5 -4 -3 -2 -5 -4 -3 -2 -5 -4 -3 -2 -5 -4 -3 -2 -5 -4 -3 -2 -5
// -14-13-12-11-10 -9 -8 -7 -6 -5 -4 -3 -2 -1  0  1  2  3  4  5  6  7
//
// * 
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, March 03, 2020
// Released into the public domain.

#include "src/statemachine/D1_class_Statemachine.h"
#define STATE_MIN               11     // max number of states
#define STATE_MAX               14     // max number of states
#define STATE_DELAY           1000     // state delay in ms

Statemachine stm;                      // default: 1..10, 200ms

void printStatemachineParams()
{
 Serial.print(stm.getStateMin()); Serial.print(" ... ");
 Serial.print(stm.getStateMax()); Serial.print(", delay ");
 Serial.print(stm.getStateDelay()); Serial.print(" ms, state=");
 Serial.println(stm.getState());
}

//_____setup____________________________________________________
void setup() {
 int ret;
 Serial.begin(115200); Serial.println();    // init Serial
 Serial.print("Statemachine default values: ");
 printStatemachineParams();
 //-----set new values for state machine------------------------
 stm.setStateMax(STATE_MAX);                // 1. order...
 stm.setStateMin(STATE_MIN);                // 2. ...important!
 stm.setStateDelay(STATE_DELAY);
 //printStatemachineParams(stm);
 printStatemachineParams();
 //-----test set state------------------------------------------
 stm.setState(-2);
 Serial.print("set (wrong) state -2 => expect 14 ");
 ret=stm.getState();
 if(ret==14) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 stm.setState(20);
 Serial.print("set (wrong) state 20 => expect 12 ");
 ret=stm.getState();
 if(ret==12) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 //-----test add(.., ..) method---------------------------------
 ret=stm.add(20,11);
 Serial.print("add to state 20 (=12) 11 (=3) => 15 expect 11 ");
 if(ret==11) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 ret=stm.add(-3,-5);
 Serial.print("add to state -3 (=13) -5 (=+3) => 16 expect 12 ");
 if(ret==12) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 //-----test diff method----------------------------------------
 stm.setState(11);
 ret=stm.diff(12);
 Serial.print("state=11, diff(12) 11-12=-1 expect 3 ");
 if(ret==3) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 stm.setState(12);
 ret=stm.diff(11);
 Serial.print("state=12, diff(11) 12-11=1 expect 1 ");
 if(ret==1) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 //-----set new values for state machine------------------------
 Serial.println("--------------------------------------------");
 stm.setStateMin(-5);                       // 1. order...
 stm.setStateMax(-2);                       // 2. ...important!
 stm.setStateDelay(2*STATE_DELAY);
 Serial.print("New Statemachine: ");
 printStatemachineParams();
 //-----test add(.., ..) method---------------------------------
 ret=stm.add(-12,7);
 Serial.print("add to state -12 (=-4) 7 (=3) expect -5 ");
 if(ret==-5) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 ret=stm.add(-1,-2);
 Serial.print("add to state -1 (=-5) -2 (=-3) expect -3 ");
 if(ret==-3) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 //-----test diff method----------------------------------------
 stm.setState(-10);
 ret=stm.diff(-1);
 Serial.print("state=-10=-2, diff(-1)=diff(-5) => -2-(-5) expect 3 ");
 if(ret==3) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
 stm.setState(-1);
 ret=stm.diff(-10);
 Serial.print("state=-1=-5, diff(-10)=diff(-2) => -5-(-2)=-3 expect 1 ");
 if(ret==1) Serial.println("OK");
 else { Serial.print("Error: ");Serial.println(ret);}
}

void loop() {
 int state=stm.loopBegin();                 // state begin
 Serial.println(state);                     // print state
 stm.loopEnd();                             // state end
}

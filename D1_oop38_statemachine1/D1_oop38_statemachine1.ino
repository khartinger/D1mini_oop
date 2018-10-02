//_____D1_oop38_statemachine1.ino_____________181002-181002_____
// Demo for class Statemachine:
// * 10 states, every state lasts 0.2s (200ms)
// * Blue LED shines 0.8s on - 0.4s off - 0.2s on - 0.6s off ...
//   (= a long and a short blink)
// * Just for fun: state 3 is too long (delay of 333ms)
// * Just for fun: every 5th state print "state%5=0" on Serial
// * Print state number and duration on Serial (9600Bd!)
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, October 02, 2018
// Released into the public domain.
//#include "src/statemachine/D1_class_Statemachine.h"
#include "D1_class_Statemachine.h"
#define STATE_MAX               10     // max number of states
#define STATE_DELAY            200     // state delay in ms
#define STATE_LED1_ON            1     // 1. state: led on 
#define STATE_LED1_OFF           5     // 0.8s later: led off
#define STATE_LED2_ON            7     // 0.4s later: led on 
#define STATE_LED2_OFF           8     // 0.2s later: led off
Statemachine statemachine(STATE_MAX, STATE_DELAY);
const int pinLED=D4;                   // pin number blue LED

//_____setup____________________________________________________
void setup() {
 Serial.begin(9600); Serial.println();      // init Serial
 pinMode(pinLED, OUTPUT);                   // led = output
}

void loop() {
 int state=statemachine.loopBegin();        // state begin
 Serial.print("state "+String(state)+": ");
 //-----state machine: act one state----------------------------
 if(state%5==0) Serial.print("state%5==0, ");// just for demo
 switch(state)
 {
  case 3: delay(333);                       // state too long...
   Serial.print("too long ;) ");
   break;
  case STATE_LED1_ON:
  case STATE_LED2_ON:
   Serial.print("LED on, ");                // Serial info
   digitalWrite(pinLED, LOW);               // led on
   break;
  case STATE_LED1_OFF:
  case STATE_LED2_OFF:
   Serial.print("LED off, ");               // Serial info
   digitalWrite(pinLED, HIGH);              // led off
   break;
  default: break;
 }
 int duration=statemachine.loopEnd();       // state end
 //-----Serial print duration of loop---------------------------
 Serial.println("duration="+String(duration)+"ms");
 if(state==STATE_MAX)Serial.println("------------------------");
}

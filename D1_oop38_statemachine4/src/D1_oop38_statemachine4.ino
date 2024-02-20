//_____D1_oop38_statemachine4.ino_____________181019-240220_____
// Demo for class Statemachine:
// * 10 states, every state lasts 0.2s (200ms)
// * Blue LED shines 0.8s on - 0.4s off - 0.2s on - 0.6s off ...
//   (= a long and a short blink)
// * LED on/off states are calculated by duration of led times
// * Just for fun: state 3 and state 4 are too long
//                 (each has a delay of 350ms)
// * Just for fun: every 5th state print "state%5=0" on Serial
// * Print state number and duration on Serial (115200 Bd!)
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, October 19, 2018
// Changes:
// 2024-02-20 update state machine (replace delay())
// Released into the public domain.
#include "src/statemachine/D1_class_Statemachine.h"
#define STATE_MAX               11     // max number of states
#define STATE_DELAY            200     // state delay in ms

#define STATE_LED1_START         1     // 1. state: led on 
#define STATES_LED1_ON           4     // 4x200ms = 0.8s
#define STATES_LED1_OFF          2     // 2x200ms = 0.4s
#define STATES_LED2_ON           1     // 1x200ms = 0.2s
#define STATES_LED2_OFF          3     // 3x200ms = 0.6s
Statemachine statemachine(STATE_MAX, STATE_DELAY);
const int pinLED=BUILTIN_LED;          // pin number blue LED

int stateLed1On=STATE_LED1_START;
int stateLed1Off;
int stateLed2On;
int stateLed2Off;

//_____setup____________________________________________________
void setup() {
 Serial.begin(115200); Serial.println();    // init Serial
 pinMode(pinLED, OUTPUT);                   // led = output
}

void loop() {
 int state=statemachine.loopBegin();        // state begin
 Serial.print("state "+String(state)+": ");
 if(state==3 || state==4) {
  delay(350);                               // state too long...
  Serial.print(" | too long ;)");
 } 
 if(state==stateLed1On) {
  stateLed1Off=statemachine.add(STATES_LED1_ON);
  Serial.print(" | LED1 on    ");           // Serial info
  digitalWrite(pinLED, LOW);                // led on
 }
 if(state==stateLed1Off) {
  stateLed2On=statemachine.add(STATES_LED1_OFF);
  Serial.print(" | LED1 off   ");           // Serial info
  digitalWrite(pinLED, HIGH);               // led off
 }
 if(state==stateLed2On) {
  stateLed2Off=statemachine.add(STATES_LED2_ON);
  Serial.print(" | LED2 on    ");           // Serial info
  digitalWrite(pinLED, LOW);                // led on
 }
 if(state==stateLed2Off) {
  stateLed1On=statemachine.add(STATES_LED2_OFF);
  Serial.print(" | LED2 off   ");            // Serial info
  digitalWrite(pinLED, HIGH);                // led off
 }
 if(state%5==0) Serial.print(" | state%5==0");// just for demo
 int duration=statemachine.loopEnd();       // state end
 String sDelayed = String(statemachine.getDelayed());
 //-----Serial print duration of loop---------------------------
 Serial.println(" | duration="+String(duration)+"ms, delayed="+sDelayed);
 if(state==STATE_MAX)Serial.println("------------------------");
}

//_____D1_oop94_IntBlinker1.ino__________________khartinger_____
// This sample program for the D1 mini shows the use of the
// timer interrupt in connection with a class.
// For 4 LEDs (at D5 to D8) objects of the class IntBlinker1
// are created, which blink differently with the help of an 
// interrupt routine:
// D5: 1600ms on,  400ms off
// D6: 1200ms on,  800ms off
// D7:  800ms on, 1200ms off
// D8:  400ms on, 1600ms off
// After 6 seconds there is a pause of 1s.
// To show that the loop method is not blocked, a counter
// value is output there every 0.25s via the serial interface
// (with 115200Bd).
// Hardware: 
// (1) WeMos D1 mini
// (2) 4x LEDs + series resistors 120 Ohm.
// See also:
// http://www.switchdoc.com/2015/10/iot-esp8266-timer-tutorial-arduino-ide/
// https://randomnerdtutorials.com/interrupts-timers-esp8266-arduino-ide-nodemcu/

// Created by Karl Hartinger, May 31,2021.
// Changes:
// 2021-05-31 New
// Released into the public domain.
extern "C" {
#include "user_interface.h"
}
#include <Arduino.h>

class IntBlinker1 {
 //------properties---------------------------------------------
 protected:
  int pinLed_;                              // pin number
  int ledState_;                            // 0=off, 1=on
  bool allowBlink_;                         // allow blink
  int msOn_;                                // time on
  int msOff_;                               // time off
  os_timer_t myTimer;                       // timer object

 //------helper methods-----------------------------------------
 public:
 //______timer interrupt routine________________________________
 void myTimer_ISR(void *pArg)
 {
  os_timer_disarm(&myTimer);                // stopp timer
  if(allowBlink_) 
  {//----blinking: start next phase-----------------------------
   ledState_=1-ledState_;                   // invert led state
   digitalWrite(pinLed_, ledState_);        // set Led
   if(ledState_==1) 
    os_timer_arm(&myTimer, msOn_, false);   // set next on time
   else
    os_timer_arm(&myTimer, msOff_, false);  // set next off time
  }
  else
  {//----no blinking: turn led off------------------------------
   ledState_=0;                             // led off
   digitalWrite(pinLed_, ledState_);        // turn led off
  }
 }

 //------constructor & co---------------------------------------
 public:
  IntBlinker1() {pinLed_=D4; setup(); }
  IntBlinker1(int pin) {pinLed_=pin; setup(); }
 protected:
  
  //_____Setup the properties (called by constructor)___________
  void setup() {
   pinMode(pinLed_, OUTPUT);                // pin is output
   ledState_=0;                             // led is off
   msOn_=100;                               // default: 100ms on
   msOff_=400;                              // default: 400ms off
   allowBlink_=false;                       // no blink on start
  }
 //------setter, getter methods (no hardware access)------------
 public:
  
  //_____Set blink duration_____________________________________
  void setTime(int msOn, int msOff) {msOn_=msOn; msOff_=msOff;}
 
 //------working methods----------------------------------------
 public: 
  //_____Start blinking: start timer____________________________
  void blink() { 
   allowBlink_=true; 
   os_timer_arm(&myTimer, 1, false); 
  }
  
  //_____Stop the blinking after this phase_____________________
  void off() { 
   allowBlink_=false; 
   //----For an immediate stop do this:-------------------------
// os_timer_disarm(&myTimer);               // stopp timer
// ledState_=0;                             // led off
// digitalWrite(pinLed_, ledState_);        // turn led off
  }
  
  //_____set name of external interrupt routine_________________
  void setIntfunction(void (*isr_)(void *pArg)) 
  {
   os_timer_setfn(&myTimer, isr_, NULL);
  }
};

//-------define LED-Objects AND their interrupt routine---------
// The names of the interrupt routines must be different and in
// the body the interrupt routine of the object must be called.
IntBlinker1 d5(D5);
ICACHE_RAM_ATTR void intBlinker_ISR5(void *pArg) { d5.myTimer_ISR(NULL); }

IntBlinker1 d6(D6);
ICACHE_RAM_ATTR void intBlinker_ISR6(void *pArg) { d6.myTimer_ISR(NULL); }

IntBlinker1 d7(D7);
ICACHE_RAM_ATTR void intBlinker_ISR7(void *pArg) { d7.myTimer_ISR(NULL); }

IntBlinker1 d8(D8);
ICACHE_RAM_ATTR void intBlinker_ISR8(void *pArg) { d8.myTimer_ISR(NULL); }

//_______SETUP__________________________________________________
void setup() {
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 d5.setTime(1600, 400);                     // set new durations
 d6.setTime(1200, 800);                     // set new durations
 d7.setTime( 800,1200);                     // set new durations
 d8.setTime( 400,1600);                     // set new durations
 d5.setIntfunction(intBlinker_ISR5);        // set name of ISR
 d6.setIntfunction(intBlinker_ISR6);        // set name of ISR
 d7.setIntfunction(intBlinker_ISR7);        // set name of ISR
 d8.setIntfunction(intBlinker_ISR8);        // set name of ISR
 Serial.println("\nsetup(): --Finish--");   // end of setup
}

int counter=-1;                             // start value

//_______MAIN LOOP______________________________________________
void loop() {
 counter++;                                 // increment counter
 if(counter>999) counter=0;                 // avoid overflow
 Serial.print(counter); Serial.print(" ");  // print counter value
 if(counter%20==0) Serial.println();        // lew line after 20
 if(counter%28==0) 
 {//-----start blinking-----------------------------------------
  d5.blink(); d6.blink(); d7.blink(); d8.blink();
 }
 if(counter%28==20) 
 {//-----stopp blinking-----------------------------------------
  d5.off(); d6.off(); d7.off(); d8.off();
 }
 delay(250);                                // wait 0.25s
}

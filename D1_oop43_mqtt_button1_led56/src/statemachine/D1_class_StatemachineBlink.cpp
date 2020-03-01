//_____D1_class_StatemachineBlink.cpp_________200224-200227_____
// The class StatemachineBlink helps to blink a LED in a state 
// machine. You need to know...
// ... the pin number, where the LED is wired (e.g. D4)
// ... LED is on, if pin voltage is 3V3 or 0V (= inverted)
// ... the state number when LED should start blinking
// ... the duration for LED is on (in states)
// ... the duration for LED is off (in states)
// ... the number of blinks (-1 = endless)
// Created by Karl Hartinger, February 24, 2020.
// Modified 200228: Add setStateStart()
// Released into the public domain.

#include "D1_class_StatemachineBlink.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor 1_____________________________________________
StatemachineBlink::StatemachineBlink() { 
 setup();
 pinMode(pin_, OUTPUT);                // led pin = output
}

//_____constructor 2_____________________________________________
StatemachineBlink::StatemachineBlink(
 int pin, bool invert,
 int stateStart, int statesOn, int statesOff, int blinkNum)
{
 setup();
 pin_=pin;                             // LED pin number
 invert_=invert;                       // LED inverted?
 stateStart_=stateStart;
 statesOn_  =statesOn;
 statesOff_ =statesOff;
 blinkMax_  =blinkNum;
 pinMode(pin_, OUTPUT);                // led pin = output
} 

//_____setup properties_________________________________________
void StatemachineBlink::setup() {
 pin_       =SMB_PIN;                  // LED pin number
 invert_    =SMB_INVERT;               // LED inverted?
 stateStart_=SMB_STATE_START;
 statesOn_  =SMB_STATES_ON;
 statesOff_ =SMB_STATES_OFF;
 blinkMax_  =SMB_BLINK_ENDLESS;
 restart_   =true;
 started_   =false;
 off_       =false;
}

//**************************************************************
//     set/get values
//**************************************************************
//_____set new start state and wait for start state number______
void StatemachineBlink::setStateStart(int stateStart)
{
 stateStart_=stateStart;
 restart_   =true;
 started_   =false;
 off_       =false;
}

//_____set new blink parameter and wait for start state number__
void StatemachineBlink::setParams(int stateStart, int statesOn,
 int statesOff, int blinkNum) {
  statesOn_  =statesOn;
  statesOff_ =statesOff;
  blinkMax_  =blinkNum;
  setStateStart(stateStart);
 }

//**************************************************************
//     working methods
//**************************************************************
//_____check state number, if something is to do________________
// returns the number following blinks
int StatemachineBlink::doBlink(Statemachine stm)
{
 if(off_) return 0;
 int state=stm.getState();
 if(SMB_DEBUG) { Serial.print("state=");Serial.print(state);}
 //-----start blinking?-----------------------------------------
 if(state==stateStart_)
 {
  if(restart_)
  {
   if(SMB_DEBUG){Serial.print(", pin=");Serial.print(pin_);Serial.print(" start blinking");}
   if(invert_) digitalWrite(pin_, LOW); // led on
          else digitalWrite(pin_, HIGH);// led on
   blinkCounter_=blinkMax_;
   nextStateOff_=stm.add(statesOn_);
   if(blinkMax_==SMB_BLINK_ENDLESS) restart_=false;
   started_=true;
  }
 }
 if(!started_) return blinkCounter_;
 //-----turn led off?-------------------------------------------
 if(state==nextStateOff_)
 {
  if(SMB_DEBUG){Serial.print(", pin=");Serial.print(pin_);Serial.print(" led off");}
  if(invert_) digitalWrite(pin_, HIGH);// led off
         else digitalWrite(pin_, LOW); // led off
  if(blinkCounter_>0) blinkCounter_--;
  if((blinkCounter_>0)||(blinkCounter_==SMB_BLINK_ENDLESS))
  nextStateOn_=stm.add(statesOff_);
 }
 //-----turn led on?--------------------------------------------
 if(state==nextStateOn_)
 {
  if(SMB_DEBUG){Serial.print(", pin=");Serial.print(pin_);Serial.print(" led on");}
  if(invert_) digitalWrite(pin_, LOW); // led on
         else digitalWrite(pin_, HIGH);// led on
  nextStateOff_=stm.add(statesOn_);
 }
 if(SMB_DEBUG) Serial.println();
 return blinkCounter_;
}

//_____turn blinking off________________________________________
void StatemachineBlink::off() {
 off_=true; 
 if(invert_) digitalWrite(pin_, HIGH);// led off
        else digitalWrite(pin_, LOW); // led off
}

//_____allow blinking___________________________________________
void StatemachineBlink::on()  {
 off_=false;
 restart_=true;
 started_=false; 
}
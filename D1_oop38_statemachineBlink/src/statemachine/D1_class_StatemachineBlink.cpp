//_____D1_class_StatemachineBlink.cpp_________200224-200224_____

//...

#include "D1_class_StatemachineBlink.h"

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor______________________________________________
StatemachineBlink::StatemachineBlink() { 
 setup();
 pinMode(pin_, OUTPUT);                // led pin = output
}

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
}

//**************************************************************
//     working methods
//**************************************************************
//_____check state number, if something is to do________________
// returns the number following blinks
int StatemachineBlink::doBlink(Statemachine stm)
{
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
  }
 }
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

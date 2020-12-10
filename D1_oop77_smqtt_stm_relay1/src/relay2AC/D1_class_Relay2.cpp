//_____D1_class_Relay2.cpp____________________181026-201206_____
// Class Relay2 can be used to switch a relay.
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
// Created by Karl Hartinger, December 06, 2020.
// Updates:
// 2020-12-06 New
// Released into the public domain.

#include "D1_class_Relay2.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default digital out is D1, not inverted)____
Relay2::Relay2() {
  int pin=RELAY2_PIN_RELAY;
  setup(pin, false); 
}

//_____constructor with relais pin number_______________________
Relay2::Relay2(int pinRelais) 
{ setup(pinRelais, false); }

//_____constructor with relais pin number_______________________
Relay2::Relay2(int pinRelais, bool invert)
{ setup(pinRelais, invert); }

//_____setup output pin_________________________________________
void Relay2::setup(int pin, bool invert)
{
 pinRelay_=pin;                        // pin number (e.g. D7)
 pinMode(pinRelay_, OUTPUT);           // set pin to output
 invert_=invert;                       // true: on-off reversed
 off();                                // sets relay_
 changed_=false;                       // no change until now
}

// *************************************************************
//    setter methods
// *************************************************************

//_____set relay on_____________________________________________
// calls set(): sets relay_, changed_
bool Relay2::on() { return (set(1)); }

//_____set relay off____________________________________________
// calls set(): sets relay_, changed_
bool Relay2::off() { return (set(0)); }

//_____change (toggle) output___________________________________
// calls set(): sets relay_, changed_
bool Relay2::toggle() { return (set(1-relay_)); }

//_____set relay 1=on or 0=off__________________________________
// return true: Relay switching state as desired
bool Relay2::set(int val) 
{
 bool bRet=true;
 if(val != relay_) 
 {//-----relay switch status should be changed------------------ 
  changed_=true;                       // new value -> changed!
 } 
 relay_=val;                           // save new value
 if(invert_) digitalWrite(pinRelay_, 1-relay_);  // set relay
        else digitalWrite(pinRelay_, relay_);    // set relay
 return bRet;
}

//_____set relay pin____________________________________________
// calls set(): sets relay_, changed_
bool Relay2::setPinRelay(int pin) {pinRelay_=pin; return true;}

//_____invert relay output voltage______________________________
bool Relay2::setInvertRelay(bool invert) { invert_=invert; return true; }

// *************************************************************
//    getter methods
// *************************************************************

//_____true: relay is on________________________________________
bool Relay2::isOn() { 
 bool bRet=false;
 if(getSwitchStatus()==1) bRet=true;
 return bRet;
}

//_____true: relay is off_______________________________________
bool Relay2::isOff() {
 bool bRet=false;
 if(getSwitchStatus()==0) bRet=true;
 return bRet;
}

//_____true: min. 1 change since last call______________________
bool Relay2::isChange() {
 bool bRet=changed_;
 changed_=false;
 return(bRet); 
 }

//_____get switch status of relay_______________________________
// 1=relay on, 0= relay off
int Relay2::getSwitchStatus() { return relay_; }
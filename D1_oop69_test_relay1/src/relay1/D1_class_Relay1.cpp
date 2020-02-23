//_____D1_class_Relay1.cpp____________________181026-200103_____
// D1 mini class for switching a relay and measuring
// the current (by Analog In)
// Class can be used with or without measuring the current:
// on, off, toggle...............switch the relay directly
// lampOn, lampOff, lampToggle...measure current, switch relay
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) Current transformer (ASM-010) and selfmade 
//               shield D1_INA122_V2 180924 at Analog In
//
// Uses class Ain (files D1_class_Ain.*)
// Created by Karl Hartinger, November 11, 2018.
// Modified: 2020-01-03 separate relay and lamp function
// Released into the public domain.

#include "D1_class_Relay1.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default digital out is D1, not inverted)____
Relay1::Relay1() {
  int pin=RELAY1_PIN_RELAY;
  setup(pin, RELAY1_MA_MAX, RELAY1_MA_LAMP_ON); 
}

//_____constructor with relais pin number_______________________
Relay1::Relay1(int pinRelais) 
{ setup(pinRelais, RELAY1_MA_MAX, RELAY1_MA_LAMP_ON); }

//_____constructor with relais pin number, current limits_______
Relay1::Relay1(int pinRelais, float imax_mA, float ion_mA)
{ setup(pinRelais, imax_mA, ion_mA); }

//_____setup output pin_________________________________________
void Relay1::setup(int pin, float imax, float ion)
{
 pinRelay_=pin;                        // pin number (e.g. D7)
 pinMode(pinRelay_, OUTPUT);           // set pin to output
 mA_max_=imax;                         // max. current
 ain.setRefPoints(0, 0.0, 1000, imax); // 
 mA_lamp_on_=ion;                      // current limit for on
 relay_=0;                             // relais off
 digitalWrite(pinRelay_, relay_);      // turn relais off
 delay(RELAY1_DELAY_MS);               // wait until relais set
 changed_=false;                       // no change until now
 measuring();                          // set lampstate_ 0 or 1
 set(0);                               // measure, lamp off, lastMillis_
}

//_____measures mA_, lamp_ _____________________________________
void Relay1::measuring()
{
 delay(MEASURE_DELAY_MS);
 mA_=int(ain.getVoltage());
 if(mA_<mA_lamp_on_) lampstate_=0; else lampstate_=1;
}

//**************************************************************
//     setter, getter, working methods
//**************************************************************

//_____set digital output pin for lamp__________________________
bool Relay1::set(int val)
{
 measuring();                          // measures mA_, lamp_
 if(val==lampstate_) return true;      // lamp already on|off
 //-----lamp not 'val': try to change---------------------------
 relay_=1-relay_;                      // toggle relais state
 digitalWrite(pinRelay_, relay_);      // set relais
 delay(RELAY1_DELAY_MS);               // wait until relais set
 measuring();                          // measures mA_, lamp_
 changed_=true;
 if(val==lampstate_) return true;
 return false;
}

//_____set relais pin___________________________________________
bool Relay1::setPinRelay(int pin) {
 pinRelay_=pin;                       // pin number (e.g. D7)
 pinMode(pinRelay_, OUTPUT);          // set pin to output
}

//_____set parameter____________________________________________
void Relay1::setParams(int pinRelais, float imax, float ion)
{ setup(pinRelais, imax, ion); }

//_____set current limit for state lamp is on___________________
void Relay1::setCurrentOn(float mA) { mA_lamp_on_=mA; }

//_____set current limit of current fransformer_________________
void Relay1::setCurrentMax(float mA) {
 mA_max_=mA;                           // max. current
 ain.setRefPoints(0, 0.0, 1000, mA);   // new references
 }

//_____turn digital output physical on (3V3)____________________
// return true=success, false=error
bool Relay1::on(void) { 
 if(relay_==1) return true;            // nothing to do
 changed_=true;                        // change output
 relay_=1;                             // relay state on
 digitalWrite(pinRelay_, relay_);      // set relay
 delay(RELAY1_DELAY_MS);               // wait until relay set
 return true; 
}

//_____turn digital output physical off (0V)____________________
// return true=success, false=error
bool Relay1::off(void) {
 if(relay_==0) return true;            // nothing to do
 changed_=true;                        // change output
 relay_=0;                             // relay state off
 digitalWrite(pinRelay_, relay_);      // set relay
 delay(RELAY1_DELAY_MS);               // wait until relay set
 return true; 
}

//_____toggle digital output____________________________________
// return true=success, false=error
bool Relay1::toggle(void) {
 changed_=true;                        // change output
 relay_=1-relay_;                      // change relay state
 digitalWrite(pinRelay_, relay_);      // set relay
 delay(RELAY1_DELAY_MS);               // wait until relay set
 return true; 
}

//_____turn digital output logical on (can be 3V3 or 0V)________
// return true=success, false=error
bool Relay1::lampOn(void) { return(set(1)); }

//_____turn digital output logical off (can be 0V or 3V3)_______
// return true=success, false=error
bool Relay1::lampOff(void) { return(set(0)); }

//_____toggle digital output____________________________________
// return true=success, false=error
bool Relay1::lampToggle(void) { 
  changed_=true;
  return(set(1-lampstate_)); 
 }

//_____turn digital output logical on (can be 3V3 or 0V)________
// return true=success, false=error
bool Relay1::changed(void) {
 bool changed_old=changed_;
 changed_=false;
 return changed_old;
}

//_____get relay state (0=off, 1=on)____________________________
int Relay1::getRelaystate() { return relay_; }

//_____lamp status 0(=off) | 1(=on)_____________________________
int Relay1::getLampstate() { 
 for(int i=0; i<10; i++)  measuring();
 return(lampstate_); 
}

//_____lamp current [mA]________________________________________
float Relay1::getCurrent() { measuring(); return(mA_); }

//_____limit for lamp current state "on" [mA]___________________
float Relay1::getCurrentOn(){measuring(); return(mA_lamp_on_);}

//_____A/D current limit [mA]___________________________________
float Relay1::getCurrentMax() { measuring(); return(mA_max_); }

//_____string for oled |1: xxx mA | ____________________________
String Relay1::getsStatusOled()
{
 measuring();
 String s1=String(lampstate_);
 s1+=": ";
 s1+=String((int)mA_);
 s1+=" mA          ";
 s1=s1.substring(0,10);
 return s1;
}

//_____get duration of this signal______________________________
unsigned long  Relay1::getDuration(void) 
  { return (millis()- lastMilli_); }

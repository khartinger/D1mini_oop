//_____D1_class_Relay2AC.cpp__________________181026-201104_____
// D1 mini class for switching a relay and measuring
// the current (by Analog In)
// Class can be used with or without measuring the current:
// on, off, toggle ............... switch the relay directly
// lampOn, lampOff, lampToggle ... measure current, switch relay
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) Current transformer (ASM-010) and selfmade 
//               shield D1_INA122_V3 at Analog In
//
// Uses classes Relay2 and CurrentAC !
// (files D1_class_Relay2.* and D1_class_CurrentAC.*)
// Created by Karl Hartinger, December 07, 2020.
// Updates:
// 2020-12-07 New
// 2020-12-16 add lampXXX()
// Released into the public domain.

#include "D1_class_Relay2AC.h"

// *************************************************************
//    constructor & co
// *************************************************************

//_______default constructor (relay pin D1, not inverted)_______
Relay2AC::Relay2AC() {
  int   pinRelay= RELAY2_PIN_RELAY;
  int   board   = RELAY2AC_BOARD_DEFAULT;
  float inom_A  = RELAY2AC_NOMINAL_A;
  float ion_A   = RELAY2AC_ON_A;
  int   pinAin  = RELAY2AC_PIN_AIN;
  setup(pinRelay, board, inom_A, ion_A, pinAin);
}

//_______constructor 2__________________________________________
Relay2AC::Relay2AC(int pinRelay) {
  int   board  = RELAY2AC_BOARD_DEFAULT;
  float inom_A = RELAY2AC_NOMINAL_A;
  float ion_A  = RELAY2AC_ON_A;
  int   pinAin = RELAY2AC_PIN_AIN;
  setup(pinRelay, board, inom_A, ion_A, pinAin);
}

//_______constructor 3__________________________________________
Relay2AC::Relay2AC(int pinRelay, float inom_A, float ion_A) {
  int   board  = RELAY2AC_BOARD_DEFAULT;
  int   pinAin = RELAY2AC_PIN_AIN;
  setup(pinRelay, board, inom_A, ion_A, pinAin);
}

//_______constructor 4__________________________________________
Relay2AC::Relay2AC(int pinRelay, int boardType) {
  float inom_A = RELAY2AC_NOMINAL_A;
  float ion_A  = RELAY2AC_ON_A;
  int   pinAin = RELAY2AC_PIN_AIN;
  setup(pinRelay, boardType, inom_A, ion_A, pinAin);
}

//_______constructor 5__________________________________________
Relay2AC::Relay2AC(int pinRelay, int boardType, float inom_A, 
float ion_A) 
{
  int pinAin = RELAY2AC_PIN_AIN;
  setup(pinRelay, boardType, inom_A, ion_A, pinAin);
}

//_______constructor 6__________________________________________
Relay2AC::Relay2AC(int pinRelay, int boardType, float inom_A, 
float ion_A, int pinAin) 
{
  setup(pinRelay, boardType, inom_A, ion_A, pinAin);
}

//_______set properties to default values_______________________
void Relay2AC::setup(int pinRelay, int boardType, 
 float inom_A, float ion_A, int pinAin)
{
 if(boardType>0 && boardType<3) iBoard_=boardType;
 else iBoard_= RELAY2AC_BOARD_DEFAULT;
 pinRelay_=pinRelay;                   // pin number
 pinMode(pinRelay_, OUTPUT);           // set pin to output
 cac_.setBoardType(boardType);         //
 cac_.setNominalCurrent(inom_A);       //
 cac_.setCurrentOn(ion_A);             //
 cac_.setPinAin(pinAin);               //
 invert_=false;                        // false: 0=0V, 1=3V3
 off();                                // sets relay_
 changed_=false;                       // no change until now
}

// *************************************************************
//    setter methods
// *************************************************************

//_______set relay 1=on or 0=off________________________________
// return true: Relay switching state as desired
bool Relay2AC::set(int val) 
{
 //------command: turn relay off--------------------------------
 if(val==0)
 {
  if(cac_.isOff()) return true;        // already off
  off();                               // turn relay off
  delay(RELAY2AC_DELAY_MS);            // wait until relais set
  if(cac_.isOff()) { changed_=true; return true; }
  return false;
 }
 //------command: turn relay on---------------------------------
 if(cac_.isOn()) return true;          // already on
 on();                                 // turn relay off
 delay(RELAY2AC_DELAY_MS);             // wait until relais set
 if(cac_.isOn()) { changed_=true; return true; }
 return false;
}

//_______set current limit on___________________________________
bool Relay2AC::setCurrentOn(float aOn) {
 return(cac_.setCurrentOn(aOn));
}

//_______set nominal current____________________________________
bool Relay2AC::setNominalCurrent(float inom_A) {
 return(cac_.setNominalCurrent(inom_A)); 
}

//_______try to turn on current_________________________________
// return: true=lamp is on, false=lamp is off
bool Relay2AC::lampOn()
{
 if(getCurrentStatus()==1) return true;
 toggle();
 if(getCurrentStatus()==1) return true;
 return false;
}

//_______try to turn off current________________________________
// return: true=lamp is off, false=lamp is on
bool Relay2AC::lampOff()
{
 if(getCurrentStatus()==0) return true;
 toggle();
 if(getCurrentStatus()==0) return true;
 return false;
}

//_______try to toggle current__________________________________
// return: true=lamp toggled, false=lamp NOT toggled
bool Relay2AC::lampToggle()
{
 if(getCurrentStatus()==0)
 {//-----current is off-----------------------------------------
  toggle();
  if(getCurrentStatus()==1) return true;
  return false;
 }
 //------current is on------------------------------------------
 toggle();
 if(getCurrentStatus()==0) return true;
 return false;
}

// *************************************************************
//    getter methods
// *************************************************************

//_______true: relay is on______________________________________
bool Relay2AC::isOn() {
 if(cac_.isChange()) changed_=true;
 return (cac_.isOn()); 
}

//_______true: relay is off_____________________________________
bool Relay2AC::isOff() {
 if(cac_.isChange()) changed_=true;
 return (cac_.isOn()); 
}

//_______get relay status (0=off, 1=on)_________________________
int Relay2AC::getRelayStatus() { return getSwitchStatus(); }

//_______get current status (0=off, 1=on)_______________________
int Relay2AC::getCurrentStatus() {
 if(cac_.isOn()) return 1;
 return 0;
}

//_______get current____________________________________________
float Relay2AC::getCurrent() { return cac_.getCurrent(); }

//_______get current limit "on"_________________________________
float Relay2AC::getCurrentOn() { return cac_.getCurrentOn(); }

//_______get nominal current____________________________________
float Relay2AC::getNominalCurrent() {
 return cac_.getNominalCurrent(); 
}

// *************************************************************
//     working methods
// *************************************************************

//_______test relay: toggle relay 2x, measure current___________
// return true relay and current measure are ok
bool Relay2AC::isOK()
{
  int currStatus=getCurrentStatus();   // current status 0 or 1
  toggle();                            // toggle relay
  delay(50);                           // wait a little bit
  currStatus+=getCurrentStatus();      // current status 1 or 0
  toggle();                            // old relay state
  return(currStatus==1);               // 1=ok, 0 or 2=error
}
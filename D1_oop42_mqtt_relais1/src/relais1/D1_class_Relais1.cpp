//_____D1_class_Relais1.cpp_____________________181026-181111_____
// D1 mini class for switching a relais and measuring
// the current.
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) D1_INA122_V2 180924 (Selfmade) or Analog in
//               and Current transformer ASM-010
//
// Created by Karl Hartinger, November 11, 2018.
// Released into the public domain.

#include "D1_class_Relais1.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default digital out is D7, not inverted)____
Relais1::Relais1() {
  int pin=RELAIS1_PIN_RELAIS;
  setup(pin, RELAIS1_MA_MAX, RELAIS1_MA_LAMP_ON); 
}

//_____constructor with relais pin number_______________________
Relais1::Relais1(int pinRelais) 
{ setup(pinRelais, RELAIS1_MA_MAX, RELAIS1_MA_LAMP_ON); }

//_____setup output pin_________________________________________
void Relais1::setup(int pin, float imax, float ion)
{
 pinRelais_=pin;                       // pin number (e.g. D7)
 pinMode(pinRelais_, OUTPUT);          // set pin to output
 mA_max_=imax;                         // max. current
 ain.setRefPoints(0, 0.0, 1000, imax); // 
 mA_lamp_on_=ion;                      // current limit for on
 relais_=0;                            // relais off
 digitalWrite(pinRelais_, relais_);    // turn relais off
 set(0);                               // measure, lamp off, lastMillis_
}

//_____measures mA_, lamp_ _____________________________________
void Relais1::measuring()
{
 mA_=int(ain.getVoltage());
 if(mA_<mA_lamp_on_) lampstate_=0; else lampstate_=1;
}

//**************************************************************
//     setter, getter, working methods
//**************************************************************

//_____set digital output pin physical: 0=0V, 1=3V3_____________
bool Relais1::set(int val)
{
 measuring();                          // measures mA_, lamp_
 if(val==lampstate_) return true;      // lamp already on|off
 //-----lamp not 'val': try to change---------------------------
 relais_=1-relais_;                    // toggle relais state
 digitalWrite(pinRelais_, relais_);    // set relais
 delay(RELAIS1_DELAY_MS);              // wait until relais set
 measuring();                          // measures mA_, lamp_
 if(val==lampstate_) return true;      // lamp now val (on|off)
 return false;
}

//_____set relais pin___________________________________________
bool Relais1::setPinRelais(int pin) {
 pinRelais_=pin;                       // pin number (e.g. D7)
 pinMode(pinRelais_, OUTPUT);          // set pin to output
}

//_____set parameter____________________________________________
void Relais1::setParams(int pinRelais, float imax, float ion)
{ setup(pinRelais, imax, ion); }

//_____set current limit for state lamp is on___________________
void Relais1::setCurrentOn(float mA) { mA_lamp_on_=mA; }

//_____turn digital output logical on (can be 3V3 or 0V)________
// return true=success, false=error
bool Relais1::changed(void) {
 int lamp_old=lampstate_;
 measuring();
 if(lampstate_==lamp_old) return false;
 return true; 
}

//_____turn digital output logical on (can be 3V3 or 0V)________
// return true=success, false=error
bool Relais1::on(void) { return(set(1)); }

//_____turn digital output logical off (can be 0V or 3V3)_______
// return true=success, false=error
bool Relais1::off(void) { return(set(0)); }

//_____toggle digital output____________________________________
// return true=success, false=error
bool Relais1::toggle(void) { return(set(1-lampstate_)); }

//_____lamp status 0(=off) | 1(=on)_____________________________
int Relais1::getLampstate() { measuring(); return(lampstate_); }

//_____lamp current [mA]________________________________________
float Relais1::getCurrent() { measuring(); return(mA_); }

//_____limit for lamp current state "on" [mA]___________________
float Relais1::getCurrentOn(){measuring(); return(mA_lamp_on_);}

//_____A/D current limit [mA]___________________________________
float Relais1::getCurrentMax() { measuring(); return(mA_max_); }

//_____string for oled |1: xxx mA | ____________________________
String Relais1::getsStatusOled()
{
 measuring();
 String s1=String(lampstate_)+": "+String((int)mA_)+" mA";
 s1+="          "; s1=s1.substring(0,10);
 return s1;
}

//_____get duration of this signal______________________________
unsigned long  Relais1::getDuration(void) 
  { return (millis()- lastMilli_); }

//_____D1_class_Dout.cpp______________________170402-200414_____
// D1 mini class for digital output, e.g. leds.
// Default: pin D4=GPIO2 (blue led), output not inverted (0->0V)
//          value logical 0 (=0V, because invert=false)
// Hardware: (1) WeMos D1 mini
//
// Created by Karl Hartinger, December 03, 2017.
// Changes:
// * 2017-12-03 devide (old) D1_class_Dout.h into *.h and *.cpp
//   + process invert_ now in on(), off() (instead of set)
//   + add toggle(), getDuration(), Dout(bool)
// * 2020-04-14 add start value to constructor
// * 2021-01-03 #define default values
// Released into the public domain.

#include "D1_class_Dout.h"

// *************************************************************
//       constructor & co
// *************************************************************

//_______constructor (default output pin GPIO2=D4, not inverted)
Dout::Dout(){setup(DEFAULT_PIN, DEFAULT_INVERT, DEFAULT_VALUE);}

//_______constructor with output pin number (=GPIO,not inverted)
Dout::Dout(int num) {setup(num, DEFAULT_INVERT, DEFAULT_VALUE);}

//_______constructor (default outpin GPIO2=D4), set invert______
// example: declare blue led on D1mini: Dout led_blue(true);
Dout::Dout(bool invert){setup(DEFAULT_PIN, invert, DEFAULT_VALUE);}

//_______constructor with pin number and set invert_____________
Dout::Dout(int num, bool invert){setup(num, invert, DEFAULT_VALUE);}

//_______constructor with pin number and start value____________
Dout::Dout(int num, int startvalue)
{ setup(num, DEFAULT_INVERT, startvalue); }

//_______constructor with pin number, set invert, start value___
Dout::Dout(int num, bool invert, int startvalue)
{ setup(num, invert, startvalue); }

//_______setup output pin_______________________________________
void Dout::setup(int num, bool invert, int startvalue)
{
 invert_=invert;                       // 
 doutNum_=num;                         // pin number (e.g. D4)
 pinMode(doutNum_, OUTPUT);            // set pin to output
 if(invert) {
  if(startvalue!=0) startvalue=0; else startvalue=1;
 } else
 {
  if(startvalue!=0) startvalue=1; else startvalue=0;
 }
 val_=1-startvalue;                    // force digitalWrite
 set(startvalue);                      // in set ;)
}

// *************************************************************
//       setter, getter, working methods
// *************************************************************

//_______set digital output pin physical: 0=0V, 1=3V3___________
void Dout::set(int val)
{
 if(val==val_) return;                 // nothing to change
 val_=val;                             // save new value
 lastMilli_=millis();                  // new start
 digitalWrite(doutNum_, val_);         // do output
}

//_______turn digital output logical on (can be 3V3 or 0V)______
void Dout::on(void)  { if(invert_) set(0); else set(1); }

//_______turn digital output logical off (can be 0V or 3V3)_____
void Dout::off(void) { if(invert_) set(1); else set(0); }

//_______toggle digital output__________________________________
void Dout::toggle(void) { set(1-val_); }

//_______get duration of this signal____________________________
unsigned long  Dout::getDuration(void) 
  { return (millis()- lastMilli_); }

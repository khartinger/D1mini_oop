//_____D1_class_Din.cpp_______________________170402-171205_____
// D1 mini class for digital input e.g. a button. 
//    Default is D3 (=GPIO0)
// Changes 2017-12-05:
// + devide (old) D1_class_Din.h into *.h and *.cpp
// + names changed (camel case names instead of underscore)
// + getDuration(), lastMilli_ added
//
// Hardware: (1) WeMos D1 mini
//
// Created by Karl Hartinger, December 05, 2017.
// Released into the public domain.

#include "D1_class_Din.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default button is D3)_______________________
Din::Din() { setup(D3); }

//_____constructor with button number (=GPIO)___________________
Din::Din(int num) { setup(num); }

//_____setup input______________________________________________
void Din::setup(int num)
{
 dinNum_=num;                          // set input number
 pinMode(dinNum_, INPUT);              // set io to input
 lastMilli_=millis();                  // time program start
 lastDuration_=0;                      // no valid value 
 din_=this->get();                     // get first value
 lastEdge_=0;                          // start: no edge so far
}

//**************************************************************
//    setter, getter, working methods
//**************************************************************

//_____get input value__________________________________________
int Din::get(void) {
 int val = digitalRead(dinNum_);       // read input value
 
 if((val==1)&&(din_==0)) {             // changed: rising edge!
  lastEdge_=1;                         // type of change
  lastDuration_= millis()-lastMilli_;  // last duration
  lastMilli_=millis();                 // start new duration
 }
 if((val==0)&&(din_==1)) {             // changed: falling edge!
  lastEdge_=2;                         // type of change
  lastDuration_= millis()-lastMilli_;  // last duration
  lastMilli_=millis();                 // start new duration
 }
 din_=val;                             // save value
 return val;
}

//_____read digital input, return true, if button pressed (0V)__
// checks digital input
bool Din::isPressed(void) {
 if(this->get()==0) return true;
 return false;
}

//_____read digital input, return true if button is pressed_____
// 0=no, 1=rising, 2=falling
// true: falling edge since last read
bool Din::isFallingEdge(void)
{
 this->get();
 if(lastEdge_==2) { lastEdge_=0; return true; }
 return false;
}

//_____read digital input, return true if button is released____
bool Din::isRisingEdge(void)
{
 this->get();
 if(lastEdge_==1) { lastEdge_=0; return true; }
 return false;
}

//_____get last input value without pin read____________________
int Din::getLast(void) { return din_; }

//_____get duration of actual state (0 or 1)____________________
unsigned long  Din::getDuration(void) 
 { return (millis()- lastMilli_); }


//_____get last duration________________________________________
unsigned long Din::getLastDuration(void) 
 { return lastDuration_; }


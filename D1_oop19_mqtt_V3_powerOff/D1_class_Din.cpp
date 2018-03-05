//_____D1_class_Din.cpp_______________________170402-171213_____
// D1 mini class for digital input e.g. a button. 
//    Default is D3 (=GPIO0)
// Changes 2017-12-13:
// + devide (old) D1_class_Din.h into *.h and *.cpp
// + names changed (camel case names instead of underscore)
// + getDuration(), lastMilli_ added
// + button click methods/properties added
//
// Hardware: (1) WeMos D1 mini
//
// Created by Karl Hartinger, December 13, 2017.
// Released into the public domain.

#include "D1_class_Din.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default button is D3)_______________________
Din::Din() { setup(PIN_DIN); }

//_____constructor with button number (=GPIO)___________________
Din::Din(int num) { setup(num); }

//_____setup input______________________________________________
void Din::setup(int num)
{
 dinNum_=num;                          // set input number
 pinMode(dinNum_, INPUT);              // set io to input
 lastMilli_=millis();                  // time program start
 lastDuration_=0;                      // no valid value 
 din_=this->read();                    // get first value
 clickMilli_=CLICK_MS;                 // max. click time
 click1=0;                             // no 1x click
 click2=0;                             // no double click
 clickL=0;                             // no long click
 lastEdge_=0;                          // start: no edge so far
}

//**************************************************************
//    setter, getter methods (no hardware access)
//**************************************************************

//_____set maximal click duration in ms_________________________
void Din::setClickMilli(unsigned long ms_max)
{ clickMilli_ = ms_max; }

//_____get last input value without pin read____________________
int Din::getLast(void) { return din_; }

//_____get duration of actual state (0 or 1)____________________
unsigned long  Din::getDuration(void) 
 { return (millis()- lastMilli_); }

//_____get last duration________________________________________
unsigned long Din::getLastDuration(void) 
 { return lastDuration_; }

//**************************************************************
//    setter, getter, working methods
//**************************************************************

//_____read input value_________________________________________
int Din::read(void) {
 //-----read (new) input value----------------------------------
 int val = digitalRead(dinNum_);       // read input value
 //-----falling edge--------------------------------------------
 if((val==0)&&(din_==1)) {             // changed: falling edge!
  //.....set type of edge, update time (duration)...............
  lastEdge_=2;                         // type of change
  lastDuration_= millis()-lastMilli_;  // last duration edge
  lastMilli_=millis();                 // start new duration
  //...after a short wait: start click, long click, double click
  if(lastDuration_>clickMilli_)
  {
   click1=1;                           // start 1xclick
   clickL=1;                           // start long click
   if(click2!=2) click2=1;             // start 2xclick
  }
  else
  { //.....time too short for new start.........................
   click1=0;                           // no 1x click start
   clickL=0;                           // no long click start
   //.....special: second part of duoble click..................
   if(click2==2)                       // 2. part of 2x click
   { click2=3; click1=0; }             // yes: next state
   else click2=0;                      // no:  no 2xclick
  }
 }
 //-----rising edge---------------------------------------------
 if((val==1)&&(din_==0)) {             // changed: rising edge!
  //.....set type of edge, update time (duration)...............
  lastEdge_=1;                         // type of change
  lastDuration_= millis()-lastMilli_;  // last duration
  lastMilli_=millis();                 // start new duration
  //.....single click...........................................
  if(click1==1)                        // 1xclick: rising edge
  { if(lastDuration_<=clickMilli_)     // rising edge in time?
     click1=2;                         // rising edge ok
    else click1=0;                     // too late, no 1xclick
  }
  //.....1st part of double click...............................
  if(click2==1)                        // 2xclick: 1.rising edge
  { if(lastDuration_<=clickMilli_)     // 1.rising edge in time?
     click2=2;                         // 1.rising edge ok
    else click2=0;                     // too late, no 2xclick
  }
  //.....2nd part of double click...............................
  if(click2==3)                        // 2xclick: 2.rising edge
  { if(lastDuration_<=clickMilli_)     // 2.rising edge in time?
     click2=4;                         // 2.rising edge ok
    else click2=0;                     // too late, no 2x click
  }
  //.....rising edge = no long click...........................
  clickL=0;                            // rising edge, too late
 }
 //-----click1 or click2 finished-------------------------------
 if((val==1)&&(din_==1)) {             // no change, 1
  //.....click1 finished (= no double click)....................
  if(click1==2)                        // 1xclick ready?
  {if(millis()-lastMilli_>=clickMilli_)// check follow-up time
   {click1=3;                          // yes -> single click ok
    click2=0;                          // no double click
   }             
  }
  //.....click2 finished (= no tripple click)...................
  if(click2==4)                        // 2xclick ready?
  {if(millis()-lastMilli_>=clickMilli_)// check follow-up time
   {click2=5;                          // yes -> double click ok
    click1=0;                          // no 1x click
   } 
  }
 }
 //-----remember new button value-------------------------------
 din_=val;                             // save value
 return val;
}

//_____read digital input, return true, if button pressed (0V)__
// checks digital input
bool Din::isPressed(void) {            //
 if(this->read()==0) return true;      // digital input is 0V
 return false;                         // digital input is 3V3
}

//_____return true, if button is pressed longer than ms_min_____
bool Din::isPressed(unsigned long ms_min) {
 if(this->read()!=0) return false;     // button not pressed
 if(clickL<1) return false;            // long click not started
 if(getDuration()<ms_min) return false;// time too short
 clickL=0;                             // long click requested
 return true;                          // yes, long click
}

//_____read digital input, return true if button is pressed_____
// 0=no, 1=rising, 2=falling
// true: falling edge since last read
bool Din::isFallingEdge(void)
{
 this->read();                         // check digital input
 if(lastEdge_!=2) return false;        // no (new) falling edge
 lastEdge_=0;                          // falling edge requested
 return true;                          // yes, it's falling edge
}

//_____read digital input, return true if button is released____
bool Din::isRisingEdge(void)
{
 this->read();                         // read digital input
 if(lastEdge_!=1) return false;        // not rising edge
 lastEdge_=0;                          // rising edge requested
 return true;                          // yes, rising edge
}

//_____is button 1x clicked (duration less than clickMilli_)____
bool Din::isClicked()
{
 this->read();
 if(click1!=3) return false;           // 1xclick not finished
 click1=0;                             // 1xclick requested
 return true;                          // yes, single click
}

//_____is button 2x clicked (duration less than clickMilli_)____
bool Din::isDoubleClicked()
{
 this->read();
 if(click2!=5) return false;           // 2xclick not finished
 click2=0;                             // double click requested
 return true;                          // yes, double click
}


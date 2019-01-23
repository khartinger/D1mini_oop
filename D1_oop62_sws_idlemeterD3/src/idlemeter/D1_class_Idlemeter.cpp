//_____D1_class_Idlemeter.cpp______________190118-190118_____
// The class Idlemeter measures the time between two pulses.
// The highest value within a period is stored.
// Created by Karl Hartinger, January 18, 2019.
// Modified: -
// Released into the public domain.

#include "D1_class_Idlemeter.h"
extern "C" {
#include "user_interface.h"
}

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor______________________________________________
Idlemeter::Idlemeter() { setup(PERIOD_MS); }

Idlemeter::Idlemeter(unsigned long period_ms)
{ setup(period_ms); }

//_____setup properties_________________________________________
void Idlemeter::setup(unsigned long period_ms) {
 period_ms_=period_ms;
 reset();
}

//**************************************************************
//     set/get values
//**************************************************************

//_____reset all idle starttime and duration values_____________
void Idlemeter::reset() {
 last_.msIdle=0;
 last_.msStart=0;
 max1_.msIdle=0;
 max1_.msStart=0; 
 max2_.msIdle=0;
 max2_.msStart=0; 
 isNew_=false;
 starttime_ms_=millis();
}

//_____check end of period______________________________________
// return: true=new counter value, not fetched until now
bool Idlemeter::isNew() {
 if(isNew_) { isNew_=false; return true; }
 return false;
}

idleData Idlemeter::getLast() { return last_; }
idleData Idlemeter::getMax()  { return max1_; }
idleData Idlemeter::getMax2() { return max2_; }


//_____set duration of counting (period)________________________
void Idlemeter::setPeriod(unsigned long period_ms) {
 period_ms_=period_ms;
 reset();
}

//_____get duration of counting (period)________________________
unsigned long Idlemeter::getPeriod() {
 return period_ms_;
}

//_____get starttime of the running idle period_________________
unsigned long Idlemeter::getStarttime() {
 return starttime_ms_;
}

//**************************************************************
//     working methods
//**************************************************************

//_____Increment counter________________________________________
void Idlemeter::next() {
 //-----store values--------------------------------------------
 unsigned long millis_=millis();       // time now
 last_.msStart=starttime_ms_;          // starttime last idle
 last_.msIdle=millis_-starttime_ms_;   // duration last idle
 
 //-----start time too old?-------------------------------------
 if(millis_ > period_ms_)
 {
  if(max2_.msStart < (millis_-period_ms_))
  {
   max2_.msIdle =0;
   max2_.msStart=0;
  }
  if(max1_.msStart < (millis_-period_ms_))
  {
   max1_.msIdle =max2_.msIdle;
   max1_.msStart=max2_.msStart;
  }
 }
 //-----last value greater than max?----------------------------
 if(last_.msIdle >= max1_.msIdle)
 {
  max2_.msIdle =max1_.msIdle;
  max2_.msStart=max1_.msStart;
  max1_.msIdle =last_.msIdle;
  max1_.msStart=last_.msStart;
 }
 else
 {
  if(last_.msIdle >= max2_.msIdle)
  {
   max2_.msIdle =last_.msIdle;
   max2_.msStart=last_.msStart;
  }
 }
 //-----start next measuring/idle time--------------------------
 isNew_=true;
 starttime_ms_=millis();
}

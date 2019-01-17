//_____D1_class_Pulsecounter.cpp______________190117-190117_____
// The class Pulsecounter counts pulses in a given period.
// Default value: 60s = 1 minute
// Created by Karl Hartinger, January 17, 2019.
// Modified: -
// Released into the public domain.

#include "D1_class_Pulsecounter.h"
extern "C" {
#include "user_interface.h"
}

//**************************************************************
//     Timer interrupt
//**************************************************************
os_timer_t pulsecounter_timer;

//_____mytimer interrupt service routine________________________
void pulsecounter_ISR(void *pArg) {
 pulsecounterData.counterLast=pulsecounterData.counter;
 pulsecounterData.counter=0;
 pulsecounterData.isNew=true;
}

//**************************************************************
//     constructor & co
//**************************************************************

//_____constructor______________________________________________
Pulsecounter::Pulsecounter() { setup(PERIOD_MS); }

Pulsecounter::Pulsecounter(unsigned long period_ms)
{ setup(period_ms); }

//_____setup properties_________________________________________
void Pulsecounter::setup(unsigned long period_ms) {
 pulsecounterData.counterLast=0;
 pulsecounterData.counter=0;
 pulsecounterData.period_ms=period_ms;
 reset();
}

//**************************************************************
//     set/get values
//**************************************************************

//_____set counterLast, counter=0, start new period_____________
void Pulsecounter::reset() {
 pulsecounterData.counterLast=pulsecounterData.counter;
 pulsecounterData.counter=0;
 os_timer_disarm(&pulsecounter_timer); // stopp timer
 os_timer_setfn(&pulsecounter_timer, pulsecounter_ISR, NULL);
 os_timer_arm(&pulsecounter_timer, pulsecounterData.period_ms, true);
}

//_____check end of period______________________________________
// return: true=new counter value, not fetched until now
bool Pulsecounter::isNew() {
 if(pulsecounterData.isNew) {
  pulsecounterData.isNew=false;
  return true; 
 }
 return false;
}

//_____return last (finished) counter value_____________________
unsigned long Pulsecounter::getCounter()
{ return pulsecounterData.counterLast; }

//_____return actual counter value (period is NOT finished!)____
unsigned long Pulsecounter::getCounterNow()
{ return pulsecounterData.counter; }


//_____set duration of counting (period)________________________
void Pulsecounter::setPeriod(unsigned long period_ms) {
 pulsecounterData.period_ms=period_ms;
 reset();
}

//_____get duration of counting (period)________________________
unsigned long Pulsecounter::getPeriod() {
 return pulsecounterData.period_ms;
}

//**************************************************************
//     working methods
//**************************************************************

//_____Increment counter________________________________________
void Pulsecounter::add() { pulsecounterData.counter++; }

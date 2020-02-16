//_____D1_class_Door1.cpp_____________________181025-200209_____
// D1 mini class for watching a door:
// * door open/closed
// * door locked/unlocked
// * let a led shine 3s, if door state changed
//
// Hardware: (1) WeMos D1 mini
//               2x Digital input (D5,D6), 1x Digital out (D8)
//
// Created by Karl Hartinger, October 25, 2018.
// Update: 200125 return value of changed()=true if LED changed
//         200209 add setLanguage(), state instead of status
// Released into the public domain.

#include "D1_class_Door1.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor for default pins_____________________________
Door1::Door1() 
 {setup(DOOR1_PIN_OPEN,DOOR1_PIN_LOCK,DOOR1_PIN_LED,LANGUAGE_);}


//_____constructor: set pin numbers_____________________________
Door1::Door1(int pinOpen, int pinLock, int pinLed)
{ setup(pinOpen, pinLock, pinLed,'e'); }

//_____constructor: set pin numbers_____________________________
Door1::Door1(int pinOpen, int pinLock, int pinLed, char language)
{ setup(pinOpen, pinLock, pinLed, language); }

//_____setup output pin_________________________________________
void Door1::setup(int pinOpen, int pinLock, int pinLed, char language)
{
 pinOpen_=pinOpen;                     // default D5
 pinLock_=pinLock;                     // 
 pinLed_=pinLed;                       //
 pinMode(pinOpen_, INPUT);             // set pin mode
 pinMode(pinLock_, INPUT);             // set pin mode
 pinMode(pinLed, OUTPUT);              // set pin mode
 setLanguage(language);                // set language
 led_=0;                               // LED logical off
 digitalWrite(pinLed_, DOOR1_LEDOFF);  // LED physical off 
 state_old_=0;                         // no old state = change
 millisLedOff_=0;                      // led is already off
}

//**************************************************************
//     setter, getter, working methods
//**************************************************************

//_____test if door state has changed___________________________
// return: true=yes, false=no change
bool Door1::changed() {
 bool ret=false;
 //-----Is led shining? Time to turn led off?-------------------
 if(led_>0) {                          // is led on?
  if(millis()>millisLedOff_) {         // time to turn off LED
   led_=0;                             // LED logical off
   digitalWrite(pinLed_,DOOR1_LEDOFF); // LED physical off
   millisLedOff_=0;                    // no led off time
   ret=true;
  }
 }
 //-----door state changed?-------------------------------------
 int status_=getState();
 if(status_!=state_old_)
 {
  led_=1;                              // LED logical on
  digitalWrite(pinLed_, DOOR1_LEDON);  // LED physical on 
  millisLedOff_=millis()+DOOR1_LEDON_MS;// LED off time [ms]
  state_old_=status_;                  // remember state
  ret=true;
 }
 return ret;
}

//_____door state al value______________________________________
// 10uo OK-Led-unlocked-open: X=1 Status value OK, 
// u=1 door unlocked, o=1 door open
int Door1::getState() { 
 return(8+2*digitalRead(pinLock_)+digitalRead(pinOpen_));
}

//_____hole door state as text string___________________________
// return: strong like "Door locked (LED off)"
String Door1::getsStateAll() { 
 int st=getState();
 if((st&8)==0) return(String(FPSTR(pdoor[6+language_])));
 String s1=String(FPSTR(pdoor[7+language_]));    //"Door "
 s1+=getsStateDoor();
 s1+=" (";
 s1+=getsStateLed();
 s1+=")";
 return(s1);
}

//_____door state as string_____________________________________
// return: locked, closed, unclear, open
String Door1::getsStateDoor() { 
 int st=getState();
 if((st&8)==0) return(String(FPSTR(pdoor[6+language_])));
 return(String(FPSTR(pdoor[(st&3)+language_])));
}

//_____led state as string______________________________________
// return: LED off, LED on
String Door1::getsStateLed() { 
 return(String(FPSTR(pdoor[4+(getLed()&1)+language_])));
}

//_____get status of door led___________________________________
// return: 1=Led on, 0=LED off
int Door1::getLed() { return led_; }

//_____set language for gets..._________________________________
bool Door1::setLanguage(char language) {
 language_=0;
 if(language=='e') return true;
 if(language=='d' || language=='g') { language_=8; return true; }
 return false;
}
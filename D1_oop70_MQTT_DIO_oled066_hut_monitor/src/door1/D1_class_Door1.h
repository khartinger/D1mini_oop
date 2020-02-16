//_____D1_class_Door1.h_______________________181025-200209_____
// D1 mini class for watching a door:
// * door open/closed
// * door locked/unlocked
// * let a led shine 3s, if door state changed
// Hardware: (1) WeMos D1 mini
//               2x Digital input (D5,D6), 1x Digital out (D8)
//
// Created by Karl Hartinger, October 25, 2018.
// Update: 20-01-25 return value of changed()=true if LED changed
// Released into the public domain.

#ifndef D1_CLASS_DOOR1_H
#define D1_CLASS_DOOR1_H
#define DOOR1_PIN_OPEN          D5     // 
#define DOOR1_PIN_LOCK          D6     // 
#define DOOR1_PIN_LED           D8     // 
#define LANGUAGE_              'e'     // English
#define DOOR1_LEDON              1     // 1=3V3
#define DOOR1_LEDOFF             0     // 0=0V
#define DOOR1_LEDON_MS        3000     // 3s

#include "Arduino.h"                   // name D5,D6,D8


const char door00[] PROGMEM="locked";
const char door01[] PROGMEM="closed";
const char door02[] PROGMEM="unclear";
const char door03[] PROGMEM="open";
const char door04[] PROGMEM="LED off";
const char door05[] PROGMEM="LED on";
const char door06[] PROGMEM="Error";
const char door07[] PROGMEM="Door ";
const char door08[] PROGMEM="versperrt";
const char door09[] PROGMEM="zu";
const char door10[] PROGMEM="unklar";
const char door11[] PROGMEM="offen";
const char door12[] PROGMEM="LED aus";
const char door13[] PROGMEM="LED ein";
const char door14[] PROGMEM="Fehler";
const char door15[] PROGMEM="Tuere ";
const char* const pdoor[] PROGMEM = {
 door00,door01,door02,door03,door04,door05,door06,door07,
 door08,door09,door10,door11,door12,door13,door14,door15 };

class Door1 {
 //-----properties----------------------------------------------
 protected:
  int  pinOpen_;                       // door open pin (D5)
  int  pinLock_;                       // door closed pin (D6)
  int  pinLed_;                        // door state changed (D8)
  int  led_;                           // logical led state (1=on)
  int  state_old_;                     // old door state
  unsigned long millisLedOff_;         // ms since led on
  int  language_;                     // 0=English, 8=Deutsch
 //-----constructor & co----------------------------------------
 public:
  Door1();
  Door1(int pinOpen, int pinLock, int pinLed);
  Door1(int pinOpen, int pinLock, int pinLed, char language);
 protected:
  void setup(int pinOpen,int pinLock,int pinLed,char language);
 //-----setter, getter, working methods-------------------------
 public: 
  bool   changed();                    // true=door state changed
  int    getState();                   // 10uo OK-0-unlocked-open
  String getsStateAll();               // e.g. Led on|off etc.
  String getsStateDoor();              // e.g. open|closed|locked
  String getsStateLed();               // e.g. LED on|LED off
  int    getLed();                     // 1=LED on, 0=LED off
  bool   setLanguage(char language);   // 
};
#endif

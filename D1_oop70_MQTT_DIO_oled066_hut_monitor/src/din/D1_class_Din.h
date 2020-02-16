//_____D1_class_Din.h_________________________170402-171213_____
// D1 mini class for digital input e.g. a button. 
//    Default is D3 (=GPIO0)
// Changes 2017-12-05:
// + devide (old) D1_class_Din.h into *.h and *.cpp
// + names changed (camel case names instead of underscore)
// + getDuration() added
//
// Hardware: (1) WeMos D1 mini
//
// Created by Karl Hartinger, December 13, 2017.
// Released into the public domain.

#ifndef  D1_CLASS_DIN_H
#define  D1_CLASS_DIN_H
#include "Arduino.h"                   // D3
#define  CLICK_MS       550            // click time max. 550 ms
#define  PIN_DIN        D3             // Default input pin

class Din {
 //-----properties----------------------------------------------
 protected:
  int dinNum_;                         // pin number (D3=GPIO0)
  int din_;                            // last read din value
  int lastEdge_;                       // 0=no, 1=rising, 2=fall
  int clickMilli_;                     // max. click duration
  int click1;                          // state of 1x click
  int click2;                          // state of double click
  int clickL;                          // state of long click
  unsigned long lastMilli_;            // ms since last change
  unsigned long lastDuration_;         // ms last duration
 //-----constructor & co----------------------------------------
 public:
  Din();                               // default constructor D3
  Din(int num);                        // set input pin
 protected: 
  void setup(int num);                 // setup properties
 //-----setter, getter methods (no hardware access)-------------
 public: 
  void setClickMilli(unsigned long ms_max); //max.click duration
  int  getLast(void);                  // last val, NO new input
  unsigned long getDuration();         // msec since last change
  unsigned long getLastDuration();     // msec change before
 //-----working methods-----------------------------------------
 public: 
  int  read(void);                     // read input, return val
  bool isPressed(void);                // read input, 0V -> true
  bool isPressed(unsigned long ms_min);// is input 0V min. xx ms
  bool isFallingEdge(void);            // read input,true v edge
  bool isRisingEdge(void);             // read input,true ^ edge
  bool isClicked();                    // is 1x click?
  bool isDoubleClicked();              // is 2x click?
};
#endif

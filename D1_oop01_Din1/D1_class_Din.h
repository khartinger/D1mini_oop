//_____D1_class_Din.h_________________________170402-171205_____
// D1 mini class for digital input e.g. a button. 
//    Default is D3 (=GPIO0)
// Changes 2017-12-05:
// + devide (old) D1_class_Din.h into *.h and *.cpp
// + names changed (camel case names instead of underscore)
// + getDuration() added
//
// Hardware: (1) WeMos D1 mini
//
// Created by Karl Hartinger, December 05, 2017.
// Released into the public domain.

#ifndef D1_CLASS_DIN_H
#define D1_CLASS_DIN_H
#include "Arduino.h"                   // D3

class Din {
 //-----properties----------------------------------------------
 protected:
  int dinNum_;                         // pin number (D3=GPIO0)
  int din_;                            // last read din value
  int lastEdge_;                       // 0=no, 1=rising, 2=fall
  unsigned long lastMilli_;            // ms since last change
  unsigned long lastDuration_;         // ms last duration
 //-----constructor & co----------------------------------------
 public:
  Din();                               // default constructor
  Din(int num);                        // set input pin
 protected: 
  void setup(int num);                 // setup properties
 //-----setter, getter, working methods-------------------------
 public: 
  int  get(void);                      // read input, return val
  bool isPressed(void);                // read input, 0V -> true
  bool isFallingEdge(void);            // read input
  bool isRisingEdge(void);             // read input
  int  getLast(void);                  // last val, NO new input
  unsigned long getDuration();         // msec since last change
  unsigned long getLastDuration();     // msec change before
};
#endif

//_____D1_class_Dout.h________________________170402-200414_____
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

#ifndef D1_CLASS_DOUT_H
#define D1_CLASS_DOUT_H
#define DEFAULT_PIN              2     // D4 = GPIO2 (blue led)
#define DEFAULT_INVERT       false     // 0=0V, 1=5V
#define DEFAULT_VALUE            0     // start with pin=0V
#include "Arduino.h"                   // D4, pinMode

class Dout {
 //------properties---------------------------------------------
 protected:
  int  doutNum_;                       // pin number (D4=GPIO2)
  bool invert_;                        // false 0=0V, true 0=3V3
  int  val_;                           // output: 0=0V, 1=3V3
  unsigned long lastMilli_;            // ms since last change
 //------constructor & co---------------------------------------
 public:
  Dout();
  Dout(int num);
  Dout(bool invert);
  Dout(int num, bool invert);
  Dout(int num, int startvalue);
  Dout(int num, bool invert, int startvalue);
 protected:
  void setup(int num, bool invert, int startvalue);
 //------setter, getter, working methods------------------------
 public: 
  void set(int val);                   // physical 0=0V, 1=3V3
  void on(void);                       // logical on
  void off(void);                      // logical off
  void toggle();                       // change output
  unsigned long getDuration();         // msec since last change
};
#endif

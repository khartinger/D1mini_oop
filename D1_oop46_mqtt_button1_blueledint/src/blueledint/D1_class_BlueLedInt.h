//_____D1_class_BlueLedInt.h__________________181101-181101_____
// D1 mini class for blue led on D1 mini shield.
// Function: on, off, toggle, blink, brightness
// Default: pin D4=GPIO2
// Hardware: (1) WeMos D1 mini
// Note    : Class BlueLedInt uses timer interrupt
// Created by Karl Hartinger, November 01, 2018.
// Changes : -
// Released into the public domain.
#pragma once
extern "C" {
#include "user_interface.h"
}

#ifndef D1_CLASS_BLUELEDINT_H
#define D1_CLASS_BLUELEDINT_H 

#include "Arduino.h"                   // D4
#define BLUELEDINT_FREQ         5000   // PWM frequency [Hz]
#define BLUELEDINT_BRIGHTNESS    100   // default=full brightness
#define BLUELEDINT_PIN            D4   // pin blue led
#define BLUELEDINT_ON              0   // 0=on
#define BLUELEDINT_OFF             1   // 1=off
#define BLUELEDINT_BLINK         500   // blink duration 500ms

class BlueLedInt {
 //-----properties----------------------------------------------
 // see struct blueLedTimer
 int brightness_;
 bool usePwm_;
 //-----constructor & co----------------------------------------
 public:
  BlueLedInt();
  BlueLedInt(int pin);
 protected:
  void setup(int pin);
  void set(int valLogical);
 //-----setter, getter, working methods-------------------------
 public: 
  void setPin(int pin);                // default D4
  void on(void);                       // led on
  void off(void);                      // led off
  void toggle();                       // change output
  void blink(int on_ms,int off_ms);    // blink duration
  void blinkOn();                      // start blinking (again)
  void blinkOff();                     // stopp blinking
  void brightness(int persentOn);      // brightness 0..100 [%]
  void brightnessOn();                 // brightness on
  void brightnessOff();                // brightness off
};
#endif

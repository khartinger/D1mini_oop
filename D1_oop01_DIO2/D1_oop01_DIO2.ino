//_____D1_oop01_DIO2.ino__________________20170402-20180507_____
// Test digital input and output using this
// Hardware: (1) WeMos D1 mini
//           (2) D1_IO_Shield1: 2x LEDs at D7 (green), D8 (red)
//                              1x Button at D5
//           (3) Button Shield: 1x Button at D3 (or wire)
// Function: Red LED lights up for 1,5s, green LED 0,5s, ...
//           If button D5 is pressed, both LEDs are on
//           (=> yellow, if DUO-LED is used).
//           Blue led (D4) shows inverted state of Button D3
//           (blue led lights, when input is 0)
// Created by Karl Hartinger, April 02, 2017,
// Last modified 2018-05-07: Move class files to /src/...
// Released into the public domain.

#include "src/din/D1_class_Din.h"
#include "src/dout/D1_class_Dout.h"
Din button3;
Din button5(D5);
Dout led_blue;
Dout led_green(D7);
Dout led_red(D8);
int state=0;

void setup() {
 led_red.on();
}

void loop() {
 led_blue.set(button3.read());     //button D3 -> blue led
 if(button5.isPressed())
 {
  led_red.on();
  led_green.on();
 }
 else //=====button D5 not pressed==============================
 {
  if((state++)>20) state=0;
  if(state<15)
  {
   led_red.on();
   led_green.off();
  }
  else
  {
   led_red.off();
   led_green.on();
  }
 }
 delay(100);
}

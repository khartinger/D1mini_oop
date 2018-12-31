//_____D1_class_BlueLedInt.cpp________________181118-181118_____
// D1 mini class for blue led on D1 mini shield.
// Function: on, off, toggle, blink, brightness
// Default: pin D4=GPIO2
// Hardware: (1) WeMos D1 mini
// Created by Karl Hartinger, November 18, 2018.
// Changes: -
// Released into the public domain.
#include "D1_class_BlueLedInt.h"

//_____timer interrupt__________________________________________
os_timer_t blueLedTimer1;              // timer

struct {
 int pinNum=BLUELEDINT_PIN;
 int pinVal=BLUELEDINT_OFF;
 int msOn  =BLUELEDINT_BLINK;
 int msOff =BLUELEDINT_BLINK;
} blueLedTimer;

void blueLedTimerISR(void *pArg)
{
 os_timer_disarm(&blueLedTimer1);                     //stopp timer
 os_timer_setfn(&blueLedTimer1, blueLedTimerISR, NULL);
 blueLedTimer.pinVal=1-blueLedTimer.pinVal;
 digitalWrite(blueLedTimer.pinNum, blueLedTimer.pinVal);
 if(blueLedTimer.pinVal==0)
  os_timer_arm(&blueLedTimer1, blueLedTimer.msOn,  false);
 else
  os_timer_arm(&blueLedTimer1, blueLedTimer.msOff, false);
}

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default digital out is D4, inverted)________
BlueLedInt::BlueLedInt() { setup(BLUELEDINT_PIN); }

//_____constructor (pin is inverted: 0=led on, 1=led off)_______
BlueLedInt::BlueLedInt(int pin) { setup(pin); }

//_____setup output pin and properties__________________________
void BlueLedInt::setup(int pin)
{
 os_timer_disarm(&blueLedTimer1);      //stopp timer
 blueLedTimer.pinNum=pin;              // set pin number (D4)
 pinMode(blueLedTimer.pinNum, OUTPUT); // set pin to output
 analogWriteRange(100);                // for analog... 
 analogWriteFreq(BLUELEDINT_FREQ);     // ...(pwm) output
 set(blueLedTimer.pinVal);             // set output
 brightness_=BLUELEDINT_BRIGHTNESS;    // 100% brightness
 usePwm_=false;                        // no pwm
}

//_____set digital output pin physical: 0=0V, 1=3V3_____________
void BlueLedInt::set(int val)
{
 if(val!=0) {                          // set 1 (= off)
  blueLedTimer.pinVal=1;               //
  digitalWrite(blueLedTimer.pinNum, blueLedTimer.pinVal);
  return;
 }
 blueLedTimer.pinVal=0;                // set 0 (=on)
 if(usePwm_) brightness(brightness_);  // use brightness value
 else digitalWrite(blueLedTimer.pinNum, blueLedTimer.pinVal);
}

//**************************************************************
//     setter, getter, working methods
//**************************************************************

//_____set pin, calls setup()___________________________________
void BlueLedInt::setPin(int pin) { setup(pin); }

//_____turn digital output logical on (can be 3V3 or 0V)________
void BlueLedInt::on(void)  { 
 os_timer_disarm(&blueLedTimer1);      //stopp timer
 set(BLUELEDINT_ON); 
}

//_____turn digital output logical off (can be 0V or 3V3)_______
void BlueLedInt::off(void) { 
 os_timer_disarm(&blueLedTimer1);      //stopp timer
 set(BLUELEDINT_OFF); 
}

//_____toggle digital output____________________________________
void BlueLedInt::toggle(void) {
 os_timer_disarm(&blueLedTimer1);      //stopp timer
 set(1-blueLedTimer.pinVal); 
}

//_____set blink mode___________________________________________
// value <=0: no blinking
void BlueLedInt::blink(int on_ms, int off_ms)
{
 if(on_ms<=0) return; 
 blueLedTimer.msOn=on_ms;              //
 if(off_ms<=0) return;
 blueLedTimer.msOff=off_ms;            //
 os_timer_disarm(&blueLedTimer1);      //stopp timer
 os_timer_setfn(&blueLedTimer1, blueLedTimerISR, NULL);
 os_timer_arm(&blueLedTimer1, 0 , false);
/*
 os_timer_setfn(&blueLedTimer1, blueLedTimerISR, NULL);
 blueLedTimer.pinVal=1-blueLedTimer.pinVal;
 digitalWrite(blueLedTimer.pinNum, blueLedTimer.pinVal);
 
 //blueLedTimer.pinVal=1-blueLedTimer.pinVal;
 if(blueLedTimer.pinVal==1)
  os_timer_arm(&blueLedTimer1, blueLedTimer.msOn, false);
 else
  os_timer_arm(&blueLedTimer1, blueLedTimer.msOff, false);
*/
}

//_____turn blinking on_________________________________________
void BlueLedInt::blinkOn() {
 os_timer_disarm(&blueLedTimer1);      //stopp timer
 os_timer_setfn(&blueLedTimer1, blueLedTimerISR, NULL);
 os_timer_arm(&blueLedTimer1, 1, false);
}

//_____turn blinking off________________________________________
void BlueLedInt::blinkOff() {
 os_timer_disarm(&blueLedTimer1);      // stopp timer
}

//_____set brightness of blue led (0..100)______________________
void BlueLedInt::brightness(int persentOn)
{
 blinkOff();
 usePwm_=true;
 if(persentOn<0)   { usePwm_=false; return; }
 if(persentOn>100) { usePwm_=false; return; }
 //if(persentOn<0)   { usePwm_=false; persentOn=0;  }
 //if(persentOn>100) { usePwm_=false; persentOn=100; }
 brightness_=persentOn;
 int aout=100-brightness_;
 analogWrite(blueLedTimer.pinNum, aout);
}

//_____turn pwm off_____________________________________________
void BlueLedInt::brightnessOn() { usePwm_=true; }

//_____turn pwm off_____________________________________________
void BlueLedInt::brightnessOff() { usePwm_=false; }

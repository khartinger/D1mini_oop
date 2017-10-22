//_____D1_oop22_powerOff_6sec.ino_____________171017-171017_____
// Hardware: 
// (1) WeMos D1 mini
// (2) D1_OnOff shield
// ***** Function of D1_OnOff shield ***************************
// When SET button S1 on the D1_OnOff shield is pressed, a
// bipolar relay is set and the power remains switched on even 
// when button S1 is released [pin D6 must be switched off (0)].
// Then the LED lights red, yellow and green (for 2 sec each)
// and the supply voltage is switched off.
// Important: supply voltage by D1_OnOff shield! (Not by USB)

#include "D1_class_Dout.h"
Dout  pin_relay(D6);              // D6=1: reset relay (off)
Dout  led_green(D7);              // green led @ D1_OnOff shield
Dout  led_red(D8);                // red led @ D1_OnOff shield

//_____setup____________________________________________________
void setup() 
{
 //-----init output pins----------------------------------------
 pin_relay.off();                 // let relay be set (external)
}

//_____main loop________________________________________________
void loop() 
{
 led_green.off();                 // +
 led_red.on();                    // + led = red 
 delay(2000);                     // wait 2sec
 led_green.on();                  // + led = orange
 led_red.on();                    // + led = orange
 delay(2000);                     // wait 2sec
 led_green.on();                  // +
 led_red.off();                   // + led = green
 delay(2000);                     // wait 2sec
 pin_relay.on();                  // reset relay (power off)
 led_green.off();                 // led = off
 led_red.off();                   // led = off
 delay(2000);                     //  just for safety reasons
}

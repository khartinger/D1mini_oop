//_____D1_oop22_powerOff_ain_buttonD3.ino_____171021-171021_____
// Hardware: 
// (1) WeMos D1 mini
// (2) D1_OnOff shield
// (3) D1 mini 1-button shield
// *****Function of D1_OnOff shield*****************************
// When the SET button S1 on D1_OnOff shield is pressed,
// a bipolar relay is set and the power remains switched on even 
// when button S1 is released [pin D6 must be switched off (0)].
// The LED lights red.
// When button D3 is pressed, the supply voltage is measured,
// sent to Serial and supply voltage is switched off.   
// Important: supply voltage by D1_OnOff shield! (Not by USB)

#include "D1_class_Din.h"
#include "D1_class_Dout.h"
Din   pin_button(D3);             // Button for power off
Dout  pin_relay(D6);              // D6=1: reset relay (off)
Dout  led_green(D7);              // green led @ D1_OnOff shield
Dout  led_red(D8);                // red led @ D1_OnOff shield
const int pinAin=A0;              // analog input pin
const float ain_x2=1023, ain_y2=5.195, ain_x1=0, ain_y1=0;
const float ain_k=(ain_y2-ain_y1)/(ain_x2-ain_x1);
const float ain_d=(ain_x2*ain_y1-ain_x1*ain_y2)/(ain_x2-ain_x1);

//_____setup____________________________________________________
void setup() 
{
 //-----init output pins----------------------------------------
 pin_relay.off();                 // let relay be set (external)
 led_green.off();                 // +
 led_red.on();                    // + led = red 
 //-----init Serial---------------------------------------------
 Serial.begin(9600); Serial.println("\nWarten auf Tastendruck");
}

//_____main loop________________________________________________
void loop() 
{
 if(pin_button.is_falling_edge())
 {
  led_green.on();                  // + led = green
  led_red.off();                   // + 
  //-----read analog in, convert to voltage and send to Serial--
  int ain1023=analogRead(pinAin);  // analog in value (0..1023)
  float voltage=ain_k*ain1023 + ain_d;
  String s1=String(ain1023)+" = "+String(voltage)+" V";
  Serial.println(s1);             // send to Serial
  delay(2000);                    // wait 2sec  
  //-----turn power off-----------------------------------------
  pin_relay.on();                 // reset relay (power off)
  led_green.off();                // led = off
  led_red.off();                  // led = off
  delay(2000);                    // just for safety reasons
 }
}

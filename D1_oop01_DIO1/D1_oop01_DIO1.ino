//_____D1_oop01_DIO1.ino___________________________20170402_____
// Status of button (D3=DGPIO0) is shown on blue LED (D4=GPIO2).
// (Button pressed -> input=0 -> led lights)
// If button is pressed or released: message to Serial
// using classes.
// Hardware: WeMos D1 mini
//           1-Button Shield
#include "D1_class_Din.h"
#include "D1_class_Led.h"
Din button;
Led led;

void setup() {
 Serial.begin(9600);              // init serial comm 9600 bps
 Serial.println();
}

void loop() {
 led.set(button.get());
 if(button.is_falling_edge())
  Serial.println("Button pressed!");
 if(button.is_rising_edge())
  Serial.println("Button released!");
}

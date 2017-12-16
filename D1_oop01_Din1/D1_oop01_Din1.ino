//_____D1_oop01_Din1.ino______________________171213-171213_____
// Demonstrate button clicked methods of class Din:
// * if falling edge: print message to Serial
// * if rising  edge: print message to Serial
// * if button pressed longer than 1 sec: print message to Serial
// * if button is clicked short once: print message to Serial
// * if button is clicked short twice: print message to Serial
//
// Cycle time 0,1 sec, print results to Serial (115200 Baud).
//
// Hardware: (1) WeMos D1 mini
//           (2) Button shield (D3) or wire to GND
//
// Created by Karl Hartinger, December 07, 2017.
// Released into the public domain.

#include "D1_class_Din.h"
#include "D1_class_Dout.h"

#define BAUDRATE             115200    // for Serial
Din  button(D5);                       // button D5
Din  button;                           // button (default D3)
Dout blueLed(true);                    // blue LED (default D4)

void setup() 
{
 //-----begin Serial, read any existing Serial data-------------
 if(!Serial) Serial.begin(BAUDRATE);   // init serial comm
 while(!Serial) {;};                   // wait for Serial
 do{delay(10);} while(Serial.available() && Serial.read()>=0);
 Serial.println("\nPress button D3!"); // info
 //-----set maximum time for click changes
 button.setClickMilli(500L);
}

int i=0, imax=10;
void loop() 
{
 //-----read button and write value to blue led-----------------
 int state=button.read();
 blueLed.set(state);
 if(button.isFallingEdge()) Serial.println("v  Falling Edge");
 if(button.isRisingEdge())  Serial.println("\r\n^  Rising Edge");
 if(button.isPressed(1000)) Serial.println("LongClick!");
 if(button.isClicked())     Serial.println("Click!");
 if(button.isDoubleClicked()) Serial.println("DoubleClick!");
 if(button.isPressed())     Serial.print("-P");
 delay(100);                           // wait 0,1 sec
}

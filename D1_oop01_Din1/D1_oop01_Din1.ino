//_____D1_oop01_Din1.ino______________________171207-171207_____
// Demonstrate methods of class Din:   
// * get duration of last change and print this time in seconds
// * read button status (0|1) and show it on blue led
// * check for falling and rising edge, print a message if true
// * if button is pressed, print P on Serial
// Cycle time 0,2s, results are printed on Serial (115200 Baud).

//
// Hardware: (1) WeMos D1 mini
//           (2) Button shield (D3) or wire to GND
//
// Created by Karl Hartinger, December 07, 2017.
// Released into the public domain.

#include "D1_class_Din.h"
#include "D1_class_Dout.h"

#define BAUDRATE             115200    // for Serial
Din  button;                           // button (default D3)
Dout blueLed(true);                    // blue LED (default D4)

void setup() 
{
 //-----begin Serial, read any existing Serial data-------------
 if(!Serial) Serial.begin(BAUDRATE);   // init serial comm
 while(!Serial) {;};                   // wait for Serial
 do{delay(10);} while(Serial.available() && Serial.read()>=0);
 Serial.println("\nPress button D3!"); // info
}

int i=0, imax=10;
void loop() 
{
 //-----loop control: print duration since last change----------
 float fms=button.getDuration()/1000.0;// actual duration -> sec
 Serial.print(fms); Serial.print("s ");
 i++;
 if(i>=imax) {i=0; Serial.println();}  // new line
 //-----read button and write value to blue led-----------------
 int state=button.get();
 blueLed.set(state);
 //-----check for edges-----------------------------------------
 if(button.isFallingEdge())
 {
  i=0;
  Serial.println();delay(1);
  Serial.print("Last Duration: ");
  Serial.print(button.getLastDuration());
  Serial.println(" ms ---> Button pressed!");
 }
 if(button.isRisingEdge())
 {
  i=0;
  Serial.println();delay(1);
  Serial.print("Last Duration: ");
  Serial.print(button.getLastDuration());
  Serial.println(" ms ---> Button released!");
 }
 //-----if button pressed, print P------------------------------
 if(button.isPressed()) Serial.print("P ");
 delay(200);                           // wait 0,2 sec
}

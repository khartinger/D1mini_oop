//_____D1_oop60_sws_int counterD3.ino_________190115-190115_____
// * Pressing button D3 increments a counter.
// * Counter value is send to software serial (RX=D6, TX=D5)
//
// Hardware: (1) WeMos D1 mini
//           (2) 3V3/RS-232-Adapter connected to a Laptop/PC
//               TXD <---> D6 (=RX), RXD <---> D5 (=TX), GND
// Created by Karl Hartinger, January 15, 2019.
// Last modified: -
// Released into the public domain.

#include "src/softserial2/D1_class_SoftSerial2.h"

#define  VERSION60  "2019-01-15 D1_oop60_sws_int_counterD3"
#define  FUNCTION60 "Press D3 to increment counter, sent to soft serial"
#define  PIN_BUTTON             D3     // D3 is default button
#define  MILLI_PULSDELAY        30     // 30ms between 2 pulses

SoftSerial2 seri(D6, D5);              // RX,TX
unsigned long milliNext;               // next pulse allowed
bool     newPuls=false;                // no new pulse
unsigned long counter=0;               // impulse counter

//_____io interrupt service routine_____________________________
void io_ISR(void)
{
 unsigned long msStart=millis();       // time betweeen pulses..
 if(msStart<milliNext) return;         // ..too short
 //-----test for falling edge (but not a short peak)------------
 int i=digitalRead(PIN_BUTTON);        // 1. read input
 i+=digitalRead(PIN_BUTTON);           // 2. read input
 i+=digitalRead(PIN_BUTTON);           // 3. read input
 if(i>0) return;                       // return if not 3x low
 //-----falling edge = Button pressed---------------------------
 counter++;                            // increment counter
 newPuls=true;                         // new impulse here
 milliNext=msStart+MILLI_PULSDELAY;    // next pulse allowed
}

//_____SETUP____________________________________________________
void setup() {
 //-----setup I/O-----------------------------------------------
 pinMode(PIN_BUTTON, INPUT);           // pin button = input
 milliNext=millis();                   // 
 //-----setup hardware serial (baudrate, clear buffer)----------
 seri.begin(9600);                     // init Serial
 seri.println("\r\n"+String(VERSION60));
 seri.println(String(FUNCTION60));
 Serial.begin(9600);                   // init Serial
 Serial.println("\r\n"+String(VERSION60));
 Serial.println(String(FUNCTION60));
 //-----io interrupt--------------------------------------------
 attachInterrupt(PIN_BUTTON, io_ISR, FALLING);    // CHANGE,...
}

//_____LOOP_____________________________________________________
void loop() {
 //-----impulse here?-------------------------------------------
 if(newPuls)
 {
  newPuls=false;                       // act on impulse
  seri.println(counter);               // print counter value
  Serial.println(counter);             // print counter value
 }
 //-----do nothing else-----------------------------------------
 delay(100);
}

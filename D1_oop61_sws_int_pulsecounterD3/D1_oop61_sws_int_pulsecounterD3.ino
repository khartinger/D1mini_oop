//_____D1_oop61_sws_int pulsecounterD3.ino____190117-190117_____
// * Count pulses in a period (e.g. 10 seconds) and
// * send (final) counter value to software serial (RX=D6, TX=D5)
//
// Hardware: (1) WeMos D1 mini
//           (2) 3V3/RS-232-Adapter connected to a Laptop/PC
//               TXD <---> D6 (=RX), RXD <---> D5 (=TX), GND
//
// Created by Karl Hartinger, January 17, 2019.
// Last modified: -
// Released into the public domain.

#include "src/softserial2/D1_class_SoftSerial2.h"
#include "src/pulsecounter/D1_class_pulsecounter.h"

#define  DEBUG61    true               //true: print also Serial
#define  VERSION61  "2019-01-17 D1_oop61_sws_int_pulsecounterD3"
#define  FUNCTION61 "Count pulses on D3 in a period, show value on serial"
#define  PIN_BUTTON             D3     // D3 is default button
#define  MILLI_PULSDELAY        30     // 30ms between 2 pulses

SoftSerial2  seri(D6, D5);             // RX,TX
Pulsecounter pulseCnt;                 // pulsecounter

//_____io interrupt service routine_____________________________
void io_ISR(void)
{
 //-----test for falling edge (but not a short peak)------------
 int i=digitalRead(PIN_BUTTON);        // 1. read input
 i+=digitalRead(PIN_BUTTON);           // 2. read input
 i+=digitalRead(PIN_BUTTON);           // 3. read input
 if(i>0) return;                       // return if not 3x low
 //-----falling edge = Button pressed---------------------------
 pulseCnt.add();                       // increment pulsecounter
}

//_____SETUP____________________________________________________
void setup() {
 //-----setup I/O-----------------------------------------------
 pinMode(PIN_BUTTON, INPUT);           // pin button = input
 //-----setup hardware serial (baudrate, clear buffer)----------
 seri.begin(9600);                     // init Serial
 seri.println("\r\n"+String(VERSION61));
 seri.println(String(FUNCTION61));
 if(DEBUG61) {
  Serial.begin(9600);                  // init Serial
  Serial.println("\r\n"+String(VERSION61));
  Serial.println(String(FUNCTION61));
 }
 //-----pulsecounter parameter----------------------------------
 pulseCnt.setPeriod(10000);            // measure period 10 secs
 seri.println("Period: 10 sec");
 if(DEBUG61) Serial.println("Period: 10 sec");
 //-----io interrupt--------------------------------------------
 attachInterrupt(PIN_BUTTON, io_ISR, FALLING);   // CHANGE,...
}

//_____LOOP_____________________________________________________
void loop() {
 //-----impulse here?-------------------------------------------
 if(pulseCnt.isNew())
 {
  unsigned long cnt=pulseCnt.getCounter();
  seri.println(cnt);                   // print counter value
  if(DEBUG61) Serial.println(cnt);     // print counter value
 }
 //-----do nothing else-----------------------------------------
 seri.print(".");
 if(DEBUG61) Serial.print(".");
 delay(1000);
}

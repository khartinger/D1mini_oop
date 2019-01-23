//_____D1_oop62_sws_idlemeterD3.ino___________190118-190118_____
// * Measure the time between two pulses ("idle time") and 
//   print value on software serial.
// * Print maximum idle time of a period (e.g. 30 seconds)
//
// Hardware: (1) WeMos D1 mini
//           (2) 3V3/RS-232-Adapter connected to a Laptop/PC
//               TXD <---> D6 (=RX), RXD <---> D5 (=TX), GND
//           (3) 1-Button-Shield D3 (make pulses)
//
// Created by Karl Hartinger, January 18, 2019.
// Last modified: -
// Released into the public domain.

#include "src/softserial2/D1_class_SoftSerial2.h"
#include "src/idlemeter/D1_class_Idlemeter.h"

#define  DEBUG62    true               //true: print also Serial
#define  VERSION62  "2019-01-18 D1_oop62_sws_idlemeterD3"
#define  FUNCTION62 "Measure (idle) time between two pulses, show on software serial"
#define  PIN_BUTTON             D3     // D3 is default button
#define  MILLI_PULSDELAY        30     // 30ms between 2 pulses

SoftSerial2  seri(D6, D5);             // RX,TX
Idlemeter    idlemeter;                // 

//_____io interrupt service routine_____________________________
void io_ISR(void)
{
 //-----test for falling edge (but not a short peak)------------
 int i=digitalRead(PIN_BUTTON);        // 1. read input
 i+=digitalRead(PIN_BUTTON);           // 2. read input
 i+=digitalRead(PIN_BUTTON);           // 3. read input
 if(i>0) return;                       // return if not 3x low
 //-----falling edge = Button pressed---------------------------
 idlemeter.next();                     // finish period
}

//_____SETUP____________________________________________________
void setup() {
 //-----setup I/O-----------------------------------------------
 pinMode(PIN_BUTTON, INPUT);           // pin button = input
 //-----setup hardware serial (baudrate, clear buffer)----------
 seri.begin(9600);                     // init Serial
 seri.println("\r\n"+String(VERSION62));
 seri.println(String(FUNCTION62));
 if(DEBUG62) {
  Serial.begin(9600);                  // init Serial
  Serial.println("\r\n"+String(VERSION62));
  Serial.println(String(FUNCTION62));
 }
 //-----pulsecounter parameter----------------------------------
 idlemeter.setPeriod(30000);           // period 30 secs
 seri.println("Period: 30 sec");
 if(DEBUG62) Serial.println("Period: 30 sec");
 //-----io interrupt--------------------------------------------
 attachInterrupt(PIN_BUTTON, io_ISR, FALLING);   // CHANGE,...
}

//_____LOOP_____________________________________________________
void loop() {
 //-----impulse here?-------------------------------------------
 if(idlemeter.isNew())
 {
  idleData last=idlemeter.getLast();
  idleData max=idlemeter.getMax();
  String s1="\r\nNow: "+String((millis()/1000))+"s";
  s1+=" | Last @ "+String((last.msStart/1000))+"s: ";
  s1+=String((last.msIdle/1000))+"s";
  s1+=" | MAX @ "+String((max.msStart/1000))+"s: ";
  s1+=String((max.msIdle/1000))+"s";
  seri.println(s1);                    // print idle time
  if(DEBUG62) Serial.println(s1);      // print idle time
 }
 //-----do nothing else-----------------------------------------
 seri.print(".");
 if(DEBUG62) Serial.print(".");
 delay(1000);
}

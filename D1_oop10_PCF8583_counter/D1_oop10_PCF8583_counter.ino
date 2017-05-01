//_____D1_oop10_PCF8583_counter.ino___________170416-170418_____
// PCF8583 works as an event-counter with i2c interface. Every 
//   pull down edge on pin 1 (osci) is counted (max. 999999).
//   Counter value is shown every 2 secs on Serial
//   Every 4 pulses an interrupt occurs for 1 second
// Hardware: WeMos D1 mini
//           PCF8583 (event-counter/clock with 240x8-bit RAM)
//             3V3 (INT) SCL  SDA
//           +--8----7----6----5--+     Against keybounce e.g.:
//           \      PCF8583       |     10kOhm from pin 1 to 8
//           /                    |     10nF   from pin 1 to 4
//           +--1----2----3----4--+     A0=GND (=address 0x50)
//           osci (osco) A0   GND
#include "D1_class_PCF8583.h"
PCF8583 pcf8583;                       // PCF8583 object
long cnt_;

void setup() {
 Serial.begin(9600);                   // init serial comm
 Serial.println();                     // next line
 //-----PCF8583 as counter--------------------------------------
 pcf8583.setMode(MODE_COUNTER);        // PCF8583 as counter
 int address = pcf8583.getAddress();   // i2c address of 8583
 int status  = pcf8583.getStatus();    // reading address ok?
 Serial.print("Status for 0x");        // + show
 Serial.print(address,HEX);            // | address
 Serial.print(" is ");                 // | and
 if(status==0) Serial.println("OK");   // | status OK
          else Serial.println(status); // + or error
 cnt_=432100L;
 pcf8583.setCounter(cnt_);             // just to demo preset ;)
 pcf8583.setCounterAlarm(cnt_+4);      // just to demo Alarm ;)
}

void loop() {
 cnt_=pcf8583.getCounter();            // read counter value
 Serial.println(cnt_);                 // write to Serial
 //-----test alarm mode-----------------------------------------
 if(pcf8583.isCounterAlarm()) {        // is alarm?
  Serial.println("Alarm");             // 
  delay(1000);                         // let INT 0V for 1 sec
  pcf8583.clearCounterAlarm();         // set INT to 3V3 again
  pcf8583.setCounterAlarm(pcf8583.getCounter()+4); // new alarm
 }
 delay(2000);
}

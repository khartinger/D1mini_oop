//_____D1_oop11_PCF8583_clock.ino_____________170416-170430_____
// PCF8583 works as a real time clock with i2c interface.
// Every 10 secs a alarm is generated.
// Hardware: WeMos D1 mini
//           PCF8583 (event-counter/clock with 240x8-bit RAM)
//             3V3 (INT) SCL  SDA
//           +--8----7----6----5--+     32.768kHz quartz crystal
//           \      PCF8583       |     from pin 1 to 2
//           /                    |     
//           +--1----2----3----4--+     A0=3V3 (=address 0x51)
//             osci osco A0   GND
#include "D1_class_PCF8583.h"
PCF8583 pcf8583(0x50);            // PCF8583 as clock at 0x50

int hh=23, mm=59, ss=40;
String date="20170430";

void setup() {
 Serial.begin(9600);                   // init serial comm
 Serial.println();                     // next line
 //-----is PCF8583 present?-------------------------------------
 int address = pcf8583.getAddress();   // i2c address of 8583
 int status  = pcf8583.getStatus();    // reading address ok?
 Serial.print("Status for 0x");        // + show
 Serial.print(address,HEX);            // | address
 Serial.print(" is ");                 // | and
 if(status==0) Serial.println("OK");   // | status OK
          else Serial.println(status); // + or error
 //-----set date and time---------------------------------------
 String t1=pcf8583.time2string(hh,mm,ss);
 status=pcf8583.setDate(date);         // (pre)set date
 status=pcf8583.setTime(t1);           // (pre)set time
 //-----BEGIN: set alarm date and time--------------------------
 //status=pcf8583.setAlarmMode(CLOCK_ALARM);  //
 status=pcf8583.setAlarmMode(CLOCK_ALARM_DAILY);
 status=pcf8583.setAlarmDate(date);         // (pre)set date
 ss=ss+10; t1=pcf8583.time2string(hh,mm,ss);// new time
 status=pcf8583.setAlarmTime(t1);           // (pre)set time
 //-----END: set alarm date and time----------------------------
}

void loop() {
 Serial.print(pcf8583.getWeekday()+" ");
 Serial.print(pcf8583.getDate()+" ");
 Serial.println(pcf8583.getTime());
 //-----BEGIN: test alarm mode----------------------------------
 if(pcf8583.isClockAlarm()) {          // is alarm?
  Serial.println("Alarm");             // 
  delay(2000);                         // INT-pin 0V for 2 sec
  pcf8583.clearClockAlarm();           // INT-pin 3V3 again
  //.....new alarm time.........................................
  ss=ss+10;
  if(ss>59) { mm=mm+ss/60; ss=ss%60; }
  if(mm>59) { hh=hh+mm/60; mm=mm%60; }
  hh=hh%24;
  String t1=pcf8583.time2string(hh,mm,ss);  // new time
  Serial.println("New Alarm Time: "+t1); 
  pcf8583.setAlarmTime(t1);                 // (pre)set time
 }
 else
 //-----END: test alarm mode------------------------------------
 delay(2000);
}


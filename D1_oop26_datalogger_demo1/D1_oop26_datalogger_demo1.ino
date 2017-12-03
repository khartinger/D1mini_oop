//_____D1_oop26_datalogger_demo1.ino__________171203-171203_____
// Demo for Data Logger Shield (microSD+DS1307RTC-Shield):
// Read 5 analog in values and write them with date and time 
// into file "analogin.txt" on microSD card.
// Status and results are printed to Serial (115200 Baud).
// * Setup: init Serial, blue led on, reference points analog in
// * Setup: old data file on micro SD is deleted (removed)
// * Setup: try to connect to WLAN, fetch actual time from a
//   server and write it to RTC. If no success, use old RTC time
// * Loop : read analog value, print it and write it into file
// * Finish: turn blue led off
// * Finish: print file content to Serial
//
// Hardware: (1) WeMos D1 mini
//           (2) Data logger shield (microSD + DS1307RTC-Shield)
//           (3) analog input values
//
// Created by Karl Hartinger, December 03, 2017.
// Released into the public domain.

#include "D1_class_Dout.h"             // switch blue led
#include "D1_class_Ain.h"              // get analog in value
#include "D1_class_DS1307.h"           // real time clock
#include "D1_class_TimeHttp.h"         // get time from a server
#include "D1_class_SDfile.h"           // micro SD card
#define TIMEZONE 1

TimeHttp time1("..ssid..", "..password..","httpservername");
Dout blueled(true);                    // invert! on=0V
Ain ain;                               // object analog in
DS1307 ds1307;                         // object DS1307 clock
SDfile file1("analogin.txt");
bool     bTimeset=false;

//_____set time_________________________________________________
String setHttpTime()
{
 //-----try to get date and time from server--------------------
 int ret=-1;
 String sDT="";
 if(DEBUG1) Serial.println("Starting a HTTP-request...");
 sDT=time1.getDateTime();
 //-----if values available: try to set time--------------------
 if(sDT!="") {
  int st=time1.getSummertime(sDT, TIMEZONE);
  sDT=time1.addsubHours(sDT, st+TIMEZONE);
  ret=ds1307.setDateTime(sDT);
 }
 if(ret==0) bTimeset=true;             // success time set
 //-----print result to Serial----------------------------------
 if(DEBUG1) {
  Serial.print("DS1307 set time: ");
  if(ret==0) Serial.println("OK");
  else { 
   if(ret==-1) Serial.println("ERROR: No date+time from server!");
   else { Serial.print("ERROR # ");  Serial.println(ret); }
  }
 }
 return sDT;
}

void setup() {
 //-----init Serial, I/O----------------------------------------
 Serial.begin(115200); Serial.println();    // init serial comm
 blueled.on();                              // start: led on
 ain.setRefPoints(7, 0.0, 995, 3.29);       // new ref values
 //-----sd card-------------------------------------------------
 if(SDfile_init)
 {
  Serial.println("SD card not ready!");
  while(1) { blueled.toggle(); delay(250); };
 }
 file1.remove();                            // delete old file
 //-----set time------------------------------------------------
 setHttpTime();
}

int i=0, imax=5;                       // measurement counter
void loop()
{
 //-----measurements--------------------------------------------
 if(i<imax)
 {
  String sAin=ain.getsVoltage();       // read analog in
  String dt1=ds1307.getDateTime2();    // dd.mm.yyyy HH:MM:SS
  file1.addLine(dt1+" "+sAin+" V");    // write file
  i++;                                 // increment counter
  Serial.print(i);Serial.println(": "+sAin+" V");
 }
 //-----after last measurement----------------------------------
 if(i==imax)
 {
  blueled.off();                       // finished: led off
  Serial.println("\n--Measurement finished - File content:--");
  file1.show();                        // show measured values
  i++;                                 // last increment counter
 } 
 delay(2000);                          // wait 2 secs
}


//_____D1_oop23_DS1307_setTimeHttp.ino________171126-171126_____
// Demo for class TimeHttp: Set date and time of a DS1307 RTC
// by sending a HTTP 1.1 request (port 80) to a server with
// missing "Host: " in header and waiting for an answer.
// Using this program:
// * Send character s by Serial sets date+time by HTTP-Request
// * Send character r by Serial resets date+time to sDateTime0
// All results are printed to Serial (115200 Baud).
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
// Hardware: (1) WeMos D1 mini
//           (2) Data logger Shield (microSD+DS1307RTC-Shield)
#include "D1_class_DS1307.h"
#include "D1_class_TimeHttp.h"
DS1307 ds1307();                         // object DS1307 clock
TimeHttp time1("..ssid..", "..password..","httpservername");

#define TIMEZONE 1
String sDateTime0="20161231 235954";
String sInfo="Serial: s sends HTTP-Request, r resets time";

//_____set time________________________________________________
String setHttpTime()
{
 int ret=-1;
 String sDT="";
 Serial.println("Starting a HTTP-request...");
 sDT=time1.getDateTime();
 if(sDT!="")
 {
  int st=time1.getSummertime(sDT, TIMEZONE);
  if(st>0) Serial.print("Summertime.  ");
      else Serial.print("Normal time. ");
  sDT=time1.addsubHours(sDT, st+TIMEZONE);
  ret=ds1307.setDateTime(sDT);
 }
 Serial.print("DS1307 set time: ");
 if(ret==0) Serial.println("OK");
 else { Serial.print("ERROR # ");  Serial.println(ret); }
 return sDT;
}

void setup() {
 Serial.begin(115200);Serial.println(); // init serial comm
 //-----is DS1307 present?--------------------------------------
 Serial.print("RTC DS1307: ");
 int status=ds1307.getStatus();
 if(status) Serial.println("Error "+String(status));
 else Serial.println("OK");
 //-----set time------------------------------------------------
 String s1=setHttpTime();
 if(s1!=""){ Serial.println("Setup: setHttpTime = "+s1); }
 else Serial.println("ERROR while setHttpTime()");
}

void loop()
{
 //-----Serial: char available?--------------------------------- 
 if(Serial.available())
 {
  char c1=Serial.read();
  if(c1=='r') {
   int ret=ds1307.setDateTime(sDateTime0); 
   if(ret==0) Serial.println("DS1307 reset time: OK");
   else Serial.println("DS1307 reset time: ERROR");
  }
  if(c1=='s') setHttpTime();
  Serial.flush();  
 }
 //-----get date and time from DS1307 and show it---------------
 String s1=ds1307.getDateTime2();       // DS1307 request
 Serial.print(ds1307.getLastWeekday()); // no DS1307 request
 Serial.println(" "+s1+" | "+sInfo);
 delay(2000);
}

//_____D1_oop26_checkDatalogger1.ino__________171208-171222_____
// Check Data Logger Shield (microSD+DS1307RTC-Shield):
// * A short keystroke on D3 starts measuring, blue led is on:
//   + read analog input value
//   + display value on oled
//   + read date and time from RTC DS1307
//   + write value with date and time to file "analogin.txt"
//     on microSD card
//   + wait 2 sec
// * Another short keystroke on D3 stopps measuring and turns 
//   blue led is off.
// * a long keystroke on D3 starts time synchronisation:
//   Try to connect to WLAN, fetch actual time from a
//   server and write it to RTC
// Results are printed on Serial (115200 Baud).
//
// Hardware: (1) WeMos D1 mini
//           (2) Data logger shield (microSD + DS1307RTC-Shield)
//           (3) analog input values
//           (4) button shield D3
//           (5) oled shield
//
// Created by Karl Hartinger, December 08, 2017.
// Released into the public domain.
#define DEBUG1               true
#define OLED_RESET           D0        // OLED_RESET=D0
#define TIMEZONE             1         // MEZ,Central Europ.Time
#include "D1_class_Din.h"              // button D3
#include "D1_class_Dout.h"             // switch blue led
#include "D1_class_Ain.h"              // get analog in value
#include "D1_class_DisplayKH.h"
#include "D1_class_DS1307.h"           // real time clock
#include "D1_class_TimeHttp.h"         // get time from a server
#include "D1_class_SDfile.h"           // micro SD card

//TimeHttp time1("..ssid..", "..password..","httpservername");
Din       button;                      // button D3
Dout      blueled(true);               // invert! on=0V
Ain       ain;                         // object analog in
DisplayKH display_(OLED_RESET);        // OLED
DS1307    ds1307;                      // object DS1307 clock
SDfile    file1("analogin.txt");       // data file

//_____set time_________________________________________________
String setHttpTime()
{
 //-----try to get date and time from server--------------------
 int ret=-1, summertime=0;
 String sDT="", sDTold;
 if(DEBUG1) Serial.println("Starting a HTTP-request...");
 sDT=time1.getDateTime();
 //-----if values available: try to set time--------------------
 if(sDT!="") {
  summertime=time1.getSummertime(sDT, TIMEZONE);
  sDT=time1.addsubHours(sDT, summertime+TIMEZONE);
  sDTold=ds1307.getDateTime();
  ret=ds1307.setDateTime(sDT);
 }
 //-----print result to Serial----------------------------------
 if(DEBUG1) {
  Serial.print("DS1307 time old="+sDTold+", new=");
  if(ret==0) Serial.println(sDT);
  else { 
   if(ret==-1) 
    Serial.println(" ERROR: No date+time from server!");
   else
   { Serial.print(" ERROR # ");  Serial.println(ret); }
  }
 }
 return sDT;
}

//_____oled: display start lines________________________________
void display_start() {
 display_.screen112(2,"START",'c'); // + update oled
 display_.screen112(3,"Stopp: D3");// +
}

//_____oled: display stopp lines________________________________
void display_stopp() {
 display_.screen112(2,"STOP",'c');  // + update oled
 display_.screen112(3,"Start: D3");// +
}

void setup() {
 //-----init Serial, I/O----------------------------------------
 Serial.begin(115200);Serial.println();// init serial comm
 Serial.println("Start: Press D3!");   // start message
 blueled.off();                        // no measuring, led off
 ain.setRefPoints(7, 0.0, 995, 3.29);  // new ref values
 display_.clearDisplay();              // +
 display_.screen112(1,"Datalogger");   // |
 display_.screen112(2,"READY",'c');    // | write to oled
 display_.screen112(3,"Press D3");    // +
 display_.screen112(4,"V171208 KH");   // |
 //-----check for sd card---------------------------------------
 if(SDfile_init)
 {
  display_.screen112(2,"ERROR");       // +
  display_.screen112(3,"SD card");     // | write to oled
  display_.screen112(4,"not ready!");  // +
  Serial.println("SD card not ready!");// 
  while(1) { blueled.toggle(); delay(250); };    // endless loop
 }
 file1.remove();                       // delete old file
}

int  i=0, imax=20;                     // measurement counter
bool bMeasure=false;                   // true=do measuring
int  sync=0;                           // 

void loop()
{
 //-----check button D3, start or stop measuring----------------
 if(button.isClicked())
 {
  bMeasure=!bMeasure;
  if(bMeasure)
  {
   blueled.on();                      // start measuring:led on
   display_start();
   file1.remove();                    // delete old measuring
  }
  else
  {
   blueled.off();                     // finished: led off
   display_stopp();
   Serial.println();
   Serial.println("-----File content----------");
   file1.show();                      // show measured values
   Serial.println("---------------------------");
   Serial.println();
  }
 }
 //-----synchronise time?---------------------------------------
 if(button.isPressed(1000))
 {
  display_.screen112(2,"SYNC",'c');   // update oled
  display_.screen112(3,"wait..."); // 
  if(setHttpTime()=="")
  {
   display_.screen112(3,"failed..."); // 
   delay(2000);
  }
  if(bMeasure) display_start();
          else display_stopp();
 }
 else
  sync=0;
 //-----measuring?----------------------------------------------
 if(bMeasure && (i==0)) 
 {
  String sAin=ain.getsVoltage();       // read analog in
  String dt1=ds1307.getDateTime2();    // dd.mm.yyyy HH:MM:SS
  if(dt1=="")
  {
   Serial.print("DS1307: Error #");
   Serial.println(ds1307.getStatus());
  }
  String line_=dt1+" "+sAin+" V";
  file1.addLine(line_);                // write file
  Serial.println(line_);               // write serial
  display_.screen112(2,sAin);          // update oled
 }
 i++; if(i>=imax) i=0;                 // increment counter
 delay(100);                           // wait 0,2 secs
}

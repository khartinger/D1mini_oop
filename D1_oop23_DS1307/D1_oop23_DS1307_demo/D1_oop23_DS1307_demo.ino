//_____D1_oop23_DS1307_demo.ino_______________171125-171125_____
// Example for use of class DS1307 to exchange data with 
// real time clock (RTC) via i2c interface.
// * check for i2c answer from device (ask for its address)
// * set date and time values shortly before "midnight"
// * clear RAM (with default value 0) and print result
// * write a byte and a int to RAM, read them and print result
// * read weekday, date, time and secs from 1.1.1970 from RTC
//   every 2 seconds for 5 times 
// * read last values without RTC access (just for check)
// All results are printed to Serial (115200 Baud).
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
// Hardware: (1) WeMos D1 mini
//           (2) microSD+DS1307RTC-Shield
#define  TIMEZONE 1
#include "D1_class_DS1307.h"
DS1307 ds1307;                         // object DS1307 clock

byte hh=23, mm=59, ss=56;              // test data for time
String date="20171125";                // test data for date
unsigned long secs=1511654396L;        // = 20171125 235956
int status;

void setup() {
 Serial.begin(115200);                 // init serial comm
 Serial.println();                     // next line
 Serial.println("class DS1307 demo:"); //
 //-----is DS1307 present?--------------------------------------
 int address = ds1307.getAddress();    // i2c address of DS1307
 int status  = ds1307.getStatus();     // reading address ok?
 Serial.print("i2c-status for 0x");    // + show
 Serial.print(address,HEX);            // | address
 Serial.print(" is ");                 // | and
 if(status==0) Serial.println("OK");   // | status OK
          else Serial.println(status); // + or error
 //-----set date and time---------------------------------------
 String s1;
 //status=ds1307.setSecs1970(secs);
 status=ds1307.setDate(date);          // (pre)set date
 if(status!=0) Serial.println("Fehler beim Datum-Setzen!");
 s1=ds1307.time2string(hh,mm,ss);
 status=ds1307.setTime(s1);            // (pre)set time
 if(status!=0) Serial.println("Fehler beim Zeit-Setzen!");
 if(ds1307.isSummertime(TIMEZONE)) Serial.println("Sommerzeit!");
 else Serial.println("Winterzeit!");
 //-----Test RAM------------------------------------------------
 if(ds1307.clearRAM()==0)  Serial.println("RAM clearing ok.");
 else  Serial.println("RAM error clearing ok.");
 byte b=0xAA;
 ds1307.setRAMbyte(63,b);
 if(ds1307.getRAMbyte(63)==b) Serial.println("RAM set byte ok.");
 else Serial.println("RAM set byte error.");
 int i=-32000;
 ds1307.setRAMint16(62,i);
 if(ds1307.getRAMint16(62)==i) Serial.println("RAM set int16 ok.");
 else Serial.println("RAM set int16 error.");
}

int i=0, imax=5;
void loop()
{
 if(i<imax)
 {
  i++;
  Serial.println("-----Data from DS1307----------------------");
  Serial.print(ds1307.getWeekday()+" ");
  Serial.print(ds1307.getDate2()+" ");
  Serial.print(ds1307.getTime2()+" = ");
  Serial.print(ds1307.getDateTime()+" = ");
  secs=ds1307.getSecs1970();
  Serial.print(secs/10); Serial.println(secs%10);
  //-----read last values without DS1307 access-----------------
  Serial.println("Last values (should be the same):");
  Serial.print(ds1307.getLastWeekday()+" ");
  Serial.print(ds1307.getLastDate2()+" ");
  Serial.print(ds1307.getLastTime2()+" = ");
  Serial.print(ds1307.getLastDateTime()+" = ");
  secs=ds1307.getLastSecs1970();
  Serial.print(secs/10); Serial.println(secs%10);
  
  delay(2000);
 }
 if(i==imax) { i++; Serial.println("Finished :)"); }
}


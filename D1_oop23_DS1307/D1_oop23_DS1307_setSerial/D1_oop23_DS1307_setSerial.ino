//_____D1_oop23_DS1307_setSerial.ino__________171125-171125_____
// Set date and time of a DS1307 RTC by Serial and show values.
// When using Arduino-IDE: Don't forget to change drop down menu
// to "new line" on the lower right of the serial monitor!
// All results are printed to Serial (115200 Baud).
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
// Hardware: (1) WeMos D1 mini
//           (2) Data logger Shield (microSD+DS1307RTC-Shield)
#include "D1_class_DS1307.h"
DS1307 ds1307;                         // object DS1307 clock
String sInfo=" | Set time by sending T_yyyymmdd HHMMSS";

void setup() {
 Serial.begin(115200);Serial.println();// init serial comm
 //-----is DS1307 present?--------------------------------------
 Serial.print("RTS DS1307: ");
 int status=ds1307.getStatus();
 if(status) Serial.println("Error "+String(status));
 else Serial.println("OK");
 Serial.println("Arduino-IDE: Don't forget to set drop down ");
 Serial.println("menu on lower right to \"New line\" :-)");
}

void loop()
{
 if(Serial.available())
 {
  String s1=Serial.readStringUntil('\n');     // read string
  if((s1.length()==17)&&(s1.substring(0,2)=="T_"))
  {
   //-----set date----------------------------------------------
   int status=ds1307.setDate(s1.substring(2,10));
   Serial.print("Set date: ");
   if(status) Serial.println("Error "+String(status));
   else Serial.println("OK");
   //-----set time----------------------------------------------
   status=ds1307.setTime(s1.substring(11));
   Serial.print("Set time: ");
   if(status) Serial.println("Error "+String(status));
   else Serial.println("OK");
  }
  Serial.flush();
 }
 //-----print date and time to Serial---------------------------
 Serial.print(ds1307.getWeekday()+" ");
 Serial.print(ds1307.getDate2()+" ");
 Serial.print(ds1307.getTime2()+" ");
 Serial.println(sInfo);
 delay(2000);
}


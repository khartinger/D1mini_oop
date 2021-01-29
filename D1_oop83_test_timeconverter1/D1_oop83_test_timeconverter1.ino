//_____D1_oop83_test_timeconverter1.ino_______210129_210129_____
// This program for a D1 mini (or ESP32 D1mini) tests methods
// of the class timeconverter1 and shows the result
//  on Serial (115200Bd)
// Hardware:
// (1) WeMos D1 mini (OR ESP32 D1 mini)
//
// Created by Karl Hartinger, January 29, 2021.
// Changes:
// 2021-01-29 First release
// Released into the public domain.

#include <Arduino.h>
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/timeconverter1/D1_class_Timeconverter1.h"

//_______sensors, actors, global variables______________________
Timeconverter1  myTC;                       // time + format

//_______setup Serial, test methods of class timeconverter1_____
void setup() {
 String sYmd="20210131";
 String sHMS="010203";
 time_t secEx=1612054923;
 time_t secTC;
 String sDT;
 int yyyy,mm,dd,HH,MM,SS;
 //------Serial-------------------------------------------------
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //-----set date and time---------------------------------------
 myTC.setDate(sYmd);
 myTC.setTimeHMS(sHMS);
 //------show date and time-------------------------------------
 Serial.println("-----Date and time should be "+sYmd+" "+sHMS+"---");
 Serial.print("Seconds since 1970-01-01: ");
 secTC=myTC.getSec1970();
 Serial.print(secTC);
 if(secTC==secEx) Serial.println(" (OK)");
 else {Serial.print("Error! Should be "); Serial.println(secEx); }
 Serial.print("getYmdHMS():  ");
 Serial.println(myTC.getYmdHMS());
 Serial.print("getYmdHMS_(): ");
 Serial.println(myTC.getYmdHMS_());
 Serial.print("getTmJHMS_(): ");
 Serial.println(myTC.getTmJHMS_());
 Serial.println("-----show date only-------------------------");
 Serial.print("getYmd():     ");
 Serial.println(myTC.getYmd());
 myTC.getYmd(yyyy,mm,dd);
 Serial.printf("getYmd():     yyyy=%d mm=%d dd=%d\n",yyyy,mm,dd);
 Serial.print("getYmd_():    ");
 Serial.println(myTC.getYmd_());
 Serial.print("getTmJ_():    ");
 Serial.println(myTC.getTmJ_());
 Serial.println("-----show time only-------------------------");
 Serial.print("getHMS():     ");
 Serial.println(myTC.getHMS());
 myTC.getHMS(HH,MM,SS);
 Serial.printf("getHMS():     HH=%d MM=%d SS=%d\n",HH,MM,SS);
 Serial.print("getHMS_():    ");
 Serial.println(myTC.getHMS_());
 Serial.print("getHM_():     ");
 Serial.println(myTC.getHM_());
 Serial.println("-----working methods------------------------");
 Serial.println("-----add 29 days => 2021-03-01--------------");
 myTC.addDays(29);
 Serial.print("getYmd_():    ");
 Serial.println(myTC.getYmd_());
 time_t dday=myTC.daydiff(sYmd);
 Serial.printf("Difference in Days    : %ld\n", dday);
 Timeconverter1 myTC2=Timeconverter1(sYmd,"000000");
 dday=myTC.daydiff(myTC2);
 Serial.printf("Difference in Days TC2: %ld\n", dday);
 Serial.println("-----add 82676 seconds => 23:59:59----------");
 myTC.addSecs(82676);                       // 23:59:59-01:02:03
 Serial.print("getYmdHMS_(): ");
 Serial.println(myTC.getYmdHMS_());
 Serial.println("-----add 1 second => 2021-03-02 00:00:00----");
 myTC.addSecs(1);
 Serial.print("getYmdHMS_(): ");
 Serial.println(myTC.getYmdHMS_());
 Serial.print("getWeekday (DI): ");
 Serial.println(myTC.getWeekday());

 Serial.println("-----summertime-----------------------------");
//29.03.2020, 02:00 MEZ – 25.10.2020, 03:00 MESZ
//28.03.2021, 02:00 MEZ – 31.10.2021, 03:00 MESZ
//27.03.2022, 02:00 MEZ – 30.10.2022, 03:00 MESZ
 //......Winter -> Summer 2021..................................
 sDT="20210328 015959";
 secEx=1616896799;
 myTC.set(sDT);
 secTC=myTC.getSec1970();
 Serial.print(secTC);
 if(secEx==secTC) Serial.print(" (OK) = ");
             else Serial.print(" (ERROR) = ");
 Serial.print(myTC.getYmdHMS()+" (MEZ)  is ");
 if(myTC.isSummertime()) Serial.println("MESZ (ERROR)");
                    else Serial.println("MEZ  (OK)");
 myTC.addSecs(1);
 secEx++;
 secTC=myTC.getSec1970();
 Serial.print(secTC);
 if(secEx==secTC) Serial.print(" (OK) = ");
             else Serial.print(" (ERROR) = ");
 Serial.print(myTC.getYmdHMS()+" (MESZ) is ");
 if(myTC.isSummertime()) Serial.println("MESZ (OK)");
                    else Serial.println("MEZ  (ERROR)");
 myTC.addSecs(1);
 secTC=myTC.getSec1970();
 Serial.print(secTC); Serial.print("      = ");
 Serial.print(myTC.getYmdHMS()+" (MESZ) is ");
 if(myTC.isSummertime()) Serial.println("MESZ (OK)");
                    else Serial.println("MEZ  (ERROR)");
 //......Summer-> Winter 2021..................................
 sDT="20211031 025959";
 secEx=1635649199;
 myTC.set(sDT);
 secTC=myTC.getSec1970();
 Serial.print(secTC);
 if(secEx==secTC) Serial.print(" (OK) = ");
             else Serial.print(" (ERROR) = ");
 Serial.print(myTC.getYmdHMS()+" (MESZ) is ");
 if(myTC.isSummertime()) Serial.println("MESZ (OK)");
                    else Serial.println("MEZ  (ERROR)");
 myTC.addSecs(1);
 secEx++;
secTC=myTC.getSec1970();
 Serial.print(secTC);
 if(secEx==secTC) Serial.print(" (OK) = ");
             else Serial.print(" (ERROR) = ");
 Serial.print(myTC.getYmdHMS()+" (MEZ)  is ");
 if(myTC.isSummertime()) Serial.println("MESZ (ERROR)");
                    else Serial.println("MEZ  (OK)");
 myTC.addSecs(1);
 secTC=myTC.getSec1970();
 Serial.print(secTC); Serial.print("      = ");
 Serial.print(myTC.getYmdHMS()+" (MEZ)  is ");
 if(myTC.isSummertime()) Serial.println("MESZ (ERROR)");
                    else Serial.println("MEZ  (OK)");
 

//------setup finish-------------------------------------------
 Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
}
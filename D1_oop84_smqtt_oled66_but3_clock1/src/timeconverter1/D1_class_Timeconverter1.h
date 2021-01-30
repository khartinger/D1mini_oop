//_____D1_class_Timeconverter1.h______________210129-210129_____
// The class Timeconverter can be used to convert seconds since 
// 1970-01-01 (Unix time) to date & time and vice versa.
// Created by Karl Hartinger, January 29, 2021.
// Changes:
// 2021-01-29 First release
// Hardware: D1 mini OR ESP32 D1mini
// Released into the public domain.

#ifndef D1_CLASS_TIMECONVERTER1_H
#define D1_CLASS_TIMECONVERTER1CONVERTER_H
#include <Arduino.h>                   // D1, ...
#include <time.h>                      // time_t, ...

class Timeconverter1 {
 protected:
 //------properties---------------------------------------------
  time_t sec1970_;                     // time of the object
  int    timezone_;                    // UTC=0, MEZ=1
 public:
 //------constructor & co---------------------------------------
  Timeconverter1();
  Timeconverter1(time_t sec1970);
  Timeconverter1(const String sYmd, const String sHMS);
protected:
 //_____initialize all properties. Called by constructor(s)____
  void   setup();                      // called by constructors
 public:
 //------setter methods: date and time--------------------------
  void   set(time_t sec1970);
  bool   set(const String sYmdHMS);
  bool   set(const String sYmd, const String sHMS);
  void   setTimezone(int timezone);    // UTC=0, MEZ=1
 //------setter methods: date-----------------------------------
  bool   setDate(const String sYmd);
  bool   setDate(int yyyy, int mm, int dd);
 //------setter methods: time-----------------------------------
  bool   setTimeHM(String sHM);        // HH:MM
  bool   setTimeHMS(String sHMS);      // HH:MM:SS

 //------getter methods: date and time--------------------------
  time_t getSec1970();                 // seconds since 1970-01-01
  String getYmdHMS();                  // yyyymmdd HHMMSS
  String getYmdHMS_();                 // yyyy-mm-dd HH:MM:SS
  String getTmJHMS_();                 // tt.mm.JJJJ HH:MM:SS
  int    getTimezone();                // UTC=0, MEZ=1
 //------getter methods: date-----------------------------------
  String getYmd();                     // yyyymmdd year month day
  void   getYmd(int& year, int& mon, int& day);
  String getYmd_();                    // yyyy-mm-dd year month day
  String getTmJ_();                    // TT.MM.JJJJ
 //------getter methods: time-----------------------------------
  String getHMS();                     // HHMMSS
  void   getHMS(int& HH, int& MM, int& SS);
  String getHMS_();                    // HH:MM:SS
  String getHM_();                     // HM       hour:minute

 //-------working methods----------------------------------------
  time_t daydiff(String Ymd);          // return days=Ymd-this
  time_t daydiff(Timeconverter1 t2);   // return days=t2-this
  void   addSecs(long ss);             // add seconds to this
  void   addDays(time_t dd);           // add days to this
  bool   isSummertime();               // is this Summertime?
  bool   isSummertime(int yyyy, int mm,
         int dd, int HH, int timezone);
  String getWeekday();                 // SO, MO, DI, MI...
  
 //------helper methods-----------------------------------------
  time_t YmdToSec(const String sYmd);  // YYYYmmdd or YYYY-mm-dd
  time_t YmdToSec(int yyyy, int mm, int dd);
  int    getiDayOfWeek(int yyyy, byte mm, byte dd);
  bool   isLeapyear(int year);
};
#endif // D1_CLASS_TIMECONVERTER1_H

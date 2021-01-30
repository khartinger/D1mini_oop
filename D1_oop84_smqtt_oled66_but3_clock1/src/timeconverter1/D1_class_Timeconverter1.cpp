//_____D1_class_Timeconverter1.cpp____________210129-210129_____
// The class Timeconverter can be used to convert seconds since 
// 1970-01-01 (Unix time) to date & time and vice versa.
// Created by Karl Hartinger, January 29, 2021.
// Changes:
// 2021-01-29 First release
// Hardware: D1 mini OR ESP32 D1mini
// Released into the public domain.

#include "D1_class_Timeconverter1.h"

// *************************************************************
//       constructor & co
// *************************************************************

//_______constructor____________________________________________
Timeconverter1::Timeconverter1() {
 setup();
}

//_______constructor____________________________________________
Timeconverter1::Timeconverter1(time_t sec1970){
 setup();
 set(sec1970);
}

//_______constructor____________________________________________
Timeconverter1::Timeconverter1(const String sYmd, const String sHMS){
 setup();
 set(sYmd, sHMS);
}

//_______setup__________________________________________________
void Timeconverter1::setup() {
 sec1970_=0;
 timezone_=1;                          // UTC=0, MEZ=1
}

// *************************************************************
//       Setter methods: date and time
// *************************************************************

//_______set seconds since 1979-01-01___________________________
void Timeconverter1::set(time_t sec1970) { sec1970_ = sec1970; }

//_______set date and time with format YYYYmmdd HHMMSS__________
bool Timeconverter1::set(const String sYmdHMS) {
  return set(sYmdHMS.substring(0,8), sYmdHMS.substring(9));
}

//_______set date and time as string____________________________
bool Timeconverter1::set(const String sYmd, const String sHMS) {
 bool ret=setDate(sYmd);
 ret&=setTimeHMS(sHMS);
 return ret;
}

//_______set timezone (UTC=0, MEZ=1)____________________________
void Timeconverter1::setTimezone(int timezone) {
 timezone_=timezone;
}

// *************************************************************
//       Setter methods: date
// *************************************************************

//_______set year month and day, format YYYYmmdd or YYYY-mm-dd__
// time is set to 00:00:00
// return: true = seconds set
bool Timeconverter1::setDate(const String sYmd) {
 time_t sec1=YmdToSec(sYmd);
 if(sec1<0) return false;
 sec1970_=sec1;
 return true;
}

//_______set year month and day_________________________________
bool Timeconverter1::setDate(int yyyy, int mm, int dd) {
 time_t sec1=YmdToSec(yyyy,mm,dd);
 if(sec1<0) return false;
 sec1970_=sec1;
 return true;
}

// *************************************************************
//       Setter methods: time
// *************************************************************

//_______set time HHMM or HH:MM to actual date__________________
// return: true=time set, false: no change of time
bool Timeconverter1::setTimeHM(String sHM) {
 String sHMS;
 if(sHM.length()==4)
 {
  sHMS=sHM;
 }
 {
  if(sHM.length()!=5) return false;
  if(sHM[2]!=':') return false;
  sHMS=sHM.substring(0,2)+sHM.substring(3);
 }
 sHMS+="00";
 return setTimeHMS(sHMS);
}

//_______set time HHMMSS or HH:MM:SS to actual date____________
// return: true=time set, false: no change of time
bool Timeconverter1::setTimeHMS(String sHMS) {
 //bool   ret=true, ret2;
 //time_t s_=0;
 //int    temp1, temp0, temp;
 int h1,h0,m1,m0,s1,s0, h_, m_, s_;
 if(sHMS.length()==6)
 {//-----HHMMSS-------------------------------------------------
  h1=sHMS[0]-'0';
  h0=sHMS[1]-'0';
  m1=sHMS[2]-'0';
  m0=sHMS[3]-'0';
  s1=sHMS[4]-'0';
  s0=sHMS[5]-'0';
 }
 else
 {//-----HH:MM:SS-----------------------------------------------
  if(sHMS.length()!=8) return false;
  if((sHMS[2]!=':')||(sHMS[5]!=':')) return false;
  h1=sHMS[0]-'0';
  h0=sHMS[1]-'0';
  m1=sHMS[3]-'0';
  m0=sHMS[4]-'0';
  s1=sHMS[6]-'0';
  s0=sHMS[7]-'0';
 }
 //------calculate hour, minute and second value----------------
 h_=10*h1+h0;
 m_=10*m1+m0;
 s_=10*s1+s0;
 //------check digits and values--------------------------------
 if(h1<0 || h1>2 || h0<0 || h0>9 || h_<0 || h_>23) return false;
 if(m1<0 || m1>5 || m0<0 || m0>9 || m_<0 || m_>59) return false;
 if(s1<0 || s1>5 || s0<0 || s0>9 || s_<0 || s_>59) return false;
 //------set hour, minute and second value----------------------
 struct tm* tm1_=localtime(&sec1970_);
 tm1_->tm_hour=h_;
 tm1_->tm_min=m_;
 tm1_->tm_sec=s_;
 sec1970_=mktime(tm1_);
 return true;
}

// *************************************************************
//       Getter methods: date and time
// *************************************************************

//_______return seconds since 1.1.1970__________________________
time_t Timeconverter1::getSec1970() { return sec1970_; }

//_______get string date and time as yyyymmdd HHMMSS____________
String Timeconverter1::getYmdHMS() {
 char sc[16];
 String s1="00000000 000000";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,16,"%Y%m%d %H%M%S\0",tm1_);
 if(ret==15) s1=sc;
 return s1;
}

//_______get string date and time as yyyy-mm-dd HH:MM:SS________
String Timeconverter1::getYmdHMS_() {
 char sc[20];
 String s1="0000-00-00 00:00:00";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,20,"%Y-%m-%d %H:%M:%S\0",tm1_);
 if(ret==19) s1=sc;
 return s1;
}

//_______get string date and time as tt.mm.JJJJ HH:MM:SS________
String Timeconverter1::getTmJHMS_() {
 char sc[20];
 String s1="00.00.0000 00:00:00";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,20,"%d.%m.%Y %H:%M:%S\0",tm1_);
 if(ret==19) s1=sc;
 return s1;
}

//_______timezone (UTC=0, MEZ=1)________________________________
int Timeconverter1::getTimezone() { return timezone_; }

// *************************************************************
//       Getter methods: date
// *************************************************************

//_______convert time to yyyymmdd_______________________________
String Timeconverter1::getYmd() {
 char sc[12];
 String s1="";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,11,"%Y%m%d\0",tm1_);
 if(ret==8) s1=sc;
 return s1;
}

//_______get year, month and day of date________________________
void Timeconverter1::getYmd(int& year, int& mon, int& day) {
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 year=1900+tm1_->tm_year;
 mon=1+tm1_->tm_mon;
 day=tm1_->tm_mday;
}

//_______convert time to yyyy-mm-dd_____________________________
String Timeconverter1::getYmd_() {
 char sc[14];
 String s1="";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,11,"%Y-%m-%d\0",tm1_);
 if(ret==10) s1=sc;
 return s1;
}

//_______convert time to tt.mm.JJJJ_____________________________
String Timeconverter1::getTmJ_() {
 char sc[14];
 String s1="";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,11,"%d.%m.%Y\0",tm1_);
 if(ret==10) s1=sc;
 return s1;
}

// *************************************************************
//       Getter methods: time
// *************************************************************

//_______get hhmmss (hour+minute+seconds)_______________________
String Timeconverter1::getHMS() {
 char sc[7];
 String s1="";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,7,"%H%M%S\0",tm1_);
 if(ret==6) s1=sc;
 return s1;
}

//_______get hour, minute and second of this time_______________
void Timeconverter1::getHMS(int& HH, int& MM, int& SS) {
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 HH=tm1_->tm_hour;
 MM=tm1_->tm_min;
 SS=tm1_->tm_sec;
}

//_______get hh:mm:ss (hour:minute:seconds)_____________________
String Timeconverter1::getHMS_() {
 char sc[9];
 String s1="";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,9,"%H:%M:%S\0",tm1_);
 if(ret==8) s1=sc;
 return s1;
}

//_______get hour:minute________________________________________
String Timeconverter1::getHM_() {
 char sc[7];
 String s1="";
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime!
 int ret=strftime(sc,6,"%H:%M\0",tm1_);
 if(ret==5) s1=sc;
 return s1;
}

// *************************************************************
//       working methods
// *************************************************************

//_______number of days between sYmd and this___________________
// return days = sYmd - this
time_t Timeconverter1::daydiff(String sYmd)
{
 struct tm* tm1_=localtime(&sec1970_);
 tm1_->tm_hour=0;
 tm1_->tm_min=0;
 tm1_->tm_sec=0;
 time_t sec1=mktime(tm1_);
 time_t sec2=YmdToSec(sYmd);
 time_t days=(sec2-sec1)/86400;
 return days;
}

//_______number of days between t2 and this_____________________
// return day s =t2 - this
time_t Timeconverter1::daydiff(Timeconverter1 t2)
{
 struct tm* tm1_=localtime(&sec1970_);
 tm1_->tm_hour=0;
 tm1_->tm_min=0;
 tm1_->tm_sec=0;
 time_t sec1=mktime(tm1_);
 time_t sec2=t2.YmdToSec(t2.getYmd());
 time_t days=(sec2-sec1)/86400;
 return days;
}

//_______add seconds to this____________________________________
void Timeconverter1::addSecs(long ss) { sec1970_+=ss; }

//_______add days to this_______________________________________
void Timeconverter1::addDays(time_t dd)
{
 struct tm* tm1_=localtime(&sec1970_);  // MUST be localtime
 tm1_->tm_mday=tm1_->tm_mday+dd;
 sec1970_=mktime(tm1_);
}

//_______is this date+time Daylight Savings Time?_______________
bool Timeconverter1::isSummertime() {
  int yyyy, mm, dd, HH, MM, SS;
  getYmd(yyyy, mm, dd);
  getHMS(HH, MM, SS);
  return isSummertime(yyyy,mm,dd,HH,timezone_);
}

//_______European Daylight Savings Time (DST) calculation_______
// input : year, month, day, hour and timezone (0=UTC, 1=MEZ)
// uses  : getDayOfWeek(), isLeapYear()
// return: true = daylight saving time, false = normal time
// timezone: UTC=0, MEZ=1
bool Timeconverter1::isSummertime(int yyyy, int mm,
     int dd, int HH, int timezone)
{
 int hhGiven;                          // hours since 1. of month
 int hhUTC;                            // hours to start/end DST
 int ddBegin;                          // day of last Sunday March
 int ddEnd;                            // day of last Sunday Oct
 if(mm<3 || mm>10) return false;       // no DST Jan,Feb,Nov,Dez
 if(mm>3 && mm<10) return true;        // DST in Apr...Sep
 //------March--------------------------------------------------
 if(mm==3)
 {
  ddBegin=31-getiDayOfWeek(yyyy,3,31); //last Sunday in March
  //.....hours since 1.3.yyyy 00:00:00 (UTC)....................
  hhGiven=24*dd+HH-timezone;           // MEZ -> UTC
  hhUTC=24*ddBegin+1;                  // UTC: 01:00 -> 02:00
  //Serial.printf("| isSummertime(): last SO in Maerz: %d, hhGiven=%d, hhUTC=%d | ", ddBegin, hhGiven, hhUTC);
  if(hhGiven >= hhUTC) return true;
  return false;
 }
 //------October------------------------------------------------
 ddEnd=31-getiDayOfWeek(yyyy,10,31);   //last Sunday in Oct
 //.....hours since 1.10.yyyy 00:00:00 (UTC)....................
 hhGiven=24*dd+HH-timezone;            // MEZ -> UTC
 hhUTC=24*ddEnd+1;                     // UTC: 02:00 -> 01:00
 //Serial.printf("| isSummertime(): last SO in Okt: %d, hhGiven=%d, hhUTC=%d | ", ddEnd, hhGiven, hhUTC);
 if(hhGiven <= hhUTC) return true;
 return false;
}

//_______get weekday____________________________________________
String Timeconverter1::getWeekday()
{
  String s1[]={"SO","MO","DI","MI","DO","FR","SA"};
  int yyyy,mm,dd;
  getYmd(yyyy,mm,dd);
  return s1[getiDayOfWeek(yyyy, mm, dd)];
}

// *************************************************************
//       helper methods
// *************************************************************

//_______convert string Ymd 00:00:00 to sec1970_________________
// Ymd may be YYYYmmdd or YYYY-mm-dd
// return: seconds since 1.1.1970 or -1 on error
time_t Timeconverter1::YmdToSec(const String sYmd)
{
 String sYmd1, sYmd2;
 String sYear, sMon, sDay;
 //------parse date string--------------------------------------
 if(sYmd.length()==8)
 {
  sYear=sYmd.substring(0,4);
  sMon=sYmd.substring(4,4+2);
  sDay=sYmd.substring(6);
 }
 else
 {
  if(sYmd.length()!=10) return -1;
  if((sYmd[4]!='-')||(sYmd[7]!='-')) return -1;
  sYear=sYmd.substring(0,4);
  sMon=sYmd.substring(5,5+2);
  sDay=sYmd.substring(8);
 }
 sYmd1=sYear+sMon+sDay;
 //------onvert parts of string to year, month, day-------------
 int year_, mon_, day_;
 year_=sYear.toInt();
 if((year_<1970)||(year_>9999)) return -1;
 mon_=sMon.toInt();
 if((mon_<1)||(mon_>12)) return -1;
 day_=sDay.toInt();
 if((day_<1)||(day_>31)) return -1;
 //------calculate seconds for date1 12:00:00 from 1.1.1970-----
 return YmdToSec(year_, mon_, day_);
}

//_______convert yyyy, mm, dd 00:00:00 to sec1970_______________
// return: seconds since 1.1.1970 or -1 on error
time_t Timeconverter1::YmdToSec(int yyyy, int mm, int dd)
{
 time_t tmp[]={31,28,31,30,31,30,31,31,30,31,30,31};
 int dd2=28;
 if((yyyy<1970)||(yyyy>9999)) return -1;
 if((mm<1)||(mm>12)) return -1;
 if(dd<1) return -1;
 if(mm==2)
 {
  if((yyyy%4==0 && yyyy%100) || yyyy%400==0) dd2=29;
  if(dd>dd2) return -1;
 }
 else
 {
  if(dd>tmp[mm-1]) return -1;
 }
 //------calculate seconds for date1 00:00:00 from 1.1.1970-----
 time_t temp_t;
 struct tm* tm1_=gmtime(&temp_t);
 tm1_->tm_year=yyyy-1900;
 tm1_->tm_mon=mm-1;
 tm1_->tm_mday=dd;
 tm1_->tm_hour=0;
 tm1_->tm_min=0;
 tm1_->tm_sec=0;
 tm1_->tm_isdst=1;                    //daylight saving time flag
 temp_t=mktime(tm1_);
 if(temp_t<0) return -1;
 return temp_t;
}

//_______calculate day of week__________________________________
// uses  : isLeapYear()
// return: 0=SO, 1=MO, 2=DI, ...
int Timeconverter1::getiDayOfWeek(int yyyy, byte mm, byte dd)
{
 int MonthTable[24]= {0,3,3,6,1,4,6,2,5,0,3,5,-1,2,3,6,1,4,6,2,5,0,3,5};
 int y = yyyy%100;
 int c = 6 - 2 * ((yyyy/100)%4); 
 int tmp_=dd + y + (y / 4) + c;
 int sj=0;
 if(isLeapyear(yyyy)) sj=1;
 tmp_+=MonthTable[sj*12+mm-1];
 return (tmp_ % 7);
}

//_______is year (4 digits) a leapyear?_________________________
bool Timeconverter1::isLeapyear(int year){
 if(!(year % 400) || ((year % 100) && !(year % 4))) return true;
 return false;
}

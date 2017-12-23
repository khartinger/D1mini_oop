//_____D1_class_TimeHttp.cpp__________________171127-171127_____
// The class TimeHttp sends a HTTP 1.1 request (port 80) with 
// missing 'Host: ' in header to a server and waits for the 
// error answer that includes a 'Date: ' header ;)
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
#include "D1_class_TimeHttp.h"

//**************************************************************
//     constructor
//**************************************************************

//_____constructor 1____________________________________________
TimeHttp::TimeHttp(char* ssid, char* pwd)
{ 
 strcpy(ssid_, ssid);
 strcpy(pass_, pwd);
 strcpy(http_, HTTP_SERVER);
 port_=PORT_;
 setup();
}

//_____constructor 2____________________________________________
TimeHttp::TimeHttp(char* ssid, char* pwd, char* http_server)
{ 
 strcpy(ssid_, ssid);
 strcpy(pass_, pwd);
 strcpy(http_, http_server);
 port_=PORT_;
 setup();
}

//_____constructor 3____________________________________________
TimeHttp::TimeHttp(char* ssid, char* pwd, char* http_server,
 unsigned int port)
{ 
 strcpy(ssid_, ssid);
 strcpy(pass_, pwd);
 strcpy(http_, http_server);
 port_=port;
 setup(); 
}

//_____setup properties (called by constructor)_________________
void TimeHttp::setup()
{
 millis_lastConnected = 0;
 dt_.yy=0;                   // year yyyy
 dt_.mm=0; dt_.dd=0;         // month (1..12), day (1..31)
 dt_.HH=0; dt_.MM=0; dt_.SS=0;// hour (0..23),minute,seconds
 dt_.ww=0;                   // day of week 0=sunday, ..
 setup_wifi();
}

//**************************************************************
// methods to setup WLAN
//**************************************************************

//_____connect to the WiFi network______________________________
// return: true=connected, false=not connected to WiFi
bool TimeHttp::setup_wifi()
{
 if(WiFi.status()==WL_CONNECTED) return true;
 delay(10);
 if(DEBUG1) Serial.println("\nConnecting to "+String(ssid_));
 WiFi.begin(ssid_, pass_);
 //-----try to connect to WLAN (access point)-------------------
 int i=TIMEOUT_WIFI_CONNECT_MS/200;
 while((WiFi.status()!=WL_CONNECTED) && (i>0))
 {
  delay(200);
  i--;
  if(DEBUG1){Serial.print("."); if(i%50==0) Serial.println("");}
 }
 //-----connected to WLAN (access point)?-----------------------
 if(i<1)
 { //-----not connected to WLAN---------------------------------
  if(DEBUG1) Serial.println("No connection - time-out!");
  return false;
 }
 //-----success WiFi new connection/reconnect-------------------
 if(DEBUG1)Serial.println("\nConnected! IP address is "+WiFi.localIP().toString());
 return true;
}

//_____check for connect, if not: try to reconnect______________
// return: true=connected, false=not connected to WiFi
bool TimeHttp::reconnect()
{
 if(WiFi.status()==WL_CONNECTED) return true;
 if(DEBUG1) Serial.println("Disconnected. Try to reconnect...");
 return setup_wifi();
}

//_____is http connection ok? (no: reconnect)___________________
bool TimeHttp::isConnected()
{
 if(WiFi.status()==WL_CONNECTED) return true;
 long now = millis();
 if (now - millis_lastConnected > HTTP_RECONNECT_MS) 
 {
  millis_lastConnected=now;
  if(reconnect()) millis_lastConnected=0;
 }
 else
 {
  String sSec=String((now - millis_lastConnected)/1000);
  if(DEBUG1) Serial.println("Not connected. Reconnect in "+sSec+" sec");
 }
 if(WiFi.status()==WL_CONNECTED) return true;
 return false;
}

//**************************************************************
// methods for http request
//**************************************************************

//_____send HTTP request to server______________________________
// result: values in struct dt_ (
// return: status 0=OK, 1=no Wifi, 2=no request server, 
//         3=no 'Date: ' header
int TimeHttp::sendRequest()
{
 char mmm[5];                          // month as english text
 char mm_;                             // month number
 Dati dt;
 //if(!isConnected()) return 1;
 if(!reconnect()) return 1;
 if(DEBUG1) Serial.println("Connected to Wifi!");
 if(!d1miniClient.connect(http_, port_)) return 2;
 if(DEBUG1) Serial.println("Connected to server!");
 d1miniClient.setTimeout(5000);
 // Make a HTTP 1.1 request whith missing "Host: " in header.
 // => Host answers with an error that includes Date: header ;)
 d1miniClient.print(F("GET / HTTP/1.1 \r\n\r\n"));
 //-----look for Date: header-----------------------------------
 if(!(d1miniClient.find((char *)"\r\nDate: ")
   && d1miniClient.readBytes(mmm, 5) == 5))  // discard
 return 3;
 dt_.dd=d1miniClient.parseInt();          // day
 d1miniClient.readBytes(mmm, 1);          // discard blank
 d1miniClient.readBytes(mmm, 3);          // month engl. text
 dt_.yy=d1miniClient.parseInt();          // year
 dt_.HH=d1miniClient.parseInt();          // hour
 dt_.MM=d1miniClient.parseInt();          // minute
 dt_.SS=d1miniClient.parseInt();          // second
 //-----translate month text to month nr and num days before----
 switch (mmm[0])
 {
  case 'J': if(mmm[1]=='a') mm_=1;   // Jan, Jun, Jul
            else { if(mmm[2]=='n') mm_=6; else mm_=7; }
            break;
  case 'F': mm_=2; break;            // Feb
  case 'M': if(mmm[2]='r') mm_=3; else mm_=5;  // Mar, May
            break;
  case 'A': if(mmm[2]='r') mm_=4; else mm_=8;  // Apr, Aug
  case 'S': mm_=9;  break;           // Sep
  case 'O': mm_=10; break;           // Oct
  case 'N': mm_=11; break;           // Nov
  case 'D': mm_=12; break;           // Dec
  default:  mm_=0;  break;           // not possible
 }
 dt_.mm=mm_;                         // month
 delay(10);
 d1miniClient.flush();
 d1miniClient.stop();
 //-----for test only-------------------------------------------
 //dt_.yy=2018; dt_.mm=10; dt_.dd=28;
 //dt_.HH=1; dt_.MM=0; dt_.SS=0;
 //dt_.ww=getDayOfWeek(dt_.yy, dt_.mm, dt_.dd);
 return 0;
}

//_____get String date time UTC yyyymmdd hhmmss_________________
String TimeHttp::getDateTime()
{
 char buf[80];
 String s1="";
 int summertime=0;
 if(sendRequest()==0) {
   sprintf(buf,"%04d%02d%02d %02d%02d%02d",dt_.yy,
   dt_.mm,dt_.dd,dt_.HH,dt_.MM,dt_.SS);
  s1=String(buf);
 }
 return s1;
}
  
//_____get String date time UTC dd.mm.yyyy hh:mm:ss_____________
String TimeHttp::getDateTime2()
{
 char buf[80];
 String s1="";
 if(sendRequest()==0) {
  sprintf(buf,"%02d.%02d.%04d %02d:%02d:%02d",dt_.dd,
   dt_.mm,dt_.yy,dt_.HH,dt_.MM,dt_.SS);
  s1=String(buf);
 }
 return s1;
}

//**************************************************************
// little helpers
//**************************************************************

//_____is year (4 digits) a leapyear?___________________________
bool TimeHttp::isLeapyear(int year){
 if(!(year % 400) || ((year % 100) && !(year % 4))) return true;
 return false;
}

//_____calculate day of week____________________________________
// 0=SO, 1=MO, 2=DI, ...
byte TimeHttp::getDayOfWeek(int yyyy, byte mm, byte dd)
{
 char MonthTable[24]=  
  {0,3,3,6,1,4,6,2,5,0,3,5,-1,2,3,6,1,4,6,2,5,0,3,5};
 int y = yyyy%100;
 int c = 6 - 2 * ((yyyy/100)%4); 
 int tmp_=dd + y + (y / 4) + c;
 int sj=0;
 if(isLeapyear(yyyy)) sj=1;
 tmp_+=MonthTable[sj*12+mm-1];
 return (tmp_ % 7);
}

//_____Add or sub hours (max. 255)______________________________
//return: style=1: yyyymmdd HHMMSS, style=2: dd.mm.yyyy HH:MM:SS
String TimeHttp::addsubHours(int yy, int mm, int dd, 
 int HH, int MM, int SS, int hours, int style)
{
 byte dd_max[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
 byte dd_max1;
 int  h1;
 String s1=""; 
 //-----plausibility check--------------------------------------
 if(hours >  255) return s1;
 if(hours < -255) return s1;
 if((yy<2000)||(yy>2099)) return s1;
 if((mm<1)||(mm>12)) return s1;
 if((dd<1)||(dd>dd_max[mm])) return s1;
 if((HH<0)||(HH>23)) return s1;
 if((MM<0)||(MM>59)) return s1;
 if((SS<0)||(SS>59)) return s1;
 //-----add hours-----------------------------------------------
 if(hours>0)
 {
  h1 = HH + hours;                // new hours
  HH = h1 % 24;                   // hour (0..23)
  h1 = h1 / 24;                   // days to add
  dd = dd + h1;                   // new days
  dd_max1=dd_max[mm];             // max days of this month
  if((mm==2)&&isLeapyear(yy)) dd_max1++;
  if(dd > dd_max1)                // too much days of this month
  {
   dd = dd - dd_max1;             // new day
   mm = mm + 1;                   // next month
   if(mm > 12)                    // too much months
   {
    yy = yy + 1;                  // next year
    mm = 1;                       // January
   }
  }
 }
 //-----sub hours-----------------------------------------------
 if(hours<0)
 {
  h1 = HH + hours;                // new hours
  if(h1>=0) HH=h1;                // hours ok
  else                            // hour is day(s) before
  {
   h1=1+int(-h1/24);              // full days to sub
   HH = HH +h1*24+hours;          // hours now ok
   h1=dd-h1;                      // new day
   if(h1>0) dd = h1;              // day in this month
   else                           // day in month before
   {
    mm = mm - 1;                  // month before
    if(mm < 1)                    // year before
    {
     mm = 12;                     // December
     yy = yy - 1;                 // year before
    }
    dd_max1=dd_max[mm];           // max days of this month
    if((mm==2)&&isLeapyear(yy)) dd_max1++;
    dd = dd_max1 + h1;            // day ok;
   }
  }
 }
 //-----build string--------------------------------------------
 char buf[80];
 if(style==1)
  sprintf(buf,"%04d%02d%02d %02d%02d%02d",yy,mm,dd,HH,MM,SS);
 else
  sprintf(buf,"%02d.%02d.%04d %02d:%02d:%02d",dd,mm,yy,HH,MM,SS);
 s1=String(buf);
 return s1;
}

//_____Add or sub hours (max. 255) to String____________________
// sDT = yyyymmdd HHMMSS
String TimeHttp::addsubHours(String sDT, int hours)
{
 int yy=(sDT.substring(0,4)).toInt();
 int mm=(sDT.substring(4,6)).toInt();
 int dd=(sDT.substring(6,8)).toInt();
 int HH=(sDT.substring(9,11)).toInt();
 int MM=(sDT.substring(11,13)).toInt();
 int SS=(sDT.substring(13,15)).toInt();
 return addsubHours(yy,mm,dd,HH,MM,SS,hours,1);
}

//_____Add or sub hours (max. 255) to String____________________
// sDT = dd.mm.yyyy HH:MM:SS
String TimeHttp::addsubHours2(String sDT, int hours)
{
 int dd=(sDT.substring(0,2)).toInt();
 int mm=(sDT.substring(3,5)).toInt();
 int yy=(sDT.substring(6,10)).toInt();
 int HH=(sDT.substring(11,13)).toInt();
 int MM=(sDT.substring(14,16)).toInt();
 int SS=(sDT.substring(17,19)).toInt();
 return addsubHours(yy,mm,dd,HH,MM,SS,hours,2);
}

//_____is summertime (DST)? (0=NO, 1=yes)_______________________
// Date: UTC + timezone, parameter NOT summertime! (CET: TZ=1)
// return: 0=NO, 1=yes
// UTC=20180325 005959 CET=20180325 015959 (ST ------) -> ST=0
// UTC=20180325 010000 CET=20180325 020000 (ST 030000) -> ST=1
// UTC=20181028 005959 CET=20181028 015959 (ST 025959) -> ST=1
// UTC=20181028 010000 CET=20181028 020000 (ST -3----) -> ST=0
// UTC=20181028 010001 CET=20181028 020001 (ST ------) -> ST=0
int TimeHttp::getSummertime(int yy, int mm, int dd, 
 int HH, int timezone)
{
 if(mm<3 || mm>10) return 0;      // no DST Jan,Feb,Nov,Dez
 if(mm>3 && mm<10) return 1;      // DST in Apr...Sep
 //-----March---------------------------------------------------
 if(mm==3)
 {
  byte ddSunday=31-getDayOfWeek(yy,3,31);   //last Sunday in Mar
  if( (24*dd+HH+timezone) >= (2+24*ddSunday) ) return 1;
  return 0;
 }
 //-----October-------------------------------------------------
 byte ddSunday=31-getDayOfWeek(yy,10,31);   //last Sunday in Oct
 if( (24*dd+HH+timezone) < (2+24*ddSunday) ) return 1;
 return 0;
}

//_____is summertime? (0=NO, 1=yes)_____________________________
// return: 0=NO, 1=yes | sDT = yyyymmdd HHMMSS
int TimeHttp::getSummertime(String sDT_UTC, int timezone)
{
 int yy=(sDT_UTC.substring(0,4)).toInt();
 int mm=(sDT_UTC.substring(4,6)).toInt();
 int dd=(sDT_UTC.substring(6,8)).toInt();
 int HH=(sDT_UTC.substring(9,11)).toInt();
 return getSummertime(yy,mm,dd,HH,timezone);
}

//_____is summertime? (0=NO, 1=yes)_____________________________
// return: 0=NO, 1=yes | sDT = dd.mm.yyyy HH:MM:SS
int TimeHttp::getSummertime2(String sDT_UTC, int timezone)
{
 int dd=(sDT_UTC.substring(0,2)).toInt();
 int mm=(sDT_UTC.substring(3,5)).toInt();
 int yy=(sDT_UTC.substring(6,10)).toInt();
 int HH=(sDT_UTC.substring(11,13)).toInt();
 return getSummertime(yy,mm,dd,HH,timezone);
}

//**************************************************************
//     convert struct date <-> seconds since 1.1.1970
//**************************************************************

//_____convert struct Dati to secs since 1.1.1970_______________
// This code will not work after Feb. 2100 (not a leap year)
unsigned long TimeHttp::dati2sec(struct Dati dt1)
{
 int daysInPrevMonths[]={0,0,31,59,90,120,151,181,212,243,273,304,334};
 int daysPrev=daysInPrevMonths[dt1.mm];
 unsigned long ret=dt1.dd;
 ret+=(dt1.yy-1970)*365;    // + days from 1970 to whole past year
 ret+=(dt1.yy-1969)>> 2;    // + one day per leap year 
 ret+=daysPrev;             // + days for previous months this year
 if (daysPrev >= 59         // if we are past February
  && ((dt1.yy & 3)==0))     // and this is a leap year
  ret+=1;                   // add one day
 //-----Remove today, add hours, minutes and seconds this month-
 ret = (((ret-1ul) * 24 + dt1.HH) * 60 + dt1.MM) * 60 + dt1.SS;
 return ret;
}

//_____convert unix-time (1.1.1970) to strut Dati_______________
// This code will not work after Feb. 2100 (not a leap year)
void TimeHttp::sec2dati(unsigned long sec1970, struct Dati* dt1,
     int timezone=0)
{
 unsigned long quot;              // division result
 long days, days1;                // days
 int leapyear;                    // 0=NO, 1=yes
 int year=1970;                   // start year
 //-----PART 1: timezone (UTC=0, CET=+1)------------------------
 sec1970=sec1970+3600*timezone;
 //dt1->tz=timezone;
 //-----PART 2: sec1970 -> secs, minutes, hours, days-----------
 dt1->SS=sec1970 % 60L;           // seconds (0...59)
 quot=sec1970 / 60L;
 dt1->MM=quot % 60L;              // minutes (0...59)
 quot = quot / 60L;
 dt1->HH=quot % 24L;              // hours   (0...23)
 days = quot / 24L;               // whole days (since 1970)
 //-----PART 3: calculate weekday (1.1.1970 was a thursday)-----
 dt1->ww=(days+4)%7;
 //-----PART 4: convert days to year + days from 1.1.-----------
 // group days by 4 years with 365+365+366+365=1461days
 year=year+4*(days/1461L);
 days=days%1461L;
 leapyear=0;
 if(days>364)                     // more days than 1st year
 {
  year++;                         // add 1 year
  days=days-365;                  // reduce days by a year
  if(days>364)                     // more days than 1st year
  {
   year++;                        // add 1 year
   days=days-365;                 // reduce days by leapyear
   if(days>365)                    // 2nd year is leap year!
   {
    year++;                        // add 1 more year
    days=days-366;                 // reduce days of leap year
   }
   else
   {
    leapyear=1;
   }
  }
 }
 dt1->yy=year;
 //-----PART 5: calculate month and day  within one year--------
 days1=59+leapyear;               // days of Jan+Feb
 if(days < days1)                 // yes, it is Jan or Feb
 {                                // devide into Jan/Feb
  dt1->mm= 1 + days / 31L;        // month (1...2)
  dt1->dd= 1 + days % 31L;        // day (1...28|29...31)
 }
 else                             // no: Mar to Dec
 {
  days=days-days1;                // days from March 1
                                  // 153days before/after Jul/Aug
  dt1->mm = 2 + (days/153L)*5;    // month = 2 or 7
  days=days % 153;                //
  dt1->mm+=(days/61L)*2;          // number of (30+31) days 0..2
                                  // (month 2|4|6 or 7|9|11)
  days=days % 61;                 // remaining days (0..60)
  dt1->dd = 1+ days%31;           // day   (1..30|31)
  dt1->mm+= 1+ days/31L;          // month (1..12)
 }
}

//_____Add or sub hours (max. 255) in struct____________________
void TimeHttp::addsubHours(struct Dati* dt1, int hours)
{
 byte dd_max[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
 byte dd_max1;
 int  h1;
 if(hours>255) return;
 if(hours<-255) return;
 //-----add hours-----------------------------------------------
 if(hours>0)
 {
  h1 = dt1->HH + hours;           // new hours
  dt1->HH = h1 % 24;              // hour (0..23)
  h1 = h1 / 24;                   // days to add
  dt1->dd = dt1->dd + h1;         // new days
  dd_max1=dd_max[dt1->mm];        // max days of this month
  if((dt1->mm==2)&&isLeapyear(dt1->yy)) dd_max1++;
  if(dt1->dd > dd_max1)           // too much days of this month
  {
   dt1->dd = dt1->dd - dd_max1;   // new day
   dt1->mm = dt1->mm + 1;         // next month
   if(dt1->mm > 12)               // too much months
   {
    dt1->yy = dt1->yy + 1;        // next year
    dt1->mm = 1;                  // January
   }
  }
 }
 //-----sub hours-----------------------------------------------
 if(hours<0)
 {
  h1 = dt1->HH + hours;           // new hours
  if(h1>=0) dt1->HH=h1;           // hours ok
  else                            // hour is day(s) before
  {
   h1=1+int(-h1/24);              // full days to sub
   dt1->HH = dt1->HH +h1*24+hours;// hours now ok
   h1=dt1->dd-h1;                 // new day
   if(h1>0) dt1->dd = h1;         // day in this month
   else                           // day in month before
   {
    dt1->mm = dt1->mm - 1;        // month before
    if(dt1->mm < 1)               // year before
    {
     dt1->mm = 12;                // December
     dt1->yy = dt1->yy - 1;       // 
    }
    dd_max1=dd_max[dt1->mm];      // max days of this month
    if((dt1->mm==2)&&isLeapyear(dt1->yy)) dd_max1++;
    dt1->dd = dd_max1 + h1;       // day ok;
   }
  }
 }
 dt1->ww = getDayOfWeek(dt1->yy, dt1->mm, dt1->dd);
}

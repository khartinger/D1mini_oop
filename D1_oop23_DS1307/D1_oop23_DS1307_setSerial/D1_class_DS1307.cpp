//_____D1_class_DS1307.cpp_____________________171127-171128_____
// D1 mini class for access to a RTC DS1307 by i2c.
// The class is just for communication with a DS1307 clock and 
// does not regard daylight saving time (=summer time) etc.
// Default i2c 7bit-address is 0x68.
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
#include "D1_class_DS1307.h"

//**************************************************************
//*     constructor(s)                                         *
//**************************************************************
//_____constructor (default 0x50)_______________________________
DS1307::DS1307() { i2cAddress=DS1307_ADDR; setup(); }

//_____constructor with i2c Address_____________________________
DS1307::DS1307(int i2c_address) {
 i2cAddress=i2c_address; 
 setup(); 
}

//**************************************************************
//*     general setter and getter methods                      *
//**************************************************************
//_____return i2c address_______________________________________
int DS1307::getAddress() { return i2cAddress; }

//_____system status____________________________________________
int DS1307::getStatus() { return status_; }

//**************************************************************
//*     clock/calendar specials  read from DS1307              *
//**************************************************************
//_____send date string to DS1307_______________________________
// return: status_
int DS1307::setDate(String yyyymmdd){
 //-----check date----------------------------------------------
 if(!isDateOK(yyyymmdd)) return status_;
 //-----date ok, save them as properties------------------------
 yy_=(yyyymmdd.substring(0,4)).toInt();
 mm_=(yyyymmdd.substring(4,6)).toInt();
 dd_=(yyyymmdd.substring(6)).toInt();
 ww_=getDayOfWeek(yy_, mm_, dd_);
 return writeDateRegs(yy_, mm_, dd_);
}

//_____send date values to DS1307_______________________________
// return: status_
int DS1307::setDate(int yyyy, byte mm, byte dd)
{
 if(!isDateOK(yyyy, mm, dd)) return status_;    // check date
 yy_=yyyy; mm_=mm; dd_=dd;
 ww_=getDayOfWeek(yy_, mm_, dd_);
 return writeDateRegs(yy_, mm_, dd_);
}

//_____send time string to DS1307_______________________________
// return: status_
int DS1307::setTime(String HHMMSS){
 if(!isTimeOK(HHMMSS)) return status_;           // check date
 HH_=(HHMMSS.substring(0,2)).toInt();
 MM_=(HHMMSS.substring(2,4)).toInt();
 SS_=(HHMMSS.substring(4)).toInt();
 return writeTimeRegs(HH_, MM_, SS_);
}

//_____send time values to DS1307_______________________________
int DS1307::setTime(byte HH, byte MM, byte SS)
{
 if(!isTimeOK(HH, MM, SS)) return status_;       // check date
 HH_=HH; MM_=MM; SS_=SS;
 return writeTimeRegs(HH_, MM_, SS_);
}

//_____set date and time to DS1307______________________________
int DS1307::setDateTime(String yyyymmdd_HHMMSS)
{
 status_=setDate(yyyymmdd_HHMMSS.substring(0,8));
 if(status_!=DS1307_OK) return status_;
 return setTime(yyyymmdd_HHMMSS.substring(9));
}

//_____set date and time to DS1307 by seconds since 1970________
// return: status_
int DS1307::setSecs1970(unsigned long secs1970)
{
 unsigned long quot;              // results by division
 long days, days1, days1970;      // number of days
 int leapyear;                    // 1=leap year, 0=not
 int year=1970;                   // start year
 //-----PART 1: sec1970 -> secs, minutes, hours, days-----------
 SS_=secs1970 % 60L;              // seconds (0...59)
 quot=secs1970 / 60L;
 MM_=quot % 60L;                  // minutes (0...59)
 quot = quot / 60L;
 HH_=quot % 24L;                  // hours   (0...23)
 days = quot / 24L;               // whole days (since 1970)
 days1970=days;
 //-----PART 2: calculate weekday (1.1.1970 was a thursday)-----
 ww_=(days+4)%7;
 //-----PART 3: convert days to year + days from 1.1.-----------
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
 yy_=year;
 //-----PART 4: calculate month and day  within one year--------
 days1=59+leapyear;               // days of Jan+Feb
 if(days < days1)                 // yes, it is Jan or Feb
 {                                // devide into Jan/Feb
  mm_ = 1 + days / 31L;           // month (1...2)
  dd_ = 1 + days % 31L;           // day (1...28|29...31)
 }
 else                             // no: Mar to Dec
 {
  days=days-days1;                // days from March 1
                                  // 153days before/after Jul/Aug
  mm_ = 2 + (days/153L)*5;        // month = 2 or 7
  days=days % 153;                //
  mm_+=(days/61L)*2;              // number of (30+31) days 0..2
                                  // (month 2|4|6 or 7|9|11)
  days=days % 61;                 // remaining days (0..60)
  dd_ = 1+ days%31;               // day   (1..30|31)
  mm_+= 1+ days/31L;              // month (1..12)
 }
 //-----PART 5: write date and time to DS1307___________________
 writeDateRegs(yy_, mm_, dd_);
 writeTimeRegs(HH_, MM_, SS_);
 return status_;
}

//_____get seconds since 1970 from DS1307_______________________
// return: number of seconds or 0
unsigned long DS1307::getSecs1970()
{
 String s1=getDateTime();
 if(s1=="") return 0; 
 return getLastSecs1970();
}

//_____read date from DS1307 and return it format dd.mm.yyyy____
String DS1307::getDate2()
{
 String s1=getDate();
 if(s1.length()!=8) return s1;
 return s1.substring(6)+"."+s1.substring(4,6)+"."+s1.substring(0,4);
}

//_____read time from DS1307 and return it in format HH:MM:SS___
String DS1307::getTime2()
{
 String s1=getTime();
 if(s1.length()!=6) return s1;
 return s1.substring(0,2)+":"+s1.substring(2,4)+":"+s1.substring(4);
}

//_____read date and time from DS1307, return yyyymmdd HHMMSS___
// return: date time string or empty string ""
String DS1307::getDateTime() {
 String s1=getDate();
 if(s1=="") return s1;
 return s1+" "+getTime();
}

//_____read date and time from DS1307, return yyyymmdd HHMMSS___
// return: date time string or empty string ""
String DS1307::getDateTime2() {
 String s1=getDate2();
 if(s1=="") return s1;
 return s1+" "+getTime2();
}

//_____weekday as two chars (in German) with DS1307 access______
String DS1307::getWeekday() {
 String weekday("SOMODIMIDOFRSA");
 ww_=read1reg(0x03)-1;
 if((ww_<0)||(ww_>6)) return "??";
 return weekday.substring(2*ww_,2*ww_+2);
}

//**************************************************************
//*     clock/calendar data without DS1307 update (properties) *
//**************************************************************
//_____return last date yyyymmdd without DS1307 access__________
String DS1307::getLastDate() {return date2string(yy_,mm_,dd_);}

//_____return last date dd.mm.yyyy without DS1307 access________
String DS1307::getLastDate2(){return date2string2(yy_,mm_,dd_);}

//_____return last time HHMMSS without DS1307 access____________
String DS1307::getLastTime() {return time2string(HH_,MM_,SS_);}

//_____return last time HH:MM:SS without DS1307 access__________
String DS1307::getLastTime2(){return time2string2(HH_,MM_,SS_);}

//_____return last date and time without DS1307 access__________
// return: yyyymmdd HHMMSS
String DS1307::getLastDateTime()
{
 String s1, s2;
 s1=date2string(yy_,mm_,dd_);
 if(s1=="") return s1;
 s2=time2string(HH_,MM_,SS_);
 if(s2=="") return s2;
 return s1+" "+s2;
}

//_____return last date and time without DS1307 access__________
// return: dd.mm.yyyy HH:MM:SS
String DS1307::getLastDateTime2()
{
 String s1, s2;
 s1=date2string2(yy_,mm_,dd_);
 if(s1=="") return s1;
 s2=time2string2(HH_,MM_,SS_);
 if(s2=="") return s2;
 return s1+" "+s2;
}

//_____return last weekday without DS1307 access________________
String DS1307::getLastWeekday() 
{
 String weekday("SOMODIMIDOFRSA");
 if((ww_<0)||(ww_>6)) return "??";
 return weekday.substring(2*ww_,2*ww_+2);
}

//_____convert properties to seconds since 1.1.1970_____________
// return: number of seconds or 0
unsigned long DS1307::getLastSecs1970()
{
 int daysInPrevMonths[]={0,0,31,59,90,120,151,181,212,243,273,304,334};
 int daysPrev=daysInPrevMonths[mm_];
 unsigned long ret=dd_; 
 String s1;
 s1=readTime();             // sets property HH_, MM_, SS_
 if(s1=="") return 0;
 s1=readDate();             // sets yy_, mm_, dd_, ww_
 if(s1=="") return 0;
 ret+=(yy_-1970)*365;       // + days from 1970 to whole past year
 ret+=(yy_-1969)>> 2;       // + one day per leap year 
 ret+=daysPrev;             // + days for previous months this year
 if (daysPrev >= 59         // if we are past February
  && ((yy_ & 3)==0))        // and this is a leap year
  ret+=1;                   // add one day
 //-----Remove today, add hours, minutes and seconds this month-
 ret = (((ret-1ul) * 24 + HH_) * 60 + MM_) * 60 + SS_;
 return ret;
}

//**************************************************************
//*     use DS1307 RAM                                         *
//**************************************************************

//_____set RAM (8..63) to 0 and check, if ok____________________
// return: status_
int DS1307::clearRAM() { return clearRAM(0); }

//_____set RAM (8..63) to initbyte and check, if ok_____________
// return: status_
int DS1307::clearRAM(byte initbyte=0)
{
 byte b_;
 //-----write to RAM--------------------------------------------
 int i=DS1307_RAMBEGIN;
 while(i<=DS1307_RAMEND)
 {
  write1reg(i++, initbyte);
  if(status_!=DS1307_OK) return status_;
 }
 //-----check for RAM content is ok-----------------------------
 i=DS1307_RAMBEGIN;
 while(i<=DS1307_RAMEND)
 { 
  b_=read1reg(i++);
  if(status_!=DS1307_OK) return status_;
  if(b_!=initbyte) return DS1307_ERR_RAM;
 }
 return status_;
}
  
//_____write a byte into DS1307 RAM at address 8..63____________
// return: status_
int DS1307::setRAMbyte(int address, byte data)
{
 if((address<DS1307_RAMBEGIN)||(address>DS1307_RAMEND))
  { status_=DS1307_ERR_RANGE; return status_; }
 write1reg(address, data);
 return status_;
}

//_____read a byte from DS1307 RAM at address 8..63_____________
// return: byte or 0 (success: see status)
byte DS1307::getRAMbyte(int address)
{
 if((address<DS1307_RAMBEGIN)||(address>DS1307_RAMEND))
 { status_=DS1307_ERR_RANGE; return 0; }
 return read1reg(address);
}

//_____write a int (2byte) into DS1307 RAM at address 8..62_____
int DS1307::setRAMint16(int address, int int16)
{
 int status;
 if((address<DS1307_RAMBEGIN)||(address>DS1307_RAMEND-1))
  { status_=DS1307_ERR_RANGE; return status_; }
 status_=write1reg(address, (byte)(int16>>8));
 if(status_!=DS1307_OK) return status_;
 status_=write1reg(address+1, (byte)(int16 & 0x00FF));
 return status_;
}

//_____read a int (2byte) from DS1307 RAM at address 8..62______
int DS1307::getRAMint16(int address)
{
 unsigned int uint16;
 int int16;
 if((address<DS1307_RAMBEGIN)||(address>DS1307_RAMEND-1))
  { status_=DS1307_ERR_RANGE; return status_; }
 uint16=read1reg(address) << 8;
 uint16=uint16 | read1reg(address+1);
 if((uint16&0x8000)>0) int16=uint16-0xFFFF-1;
 else int16=(int)uint16;
 return int16;
}

//**************************************************************
//*     public helper functions                                *
//**************************************************************

//_____convert date values to string yyyymmdd (add 0 if needed)_
String DS1307::date2string(int yyyy, byte mm, byte dd) {
 String s1="";
 if(!isDateOK(yyyy,mm,dd)) return s1;
 s1+=String(yy_);
 if(mm_<10) s1+="0";
 s1+=String(mm_);
 if(dd_<10) s1+="0";
 s1+=String(dd_);
 return s1;
}

//_____convert date to string dd.mm.yyyy (add 0 if needed)______
String DS1307::date2string2(int yyyy, byte mm, byte dd) {
 String s1="";
 if(!isDateOK(yyyy,mm,dd)) return s1;
 if(dd_<10) s1+="0";
 s1+=String(dd_);
 s1+=".";
 if(mm_<10) s1+="0";
 s1+=String(mm_);
 s1+=".";
 s1+=String(yy_);
 return s1;
}

//_____convert time values to string HHMMSS (add 0 if needed)___
String DS1307::time2string(byte hh, byte mm, byte ss) {
 String s1="";
 if(!isTimeOK(hh, mm, ss)) return s1;
 if(hh<10) s1+="0";
 s1+=String(hh);
 if(mm<10) s1+="0";
 s1+=String(mm);
 if(ss<10) s1+="0";
 s1+=String(ss);
 return s1;
}

//_____convert time to string HH:MM:SS (add 0 if needed)________
String DS1307::time2string2(byte hh, byte mm, byte ss) {
 String s1="";
 if(!isTimeOK(hh, mm, ss)) return s1;
 if(hh<10) s1+="0";
 s1+=String(hh);
 s1+=":";
 if(mm<10) s1+="0";
 s1+=String(mm);
 s1+=":";
 if(ss<10) s1+="0";
 s1+=String(ss);
 return s1;
}

//_____is year (4 digits) a leapyear?___________________________
bool DS1307::isLeapyear(int year){
 if(!(year % 400) || ((year % 100) && !(year % 4))) return true;
 return false;
}

//_____is given time summertime? (=DST=Daylight Saving Time)____
// timezone: UTC=0, MEZ=1
bool DS1307::isSummertime(int yyyy, byte mm,
     byte dd, byte HH, byte timezone=1)
{
 if(mm<3 || mm>10) return false; //no DST in Jan, Feb, Nov, Dez
 if(mm>3 && mm<10) return true;  //DST in Apr...Sep
 //-----March---------------------------------------------------
 if(mm==3)
 {
  byte ddBegin=31-getDayOfWeek(yyyy,3,31);  //last Sunday in Mar
  if( (24*dd+HH+timezone) >= (2+24*ddBegin) ) return true;
  return false;
 }
 //-----October-------------------------------------------------
 byte ddEnd=31-getDayOfWeek(yyyy,10,31);    //last Sunday in Oct
 if( (24*dd+HH+timezone) < (2+24*ddEnd) ) return true;
 return false;
}

//_____is actual time summertime (=DST=Daylight Saving Time)?___
bool DS1307::isSummertime(byte timezone)
{
 return isSummertime(2000+yy_, mm_, dd_, HH_, timezone);
}

//**************************************************************
//     protected helper functions                              *
//**************************************************************

//_____setup device DS1307_____________________________________
// called by constructors
void DS1307::setup() {
 Wire.begin();                    //(4,5): SDA on 4, SCL on 5
 delay(10);
 //-----start clock, if it is not running-----------------------
 byte reg0=read1reg(0);
 if(status_!=DS1307_OK) return;
 if((reg0&0x80)>0) write1reg(0, reg0&0x7F);
 if(status_!=DS1307_OK) return;
 //-----24 hour mode--------------------------------------------
 byte reg2=read1reg(2);
 if(status_!=DS1307_OK) return;
 if((reg2&0x40)>0) write1reg(2, reg2&0xBF);
 if(status_!=DS1307_OK) return;
 //-----init values yy_, mm_, ... reading DS1307----------------
 yy_=0; mm_=0; dd_=0; ww_=0; HH_=0; MM_=0; SS_=0;
 String s1=getDate();
 if(status_!=DS1307_OK) return;
 s1=getTime();
}

//_____convert bcd-byte to int value____________________________
byte DS1307::bcd2byte(byte bcd){
 return ((bcd >> 4) * 10) + (bcd & 0x0f);
}

//_____convert 8 bit value (00..99) to bcd______________________
// if value out of range: return 0
byte DS1307::byte2bcd(byte value8bit) {
 if((value8bit<0)||(value8bit>99)) return 0;
 return ((value8bit / 10) << 4) + (value8bit % 10);
}

//_____calculate day of week____________________________________
// 0=SO, 1=MO, 2=DI, ...
byte DS1307::getDayOfWeek(int yyyy, byte mm, byte dd)
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

//_____check if date values are ok (year: 2000...2099!)_________
// return: true or false
// success=status_: ok, wrong range
bool DS1307::isDateOK(int yyyy, byte mm, byte dd) {
 if((yyyy<2000)||(yyyy>2099)) 
  { status_=DS1307_ERR_RANGE; return false; }
 if((mm<1)||(mm>12)) { status_=DS1307_ERR_RANGE; return false; }
 if((dd<1)||(dd>31)) { status_=DS1307_ERR_RANGE; return false; }
 if(dd>30) 
 {
  if((mm==4)||(mm==6)||(mm==9)||(mm==11)) 
   { status_=DS1307_ERR_RANGE; return false; }
 }
 if(mm==2) 
 {
  if((dd>29)||((!isLeapyear(yyyy))&&(dd>28))) 
   { status_=DS1307_ERR_RANGE; return false; }
 }
 status_=DS1307_OK;
 return true;
}

//_____check if date String is ok_______________________________
// return: true or false
// success=status_: ok, wrong number of chars, wrong range
bool DS1307::isDateOK(String yyyymmdd) {
 int iyyyy, imm, idd;
 if(yyyymmdd.length()!=8) 
  { status_=DS1307_ERR_NUM_CHARS; return false; }
 iyyyy=(yyyymmdd.substring(0,4)).toInt();
 imm=(yyyymmdd.substring(4,6)).toInt();
 idd=(yyyymmdd.substring(6)).toInt();
 return isDateOK(iyyyy,imm,idd);
}


//_____check if time values are ok______________________________
bool DS1307::isTimeOK(byte HH, byte MM, byte SS)
{
 if((HH>23)||(HH<0)) { status_=DS1307_ERR_RANGE; return false; }
 if((MM>59)||(MM<0)) { status_=DS1307_ERR_RANGE; return false; }
 if((SS>59)||(SS<0)) { status_=DS1307_ERR_RANGE; return false; }
 status_=DS1307_OK;
 return true;
} 

//_____check if date String is ok_______________________________
// return: true or false
// success=status_: ok, wrong number of chars, wrong range
bool DS1307::isTimeOK(String HHMMSS) {
 int iHH, iMM, iSS;
 if(HHMMSS.length()!=6) 
  { status_=DS1307_ERR_NUM_CHARS; return false; }
 iHH=(HHMMSS.substring(0,2)).toInt();
 iMM=(HHMMSS.substring(2,4)).toInt();
 iSS=(HHMMSS.substring(4)).toInt();
 return isTimeOK(iHH,iMM,iSS);
}

//**************************************************************
//     i2c access                                              *
//**************************************************************

//_____write the byte to address reg____________________________
// return: status_
int DS1307::write1reg(byte reg, byte value) 
{
 Wire.beginTransmission(i2cAddress);
 Wire.write(reg);                 // register
 Wire.write(value);               // value
 status_=Wire.endTransmission();
 return status_;
}

//_____read one byte from address reg___________________________
byte DS1307::read1reg(byte reg) 
{
 Wire.beginTransmission(i2cAddress);
 Wire.write(reg);                 // start register
 status_=Wire.endTransmission();
 if(status_!=DS1307_OK) return 0;
 if(Wire.requestFrom(i2cAddress, 1)<1)
  { status_=DS1307_ERR_NO_BYTE; return 0; }
 byte reg0=Wire.read();
 status_=Wire.endTransmission();
 if(status_!=DS1307_OK) return 0;
 return reg0;
}

//_____write date into regs_____________________________________
// used by setDate()
// return: status_
int DS1307::writeDateRegs(int yyyy, byte mm, byte dd)
{
 //-----input ok, generate bytes--------------------------------
 byte reg4=byte2bcd(dd);
 byte reg5=byte2bcd(mm);
 byte reg6=byte2bcd(yyyy-2000);
 byte ww=getDayOfWeek(yyyy, mm, dd);
 byte reg3=ww+1;
 //-----send date bytes to DS1307-------------------------------
 status_=DS1307_ERR_NACK_DATA;
 Wire.beginTransmission(i2cAddress);             // returns none
 if(Wire.write(3)!=1) return DS1307_ERR_NACK_DATA;  // start reg
 if(Wire.write(reg3)!=1) return DS1307_ERR_NACK_DATA;
 if(Wire.write(reg4)!=1) return DS1307_ERR_NACK_DATA;
 if(Wire.write(reg5)!=1) return DS1307_ERR_NACK_DATA;
 if(Wire.write(reg6)!=1) return DS1307_ERR_NACK_DATA;
 status_=Wire.endTransmission();
 return status_;
}

//_____write time into regs_____________________________________
// used by setTime()
// return: status_
int DS1307::writeTimeRegs(byte HH, byte MM, byte SS)
{
 byte bytes=0;
 //-----check time----------------------------------------------
 if(!isTimeOK(HH, MM, SS)) return status_;
 //-----time ok, save them as properties------------------------
 HH_=HH;
 HH=0x3F & HH;                                   // 24 hours
 MM_=MM;
 SS_=SS;
 SS=0x7F & SS;                                   // clock on
 //-----send time bytes to DS1307-------------------------------
 Wire.beginTransmission(i2cAddress);             // returns none
 bytes+=Wire.write(0);                           // start reg          
 bytes+=Wire.write(byte2bcd(SS));
 bytes+=Wire.write(byte2bcd(MM));
 bytes+=Wire.write(byte2bcd(HH));
 status_=Wire.endTransmission();
 if(bytes!=4) status_=DS1307_ERR_NACK_DATA;   
 return status_;
}

//_____read date from DS1307 and return it format yyyymmdd______
// return: date string or empty string ""
String DS1307::readDate() {
 String s1="";
 //-----Date: read bytes, starting on reg 3-----------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x03);
 status_=Wire.endTransmission();
 if(status_!=DS1307_OK) return s1;
 //-----and now read 4 bytes--------------------------------------
 if(Wire.requestFrom(i2cAddress, 4)<1) 
  { status_=DS1307_ERR_NO_BYTE; return s1; }
 byte reg3=Wire.read();
 byte reg4=Wire.read();
 byte reg5=Wire.read();
 byte reg6=Wire.read();
 status_=Wire.endTransmission();
 if(status_!=DS1307_OK) return s1;
 //-----build answer--------------------------------------------
 ww_=reg3-1;
 dd_=bcd2byte((reg4&0x3F));
 mm_=bcd2byte((reg5&0x1F));
 yy_=2000+bcd2byte(reg6);
 return date2string(yy_, mm_, dd_);
}

//_____read time from DS1307___________________________________
// return: time string or empty string ""
String DS1307::readTime() {
 String s1="";
 //-----Time: read bytes, starting on reg 0-----------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x00);
 status_=Wire.endTransmission();
 if(status_!=DS1307_OK) return s1;
 //-----and now read 3 bytes--------------------------------------
 if(Wire.requestFrom(i2cAddress, 3)<1) 
  { status_=DS1307_ERR_NO_BYTE; return s1; }
 SS_=bcd2byte(Wire.read());
 MM_=bcd2byte(Wire.read());
 HH_=bcd2byte((Wire.read()) &0x3F);
 status_=Wire.endTransmission();
 if(status_!=DS1307_OK) return s1;
 return time2string(HH_,MM_,SS_);
}


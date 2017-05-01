//_____D1_class_PCF8583.h_____________________170416-170430_____
// D1 mini class for i2c clock/calendar/counter IC.
// Default i2c 7bit-address is 0x50 (other option 0x51).
#ifndef D1_CLASS_PCF8583_H
#define D1_CLASS_PCF8583_H
#include <Wire.h>
#define PCF8583_ADDR    0x50      // 7-bit address
#define MODE_COUNTER    0x24      // counter + enable alarm
#define MODE_50HZ       0x14      // clock + enable alarm
#define MODE_32KHZ      0x04      // clock + enable alarm
#define BASE_YEAR       2016      // start value until setDate()
#define CLOCK_ALARM_NO       0x00 //
#define CLOCK_ALARM_DAILY    0x90 //
#define CLOCK_ALARM_WEEKLY   0xA0 //
//#define CLOCK_ALARM          0xB0 // dated = 1x
#define CLOCK_ALARM          0xF8 //

class PCF8583 {
 public:
  PCF8583();
  PCF8583(int i2c_address);
  int    getAddress();
  int    getStatus();
  int    setMode(byte mode_);
  //-----clock/calendar specials--------------------------------
  int    setDate(String yyyymmdd);
  int    setTime(String HHMMSS);
  String getDate();
  String getTime();
  String getWeekday();
  int    setAlarmMode(byte alarmmode);
  int    setAlarmDate(String yyyymmdd);
  int    setAlarmTime(String HHMMSS);
  bool   isClockAlarm();
  int    clearClockAlarm();
  //-----counter specials---------------------------------------
  int    setCounter(long counter_);
  long   getCounter();
  int    setCounterAlarm(long counter_);
  bool   isCounterAlarm();
  int    clearCounterAlarm();
  //-----public helper functions--------------------------------
  String date2string(int yyyy, int mm, int dd);
  String time2string(int hh, int mm, int ss);
 private:
  int i2cAddress;
  int status;
  int mode_;
  int y4_;                        // year (4 digits)
  int yB_;                        // base year y4_ - y4_%4
  int y0_;                        // year (0..3): y4_%4
  int mm_, dd_;                   // month (1..12), day (1..31)
  int HH_, MM_, SS_;              // hour (0..23),minute,seconds
  int ww_;                        // day of week 0=sunday, ..
 private:  
  void setup();
  int  bcd2byte(byte bcd);
  byte byte2bcd(int value8bit);
  int  isLeapyear(int year);
  byte getDayOfWeek();
  byte getDayOfWeek(const PCF8583 &now);
  bool isDateOK(int yyyy,int mm,int dd);
  //-----i2c access----------------------------------------------
  int  write1reg(byte reg, byte value);
  byte read1reg(byte reg);
  int  setDateRegs(String yyyymmdd, byte startreg);
  int  setTimeRegs(String HHMMSS, byte startreg); 
  int  setCounterRegs(long counter_, byte startreg);
};

//_____constructor (default 0x50)_______________________________
PCF8583::PCF8583() { i2cAddress=PCF8583_ADDR; setup(); }

//_____constructor with i2c Address_____________________________
PCF8583::PCF8583(int i2c_address) {
 i2cAddress=i2c_address; 
 setup(); 
}

//_____return i2c address_______________________________________
int PCF8583::getAddress() { return i2cAddress; }

//_____system status____________________________________________
// 0: success
// 1: data too long to fit in transmit buffer
// 2: received NACK on transmit of address
// 3: received NACK on transmit of data
// 4: other error 
// 5: no byte received
// 6: wrong number of chars
// 7: wrong range
int PCF8583::getStatus() { return status; }

//_____change mode______________________________________________
// return: status
int PCF8583::setMode(byte mode) {
 mode_=mode;
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x00);
 Wire.write(mode_);
 status=Wire.endTransmission();
 return status;
}

//_____send date to PCF8583_____________________________________
// return: status
int PCF8583::setDate(String yyyymmdd){
 return setDateRegs(yyyymmdd, 0x05);
}

//_____send time to PCF8583_____________________________________
// return: status
int PCF8583::setTime(String HHMMSS){
 return setTimeRegs(HHMMSS, 0x01);
}
  
//_____read date from PCF8583___________________________________
String PCF8583::getDate() {
 String s1="";
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x05);
 status=Wire.endTransmission();
 if(status!=0) return s1;
 if(Wire.requestFrom(i2cAddress, 2)<1) { status=5; return s1; }
 byte reg5=Wire.read();
 byte reg6=Wire.read();
 status=Wire.endTransmission();
 if(status!=0) return s1;
 //-----build answer--------------------------------------------
 y0_=reg5>>6;
 y4_=yB_+y0_;
 dd_=bcd2byte((reg5&0x3F));
 ww_=reg6>>5;
 mm_=bcd2byte((reg6&0x1F));
 return date2string(y4_, mm_, dd_);
}

//_____read time from PCF8583___________________________________
String PCF8583::getTime() {
 String s1="";
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x02);
 status=Wire.endTransmission();
 if(status!=0) return s1;
 if(Wire.requestFrom(i2cAddress, 3)<1) { status=5; return s1; }
 SS_=bcd2byte(Wire.read());
 MM_=bcd2byte(Wire.read());
 HH_=bcd2byte((Wire.read()&0x3F));           
 status=Wire.endTransmission();
 if(status!=0) return s1;
 return time2string(HH_,MM_,SS_);
}

//_____weekday as two chars (in German)_________________________
String PCF8583::getWeekday() {
 String weekday("SOMODIMIDOFRSA");
 ww_=read1reg(0x06)>>5;
 if((ww_<0)||(ww_>6)) return "??";
 return weekday.substring(2*ww_,2*ww_+2);
}

//_____write byte to alarm control register____________________
// return: status
int PCF8583::setAlarmMode(byte alarmmode){
 write1reg(0x08, alarmmode);
 return status; 
}

//_____send alarm date to PCF8583_______________________________
// return: status
int PCF8583::setAlarmDate(String yyyymmdd){
 return setDateRegs(yyyymmdd, 0x0D);
}

//_____send alarm time to PCF8583_______________________________
// return: status
int PCF8583::setAlarmTime(String HHMMSS){
 return setTimeRegs(HHMMSS, 0x09);
}

//_____clock alarm accured?_____________________________________
// on clock alarm flag is set (reg 0x00, bit 1)
// return: true = yes, false = no or error (see status)
bool PCF8583::isClockAlarm() {
 byte reg0=read1reg(0x00);
 if(status!=0) return false;
 if((reg0&0x02)>0) return true;
 return false;
}

//_____clear (reset) counter interrupt flag_____________________
// return: status
int PCF8583::clearClockAlarm(){
 return setMode(MODE_32KHZ);
}
  
//**************************************************************
// counter methods
//**************************************************************

//_____set counter value________________________________________
// return: status
int PCF8583::setCounter(long counter_){
 return setCounterRegs(counter_, 0x01);
}

//_____read counter value and convert to long___________________
// return: conter value, -1 on error
long PCF8583::getCounter() {
 long result=0;
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x01);
 status=Wire.endTransmission();
 if(status!=0) return -1;
 if(Wire.requestFrom(i2cAddress, 3)<1) { status=5; return -1; }
 result=bcd2byte(Wire.read());
 result=100*bcd2byte(Wire.read())+result;
 result=10000*bcd2byte(Wire.read())+result;
 status=Wire.endTransmission();
 return result; 
}

//_____set counter value________________________________________
// return: status
int PCF8583::setCounterAlarm(long counter_){
 status=write1reg(0x08, 0xD0);
 if(status!=0) return status;
 return setCounterRegs(counter_, 0x09);
}

//_____counter alarm accured?___________________________________
// on counteralarm alarm flag is set (reg 0x00, bit 1)
// return: true = yes, false = no alarm or error -> getStatus()
bool PCF8583::isCounterAlarm(){
 byte reg0=read1reg(0x00);
 if(status!=0) return false;
 if((reg0&0x02)>0) return true;
 return false;
}

//_____clear (reset) counter interrupt flag_____________________
// return: status
int PCF8583::clearCounterAlarm(){
 return setMode(MODE_COUNTER);
}

//-----public helper functions--------------------------------
//_____convert date values to string (add 0 if necessary)_____
String PCF8583::date2string(int yyyy, int mm, int dd) {
 String s1="";
 if(!isDateOK(yyyy,mm,dd)) return s1;
 s1=String(y4_);
 if(mm_<10) s1+="0";
 s1+=String(mm_);
 if(dd_<10) s1+="0";
 s1+=String(dd_);
 return s1;
}

//_____convert time values to string (add 0 if necessary)______
String PCF8583::time2string(int hh, int mm, int ss){
 String s1="";
 if((hh>23)||(hh<0)) return s1;
 if((mm>59)||(mm<0)) return s1;
 if((ss>59)||(ss<0)) return s1;
 if(hh<10) s1+="0";
 s1+=String(hh);
 if(mm<10) s1+="0";
 s1+=String(mm);
 if(ss<10) s1+="0";
 s1+=String(ss);
 return s1;
}

//**************************************************************
// private functions
//**************************************************************

//_____setup device PCF8583_____________________________________
void PCF8583::setup() {
 mode_=MODE_32KHZ;
 yB_=BASE_YEAR;
 Wire.begin();                    //(4,5): SDA on 4, SCL on 5
 status=setMode(mode_);
}

//_____convert bcd-byte to int value____________________________
int PCF8583::bcd2byte(byte bcd){
 return ((bcd >> 4) * 10) + (bcd & 0x0f);
}

//_____convert 8 bit value (00..99) to bcd______________________
// if value out of range: return 0
byte PCF8583::byte2bcd(int value8bit) {
 if((value8bit<0)||(value8bit>99)) return 0;
 return ((value8bit / 10) << 4) + (value8bit % 10);
}

//_____is year (4 digits) a leapyear?___________________________
int PCF8583::isLeapyear(int year){
 if(!(year % 400) || ((year % 100) && !(year % 4))) return 1;
 return 0;
}

//_____calculate day of week for this object____________________
byte PCF8583::getDayOfWeek() { return getDayOfWeek(*this); }

//_____calculate day of week____________________________________
byte PCF8583::getDayOfWeek(const PCF8583 &now) {
 char MonthTable[24]=  
  {0,3,3,6,1,4,6,2,5,0,3,5,-1,2,3,6,1,4,6,2,5,0,3,5};
 int y = now.y4_%100;
 int c = 6 - 2 * ((now.y4_/100)%4); 
 int tmp_=now.dd_ + y + (y / 4) + c;
 tmp_+=MonthTable[isLeapyear(now.y4_)*12+now.mm_-1];
 return (tmp_ % 7);
}

//_____check if date data is ok_________________________________
bool PCF8583::isDateOK(int yyyy,int mm,int dd){
 if((yyyy<1900)||(yyyy>9999)) return false;
 if((mm<1)||(mm>12)) return false;
 if((dd<1)||(dd>31)) return false;
 if(dd>30) {
  if((mm==4)||(mm==6)||(mm==9)||(mm==11)) return false;
 }
 if(mm==2) {
  if((dd>29)||((isLeapyear(yyyy)==0)&&(dd>28))) return false;
 }
 return true;
}

//_____write the byte to address reg____________________________
// return: status
int PCF8583::write1reg(byte reg, byte value) {
 Wire.beginTransmission(i2cAddress);
 Wire.write(reg);                 // register
 Wire.write(value);               // value
 status=Wire.endTransmission();
 return status;
}

//_____read the byte on address reg_____________________________
byte PCF8583::read1reg(byte reg) {
 Wire.beginTransmission(i2cAddress);
 Wire.write(reg);                 // control register
 status=Wire.endTransmission();
 if(status!=0) return 0;
 if(Wire.requestFrom(i2cAddress, 1)<1){status=5; return 0;}
 byte reg0=Wire.read();
 status=Wire.endTransmission();
 if(status!=0) return 0;
 return reg0;
}

//_____copy date in regs beginning with startreg________________
// used by setDate(), setDateAlarm()
// return: status
int PCF8583::setDateRegs(String yyyymmdd, byte startreg) { 
 int iyyyy,imm,idd;
 //-----break up date string and check values-------------------
 if(yyyymmdd.length()!=8) { status=6; return status; }
 iyyyy=(yyyymmdd.substring(0,4)).toInt();
 imm=(yyyymmdd.substring(4,6)).toInt();
 idd=(yyyymmdd.substring(6,8)).toInt();
 if(!isDateOK(iyyyy,imm,idd)) { status=7; return status; }
 //-----input values ok-----------------------------------------
 y4_=iyyyy;
 y0_=y4_%4;                       // year offset (0..3)
 yB_=y4_-y0_;                     // base year
 mm_=imm;
 dd_=idd;
 //-----generate bytes------------------------------------------
 byte reg5=byte2bcd(dd_);
 reg5=reg5|(y0_<<6);
 byte reg6=byte2bcd(mm_);
 ww_=getDayOfWeek(*this);
 reg6=(ww_<<5)|reg6;
 //-----send date bytes to PCF8583------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(startreg);
 Wire.write(reg5);
 Wire.write(reg6);
 status=Wire.endTransmission();
 return status;
}

//_____copy time in regs beginning with startreg________________
// used by setTime(), setTimeAlarm()
// return: status
int PCF8583::setTimeRegs(String HHMMSS, byte startreg) {
 int iiH,iiM,iiS;
 //-----break up time string------------------------------------
 if(HHMMSS.length()!=6) { status=6; return status; }
 iiH=(HHMMSS.substring(0,2)).toInt();
 if((iiH<0)||(iiH>23)) { status=7; return status; }
 iiM=(HHMMSS.substring(2,4)).toInt();
 if((iiM<0)||(iiM>59)) { status=7; return status; }
 iiS=(HHMMSS.substring(4,6)).toInt();
 if((iiS<0)||(iiS>59)) { status=7; return status; }
 HH_=iiH;
 MM_=iiM;
 SS_=iiS;
 //-----send date bytes to PCF8583------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(startreg);
 Wire.write(0x00);                     // millisec=0
 Wire.write(byte2bcd(SS_));
 Wire.write(byte2bcd(MM_));
 Wire.write((byte2bcd(HH_)&0x3F));    // 24 hour format!
 status=Wire.endTransmission();
 return 0;
}

//_____copy counter value in regs beginning with startreg_______
// used by setCounter(), setCounterAlarm()
// return: status
int PCF8583::setCounterRegs(long counter_, byte startreg) { 
 byte tmp_, reg1, reg2, reg3;
 if((counter_<0)||(counter_>999999)) {status=7; return status;}
 //-----convert long to 3x bcd----------------------------------
 tmp_=counter_%100;
 reg1=byte2bcd(tmp_);
 counter_=counter_/100;
 tmp_=counter_%100;
 reg2=byte2bcd(tmp_);
 tmp_=counter_/100;
 reg3=byte2bcd(tmp_);
 //-----send data-----------------------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(startreg);
 Wire.write(reg1);
 Wire.write(reg2);
 Wire.write(reg3);
 status=Wire.endTransmission();
 return status;
}

#endif

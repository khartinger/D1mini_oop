# D1 mini: Examples using class DS1307 (Real Time Clock)
Sketch: -, Version 2017-11-27   
[Deutsche Version](./LIESMICH.md "Deutsche Version")

This directory contents examples, how to use the class **DS1307** to exchange data with a real time clock (RTC) DS1307 via i2c interface.  The class is just for communication with the DS1307 clock and does not regard daylight saving time (= summer time), etc.      

**Info**   
* D1_oop23_DS1307_demo: Use different methods to write and read date, time or RAM. Print results to Serial.
* D1_oop23_DS1307_setSerial: Set date and time by Serial and show values on Serial.
* D1_oop23_DS1307_setTimeHttp: Set date and time to a default value (send r by Serial) or get values from a server (e.g. gateway) (send character s). Print date and time on Serial.

## Hardware
* WeMos D1 mini
* Data logger shield (microSD + DS1307RTC Shield)   
  DS1307  uses I2C (D1-SCL, D2-SDA)   
  microSD uses SPI (D5-CLK, D6-MISO, D7-MOSI, D8-CS)   


![Image: D1mini microSD + DS1307 shield](./images/D1_microSD_DS1307_shield.png "D1mini microSD + DS1307 shield")


## Class diagram DS1307
| class DS1307 |     |
| --------------------------------- | --- |
| + __DS1307__()                    | Constructor: set default i2c address, setup()   |
| + DS1307(int i2c_address)         | Constructor, set given i2c address, setup()     |
| + int  getAddress()               | get i2c address (default is 0x68)               |
| + int  __getStatus__()            | get status: <br>0: success, 1: data too long to fit in transmit buffer, 2: received NACK on transmit of address, <br>3: received NACK on transmit of data, 4: other error, 5: i2c wrong number of bytes, <br>6: no byte received, 7: wrong number of chars, 8: wrong range, 9: error reading RAM |

&nbsp;

|  *clock/calendar specials*                    |                      |
| --------------------------------------------- | -------------------- |
| + int setDate(String yyyymmdd)                | write date to DS1307 |
| + int setDate(int yyyy, byte mm, byte dd)     | write date to DS1307 |
| + int setTime(String HHMMSS)                  | write time to DS1307 |
| + int setTime(byte HH, byte MM, byte SS)      | write time to DS1307 |
| + int __setDateTime(String yyyymmdd_HHMMSS)__ | write date and time  to DS1307 |
| + int setSecs1970(unsigned long secs1970)     | convert seconds since 1.1.1970 to date and time and write them to DS1307 |
| + unsigned long getSecs1970()     | read date and time from DS1307 and convert them to seconds since 1.1.1970 |
| + String getDate()               | read date, convert to format yyyymmdd   |
| + String getDate2()              | read date, convert to format dd.mm.yyyy |
| + String getTime()               | read time, convert to format HHMMSS     |
| + String getTime2()              | read time, convert to format HH:MM:SS   |
| + String __getDateTime()__       | read date and time, convert them to yyyymmdd HHMMSS |
| + String __getDateTime2()__      | read date and time, convert them to dd.mm.yyyy HH:MM:SS |
| + String getWeekday()            | delivers German short cuts for weekday (SO, MO, DI, MI, DO, FR, SA or ??) |

&nbsp;

| *clock/calendar data without DS1307 access (properties)*     |     |
| ------------------------------------------------------------ | --- |
| + String getLastDate()            | last date, format yyyymmdd, no DS1307 access   |
| + String getLastDate2()           | last date, format dd.mm.yyyy, no DS1307 access |
| + String getLastTime()            | last time, format HHMMSS, no DS1307 access     |
| + String getLastTime2()           | last time, format HH:MM:SS, no DS1307 access   |
| + String getLastDateTime()        | last date and time, format yyyymmdd HHMMSS, no DS1307 access |
| + String getLastDateTime2()       | last date and time, format dd.mm.yyyy HH:MM:SS, no DS1307 access |
| + String getLastWeekday()         | last german short cut for weekday, no DS1307 access  |
| + unsigned long getLastSecs1970() | convert last date and time to seconds since 1.1.1970 |

&nbsp;
     
| *use DS1307 RAM*                             | *(address 0x08...0x3F)*        |
| -------------------------------------------- | ------------------------------ |
| + int  clearRAM()                            | set RAM to 0, uses clearRAM(initbyte) |
| + int  clearRAM(byte initbyte)               | set RAM to initbyte, check if RAM contains the value (return: status) |
| + int  setRAMbyte(int address, byte data)    | write data byte to DS1307 RAM  |
| + byte getRAMbyte(int address)               | read data byte from DS1307 RAM |
| + int  setRAMint16(int address, int int16)   | write int value to DS1307 RAM  |
| + int  getRAMint16(int address)              | read int value from DS1307 RAM |

&nbsp;

| *public helper methodes*                        |     |
| ----------------------------------------------- | --- |
| + String date2string(int yyyy, int mm, int dd)  | convert date values to string yyyymmdd   |
| + String date2string2(int yyyy, int mm, int dd) | convert date values to string dd.mm.yyyy |
| + String time2string(int hh, int mm, int ss)    | convert time values to string HHMMSS     |
| + String time2string2(int hh, int mm, int ss)   | convert time values to string HH:MM:SS   |
| + bool   isLeapyear(int year)                   | is year (4 digits) a leapyear? |
| + bool   isSummertime(int yyyy, byte mm, byte dd, byte HH, byte timezone)| true=yes, date/time is daylight saving time (DST) = summertime) | 
| + bool   __isSummertime__(byte timezone)        | true=yes, date in object is daylight saving time (DST) = summertime) |

&nbsp;

| *protected helper methods*                    |     |
| --------------------------------------------- | --- |
| ~ void setup()                                | start clock, set 24 hour mode, init properties |
| ~ int  bcd2byte(byte bcd)                     | convert bcd-byte to int value |
| ~ byte byte2bcd(int value8bit)                | convert 8 bit value (00..99) to bcd |
| ~ byte getDayOfWeek()                         | calculate day of week for given object 0=SO, 1=MO, 2=DI, 3=MI, 4=DO, 5=FR, 6=SA |
| ~ byte getDayOfWeek(int yyyy, int mm, int dd) | calculate day of week for given values |
| ===== *check data* =====                      |     |
| ~ bool isDateOK(int yyyy, int mm, int dd)     | check if date values are ok |
| ~ bool isDateOK(String yyyymmdd)              | check if date values are ok |
| ~ bool isTimeOK(int HH, int MM, int SS)       | check if time values are ok |
| ~ bool isTimeOK(String HHMMSS)                | check if time values are ok |
| ===== *i2c access* =====                      |     |
| ~ int  write1reg(byte reg, byte value)        | write byte to address reg (return: status) |
| ~ byte read1reg(byte reg)                     | read one byte from address reg |
| ~ int  writeDateRegs(String yyyymmdd)         | copy date into regs |
| ~ int  writeTimeRegs(String HHMMSS)           | copy time into regs |
| ~ String readDate()                           | copy date into regs, return yyyymmdd |
| ~ String readTime()                           | copy time into regs, return HHMMSS |

&nbsp;

|  *properties*               |                                     |
| --------------------------- | ----------------------------------- |
| - int  i2cAddress;          | used i2c address (default 0x68)     |
| - int  status;              | 0=ok, >0 error                      |
| - int  yy_;                 | year (00..99)                       |
| - int  mm_, dd_;            | month (1..12), day (1..31)          |
| - int  HH_, MM_, SS_;       | hour (0..23),minute (0..59), seconds (0..59) |
| - int  ww_;                 | day of week 0=sunday, 1=munday, ... |

//_____D1_class_DS1307.h______________________171127-171127_____
// D1 mini class for access to a RTC DS1307 by i2c.
// The class is just for communication with a DS1307 clock and 
// does not regard daylight saving time (=summer time) etc.
// Default i2c 7bit-address is 0x68.
// Created by Karl Hartinger, November 27, 2017.
// Released into the public domain.
#ifndef D1_CLASS_DS1307_H
#define D1_CLASS_DS1307_H
#include "Arduino.h"              // 
#include <Wire.h>                 // for I2C
#define DS1307_ADDR     0x68      // 7-bit address
#define DS1307_RAMBEGIN 0x08      // start of RAM (56 byte)
#define DS1307_RAMEND   0x3F      // end   of RAM (56 byte)
//-----status---------------------------------------------------
#define DS1307_OK            0    // success
#define DS1307_ERR_TOO_LONG  1    // i2c data too long to fit in transmit buffer 
#define DS1307_ERR_NACK_ADDR 2    // i2c NACK transmit address
#define DS1307_ERR_NACK_DATA 3    // i2c NACK transmit data
#define DS1307_ERR_OTHER     4    // i2c other error
#define DS1307_ERR_NUM_BYTES 5    // i2c wrong number of bytes
#define DS1307_ERR_NO_BYTE   6    // no byte received
#define DS1307_ERR_NUM_CHARS 7    // wrong number of chars
#define DS1307_ERR_RANGE     8    // wrong range
#define DS1307_ERR_RAM       9    // error reading RAM

class DS1307 {
 //-----properties----------------------------------------------
 protected:
  int    i2cAddress;              // used i2c address (def. 0x68)
  int    status_;                 // 0=ok, >0 error
  int    yy_;                     // year (00..99) 
  byte   mm_, dd_;                // month (1..12), day (1..31)
  byte   HH_, MM_, SS_;           // hour (0..23),minute,seconds
  byte   ww_;                     // day of week 0=sunday, ..
  //-----constructor(s)-----------------------------------------
 public:
  DS1307();                       // Constructor, set default
  DS1307(int i2c_address);        // Constructor, set i2c addr.
  //-----general setter and getter methods----------------------
  int    getAddress();            // get i2c address (0x68)
  int    getStatus();             // get status 0=OK, >0 error
  //-----clock/calendar specials read from DS1307---------------
  int    setDate(String yyyymmdd);// write date to DS1307
  int    setDate(int yyyy, byte mm, byte dd);
  int    setTime(String HHMMSS);  // write time to DS1307
  int    setTime(byte HH, byte MM, byte SS);
  int    setDateTime(String yyyymmdd_HHMMSS);
  int    setSecs1970(unsigned long secs1970);
  unsigned long getSecs1970();              //sec since 1.1.1970 
  String getDate() {return readDate();}     // format yyyymmdd
  String getDate2();                        // format dd.mm.yyyy
  String getTime() {return readTime();}     // format HHMMSS
  String getTime2();                        // format HH:MM:SS
  String getDateTime();                     // yyyymmdd HHMMSS
  String getDateTime2();                    // dd.mm.yyyy HH:MM:SS
  String getWeekday();                      //German SO,.. or ??
  //-----clock/calendar data without DS1307 access (properties)-
  String getLastDate();                     // no DS1307 access
  String getLastDate2();                    // no DS1307 access
  String getLastTime();                     // no DS1307 access
  String getLastTime2();                    // no DS1307 access
  String getLastDateTime();                 // no DS1307 access
  String getLastDateTime2();                // no DS1307 access
  String getLastWeekday();                  // no DS1307 access
  unsigned long getLastSecs1970();          //properties to secs
  //-----use DS1307 RAM (address 0x08...0x3F)-------------------
  int    clearRAM();                        // write 0 to RAM
  int    clearRAM(byte initbyte);           // write byte to RAM
  int    setRAMbyte(int address, byte data);// write byte to RAM
  byte   getRAMbyte(int address);           //read byte from RAM
  int    setRAMint16(int address,int int16);// write int to RAM
  int    getRAMint16(int address);          // read int from RAM
  //-----public helper functions--------------------------------
  String date2string(int yyyy, byte mm, byte dd);  // yyyymmdd
  String date2string2(int yyyy, byte mm, byte dd); // dd.mm.yyyy
  String time2string(byte hh, byte mm, byte ss);   // hhmmss
  String time2string2(byte hh, byte mm, byte ss);  // hh:mm:ss
  bool   isLeapyear(int year);                     // 
  bool   isSummertime(int yyyy, byte mm, byte dd,  //
         byte HH, byte timezone);
  bool   isSummertime(byte timezone);              //
  //-----protected helper methodes------------------------------
 protected:
  void   setup();                      // setup properties
  byte   bcd2byte(byte bcd);           // convert BCD to byte
  byte   byte2bcd(byte value8bit);     // convert byte to BCD
  byte   getDayOfWeek(int yyyy, byte mm, byte dd); // 0=SO...6=SA
  //-----check data---------------------------------------------
  bool   isDateOK(int yyyy, byte mm, byte dd);
  bool   isDateOK(String yyyymmdd);
  bool   isTimeOK(byte HH, byte MM, byte SS);
  bool   isTimeOK(String hhmmss);
  //-----i2c access---------------------------------------------
  int    write1reg(byte reg, byte value);
  byte   read1reg(byte reg);
  int    writeDateRegs(int yyyy, byte mm, byte dd);
  int    writeTimeRegs(byte HH, byte MM, byte SS); 
  String readDate();              // format yyyymmdd
  String readTime();              // format HHMMSS
};
#endif

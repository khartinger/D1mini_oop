//_____D1_class_SHT30.h_______________________180129-190210_____
// D1 mini class for temperature and humidity sensor SHT30.
// SHT30: Temperature -40°C...125°C +-0,7% (0°..+65°C, +-0,3°C)
//        Humidity    +-3%RH
// Default i2c address is 0x45 (other 0x44)
// See also https://github.com/ControlEverythingCommunity/CE_ARDUINO_LIB/blob/master/SHT30/SHT30.cpp

#ifndef D1_CLASS_SHT30_H
#define D1_CLASS_SHT30_H
#include "Arduino.h"              // 
#include <Wire.h>
#define SHT30_ADDR          0x45  // 7-bit address
#define MEASURING_DELAY_MS  500   // time between 2 measurements
#define SHT30_ERR_VALUE     -999
#define SHT30_FT(tmp) ((175.0 * (tmp) / 65535.0) - 45)
#define SHT30_FH(hum) (100.0 * (hum) / 65535.0)

//-----status---------------------------------------------------
#define SHT30_OK            0     // success
#define SHT30_ERR_TOO_LONG  1     // i2c data too long to fit in transmit buffer 
#define SHT30_ERR_NACK_ADDR 2     // i2c NACK transmit address
#define SHT30_ERR_NACK_DATA 3     // i2c NACK transmit data
#define SHT30_ERR_OTHER     4     // i2c other error
#define SHT30_ERR_NUM_BYTES 5     // i2c wrong number of bytes
#define SHT30_ERR_NO_BYTE   6     // no byte received
#define SHT30_ERR_NUM_CHARS 7     // wrong number of chars
#define SHT30_ERR_RANGE     8     // wrong range
#define SHT30_ERR_CRC       9     // wrong crc
#define SHT30_ERR_NO_MEAS   10    // no measure until now

class SHT30 {
 //-----properties----------------------------------------------
 protected:
  int i2cAddress;
  int status;
  unsigned long next_measuring_;
  unsigned int hum_;
  unsigned int tmp_;
 //-----constructor & co----------------------------------------
 public:
  SHT30();
  SHT30(int i2c_address);
 protected:
  void   setup();
 public:
  void   begin();
  void   softReset();
 //-----setter and getter methods-------------------------------
 public:
  int    getStatus();
  String getsStatus();
  float  getTemperature();
  String getsTemperature(int len, int decimals);
  float  getHumidity();
  String getsHumidity(int len, int decimals);
  int    getValues(float &temperature, float &humidity);
 //-----get (last) values without making a measurement----------
  String getsLastTemperature(int len, int decimals);
  String getsLastHumidity   (int len, int decimals);
  int    getLastValues(float &temperature, float &humidity); 
 //-----helper functions----------------------------------------
 protected:
  void    measuring();
  unsigned char calcCRC(unsigned char *idata, int len);
  String  float2String(float f, int len, int decimals);
};
#endif

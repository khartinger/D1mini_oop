//_____D1_class_INA226.h______________________180216-180220_____
/*
Basic code: https://github.com/jarzebski/Arduino-INA226
This program is free software: you can redistribute it and/or 
modify it under the terms of the version 3 GNU General Public 
License as published by the Free Software Foundation.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/
*/
// Modified by Karl Hartinger, February 18, 2018.
// Default i2c 7bit-address is 0x40.

#ifndef D1_CLASS_INA226_H
#define D1_CLASS_INA226_H
#include "Arduino.h"                        // 
#include <Wire.h>                           // for I2C
#include <math.h>                           // NAN

//-----status---------------------------------------------------
#define INA226_OK            0    // success
#define INA226_ERR_TOO_LONG  1    // i2c data too long to transm
#define INA226_ERR_NACK_ADDR 2    // i2c NACK transmit address
#define INA226_ERR_NACK_DATA 3    // i2c NACK transmit data
#define INA226_ERR_OTHER     4    // i2c other error
#define INA226_ERR_NUM_BYTES 5    // i2c wrong number of bytes

//-----CJMCU-226 board values-----------------------------------
#define INA226_ADDRESS            (0x40)
#define INA226_RSHUNT             (0.1)    // Ohm
#define INA226_MAX_CURRENT        (0.8192) // Ampere
#define INA226_SHUNTVOLTAGE_LSB   (0.0000025)
#define INA226_BUSVOLTAGE_LSB     (0.00125)
#define INA226_FACTOR_CURRENT     (1.0)

//-----register addresses---------------------------------------
#define INA226_REG_CONFIG         (0x00)
#define INA226_REG_SHUNTVOLTAGE   (0x01)
#define INA226_REG_BUSVOLTAGE     (0x02)
#define INA226_REG_POWER          (0x03)
#define INA226_REG_CURRENT        (0x04)
#define INA226_REG_CALIBRATION    (0x05)
#define INA226_REG_MASKENABLE     (0x06)
#define INA226_REG_ALERTLIMIT     (0x07)
#define INA226_REG_MANUFACTURER   (0xFE)
#define INA226_REG_ID             (0xFF)

//----- Mask/Enable Register Bits (Reg 06h, Read/Write)---------
#define INA226_BIT_SOL            (0x8000)
#define INA226_BIT_SUL            (0x4000)
#define INA226_BIT_BOL            (0x2000)
#define INA226_BIT_BUL            (0x1000)
#define INA226_BIT_POL            (0x0800)
#define INA226_BIT_CNVR           (0x0400)
#define INA226_BIT_AFF            (0x0010)
#define INA226_BIT_CVRF           (0x0008)
#define INA226_BIT_OVF            (0x0004)
#define INA226_BIT_APOL           (0x0002)
#define INA226_BIT_LEN            (0x0001)

//-----number of samples average--------------------------------
typedef enum
{
 INA226_AVERAGES_1             = 0b000,
 INA226_AVERAGES_4             = 0b001,
 INA226_AVERAGES_16            = 0b010,
 INA226_AVERAGES_64            = 0b011,
 INA226_AVERAGES_128           = 0b100,
 INA226_AVERAGES_256           = 0b101,
 INA226_AVERAGES_512           = 0b110,
 INA226_AVERAGES_1024          = 0b111
} ina226_averages_t;

//-----bus conversation time------------------------------------
typedef enum
{
 INA226_BUS_CONV_TIME_140US    = 0b000,
 INA226_BUS_CONV_TIME_204US    = 0b001,
 INA226_BUS_CONV_TIME_332US    = 0b010,
 INA226_BUS_CONV_TIME_588US    = 0b011,
 INA226_BUS_CONV_TIME_1100US   = 0b100,
 INA226_BUS_CONV_TIME_2116US   = 0b101,
 INA226_BUS_CONV_TIME_4156US   = 0b110,
 INA226_BUS_CONV_TIME_8244US   = 0b111
} ina226_busConvTime_t;

//-----shunt conversation time----------------------------------
typedef enum
{
 INA226_SHUNT_CONV_TIME_140US  = 0b000,
 INA226_SHUNT_CONV_TIME_204US  = 0b001,
 INA226_SHUNT_CONV_TIME_332US  = 0b010,
 INA226_SHUNT_CONV_TIME_588US  = 0b011,
 INA226_SHUNT_CONV_TIME_1100US = 0b100,
 INA226_SHUNT_CONV_TIME_2116US = 0b101,
 INA226_SHUNT_CONV_TIME_4156US = 0b110,
 INA226_SHUNT_CONV_TIME_8244US = 0b111
} ina226_shuntConvTime_t;

//-----mode-----------------------------------------------------
typedef enum
{
 INA226_MODE_POWER_DOWN        = 0b000,
 INA226_MODE_SHUNT_TRIG        = 0b001,
 INA226_MODE_BUS_TRIG          = 0b010,
 INA226_MODE_SHUNT_BUS_TRIG    = 0b011,
 INA226_MODE_ADC_OFF           = 0b100,
 INA226_MODE_SHUNT_CONT        = 0b101,
 INA226_MODE_BUS_CONT          = 0b110,
 INA226_MODE_SHUNT_BUS_CONT    = 0b111,
} ina226_mode_t;


class INA226
{
 //-----properties----------------------------------------------
 protected:
  int    i2cAddress;              // used i2c address (def 0x40)
  int    status_;                 // state of measuring
  double currentLSB;              // 1 bit = ... A
  double powerLSB;                // 1 bit = ... W
  double rShunt;                  // 0.1 Ohm @ CJMCU-226 board
  double vBusMax;                 // max. bus voltage 36V
  double vShuntMax;               // max. shunt voltage 0,08192V
  double factorCurrent;           // adjust current value (1.0)
 //-----constructor(s) & co-------------------------------------
 public:
  INA226();                       // Constructor, set default
  INA226(int i2c_address);        // Constructor, set i2c addr.
  bool begin();                   // init i2c, set i2c address
 //-----most commonly used methodes-----------------------------
  double readShuntCurrent(void);  // shunt current in A
  double readBusVoltage(void);    // bus voltage (0...+36V)
  double readBusPower(void);      // power in W
  double readShuntVoltage(void);  // A/D shunt voltage in V

 //-----configure INA226 and calibrate current in INA226--------
  bool configure
   (ina226_averages_t avg=INA226_AVERAGES_1,
   ina226_busConvTime_t busConvTime=INA226_BUS_CONV_TIME_1100US,
   ina226_shuntConvTime_t shuntConvTime=INA226_SHUNT_CONV_TIME_1100US,
   ina226_mode_t mode=INA226_MODE_SHUNT_BUS_CONT);
  bool calibrateCurrent
   (double rShuntValue=0.1, double iMaxExcepted=0.8192);

 //-----set methods---------------------------------------------
  void setFactorCurrent(double factor);
  void enableShuntOverLimitAlert(void);
  void enableShuntUnderLimitAlert(void);
  void enableBusOvertLimitAlert(void);
  void enableBusUnderLimitAlert(void);
  void enableOverPowerLimitAlert(void);
  void enableConversionReadyAlert(void);

  void setBusVoltageLimit(double voltage);
  void setShuntVoltageLimit(double voltage);
  void setPowerLimit(double watts);

  void setAlertInvertedPolarity(bool inverted);
  void setAlertLatch(bool latch);

 //-----get methods---------------------------------------------
  double getCurrentLSB() { return currentLSB; }
  double getPowerLSB()   { return powerLSB; }
  double getRShunt()     { return rShunt; }
  
  bool isMathOverflow(void);
  bool isAlert(void);

  ina226_averages_t      getAverages(void);
  ina226_busConvTime_t   getBusConversionTime(void);
  ina226_shuntConvTime_t getShuntConversionTime(void);
  ina226_mode_t          getMode(void);

  double getMaxPossibleCurrent(void);
  double getMaxCurrent(void);
  double getMaxShuntVoltage(void);
  double getMaxPower(void);

 //-----config info as string messages--------------------------
 String getsMode();
 String getsAverages();
 String getsBusConversionTime();
 String getsShuntConversionTime();
 
 //-----helper functions----------------------------------------
 protected:
  void     setMaskEnable(uint16_t mask);
  uint16_t getMaskEnable(void);

 //-----i2c access----------------------------------------------
 protected:
  int      writeRegister16(uint8_t reg, uint16_t val);
  int16_t  readRegister16(uint8_t reg);
};

#endif

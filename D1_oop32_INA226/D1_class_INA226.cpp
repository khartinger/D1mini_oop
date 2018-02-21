//_____D1_class_INA226.cpp____________________180216-180220_____
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
#include "D1_class_INA226.h"

//**************************************************************
//*     constructor(s) & co                                    *
//**************************************************************
//_____constructor (default 0x40)_______________________________
INA226::INA226() { i2cAddress=INA226_ADDRESS; begin(); }

//_____constructor with i2c Address_____________________________
INA226::INA226(int i2c_address) { 
 i2cAddress=i2c_address; 
 begin();
}

//_____start i2c bus____________________________________________
bool INA226::begin()
{
 Wire.begin();                    // start i2c
 factorCurrent = INA226_FACTOR_CURRENT;
 vBusMax = 36;                    // see INA226 datasheet
 vShuntMax = 0.081920;            // see INA226 datasheet
 //-----configure INA226----------------------------------------
 configure(INA226_AVERAGES_4, INA226_BUS_CONV_TIME_1100US,
  INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
 //-----Calibrate INA226: set Rshunt, max. expected current-----
 calibrateCurrent(INA226_RSHUNT, INA226_MAX_CURRENT);
 return true;
}

//**************************************************************
//*     most commonly used methodes                            *
//**************************************************************
//_____read current (ampere)____________________________________
// return: current in Ampere or NAN on error
double INA226::readShuntCurrent(void) {
 int16_t  reg16;
 uint16_t calValue;
 //-----get content of current calibration register-------------
 reg16=readRegister16(INA226_REG_CALIBRATION);
 if(status_!=INA226_OK) return NAN;
 //-----is calibration register zero?---------------------------
 if(reg16<1)
 { //.....do current calibration again..........................
  calValue = (uint16_t)((0.00512) / (currentLSB * rShunt));
  status_=writeRegister16(INA226_REG_CALIBRATION, calValue);
  if(status_!=INA226_OK) return NAN;
 }
 //-----get current---------------------------------------------
 reg16=readRegister16(INA226_REG_CURRENT);
 if(status_!=INA226_OK) return NAN;
 return(reg16*currentLSB*factorCurrent);
}

//_____read voltage (volt)______________________________________
// return: bus voltage in Volt or NAN on error
double INA226::readBusVoltage(void)
{
 int16_t voltage16;
 voltage16 = readRegister16(INA226_REG_BUSVOLTAGE);
 //-----was i2c communication ok?-------------------------------
 if(status_!=INA226_OK) return NAN;
 return(voltage16*INA226_BUSVOLTAGE_LSB);
}

//_____read power (watt)________________________________________
// return: power in Watt or NAN on error
double INA226::readBusPower(void) {
 int16_t  reg16;
 uint16_t calValue;
 //-----get content of current calibration register-------------
 reg16=readRegister16(INA226_REG_CALIBRATION);
 if(status_!=INA226_OK) return NAN;
 //-----is calibration register zero?---------------------------
 if(reg16<1)
 { //.....do current calibration again..........................
  calValue = (uint16_t)((0.00512) / (currentLSB * rShunt));
  status_=writeRegister16(INA226_REG_CALIBRATION, calValue);
  if(status_!=INA226_OK) return NAN;
 }
 //-----get power-----------------------------------------------
 reg16=readRegister16(INA226_REG_POWER);
 if(status_!=INA226_OK) return NAN;
 return(reg16*powerLSB*factorCurrent);
}

//_____read shunt voltage_______________________________________
// return: shunt voltage in Volt or NAN on error
double INA226::readShuntVoltage(void)
{
 double voltage16;
 voltage16 = readRegister16(INA226_REG_SHUNTVOLTAGE);
 //-----was i2c communication ok?-------------------------------
 if(status_!=INA226_OK) return NAN;
 return(voltage16*INA226_SHUNTVOLTAGE_LSB);
}

//**************************************************************
//*     configure and calibrate INA226                         *
//**************************************************************
//_____configure ina226_________________________________________
bool INA226::configure(ina226_averages_t avg,
                       ina226_busConvTime_t busConvTime,
                       ina226_shuntConvTime_t shuntConvTime,
                       ina226_mode_t mode)
{
 uint16_t config = 0;
 config |= (avg << 9 | busConvTime << 6 | shuntConvTime << 3 | mode);
 writeRegister16(INA226_REG_CONFIG, config);
 return true;
}

//_____calibrate ina226_________________________________________
bool INA226::calibrateCurrent
 (double rShuntValue, double iMaxExpected)
{
 uint16_t calibrationValue;       // CAL value for cal-reg
 double iMaxPossible;             // max possible current
 double minimumLSB;               // 
 rShunt = rShuntValue;            // save shunt value (0.1 ohm)
 iMaxPossible = vShuntMax/rShunt; // 0.8192A @ CJMCU-226 board
 if(iMaxExpected>iMaxPossible) iMaxExpected=iMaxPossible;
 minimumLSB = iMaxExpected/32767; // min. LSB current
 currentLSB = (uint16_t)(minimumLSB * 100000000);
 currentLSB /= 100000000;
 currentLSB /= 0.00001;           // old 0.0001
 currentLSB = ceil(currentLSB);
 currentLSB *= 0.00001;           // old 0.0001
 powerLSB = currentLSB * 25;
 //Serial.print("\nminimumLSB=");Serial.println(minimumLSB,9);
 //Serial.print("currentLSB=");Serial.println(currentLSB,9);
 calibrationValue = (uint16_t)((0.00512) / (currentLSB*rShunt));
 writeRegister16(INA226_REG_CALIBRATION, calibrationValue);
 return true;
}

//**************************************************************
//*     set methods                                            *
//**************************************************************
//_____set factor for current (hardware specific)_______________
// default value should be 1
void INA226::setFactorCurrent(double factor)
{ factorCurrent = factor; }

void INA226::enableShuntOverLimitAlert(void)
{ writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_SOL); }

void INA226::enableShuntUnderLimitAlert(void)
{ writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_SUL); }

void INA226::enableBusOvertLimitAlert(void)
{ writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_BOL); }

void INA226::enableBusUnderLimitAlert(void)
{ writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_BUL); }

void INA226::enableOverPowerLimitAlert(void)
{ writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_POL); }

void INA226::enableConversionReadyAlert(void)
{ writeRegister16(INA226_REG_MASKENABLE, INA226_BIT_CNVR); }

void INA226::setBusVoltageLimit(double voltage)
{ 
 uint16_t value = voltage / 0.00125;
 writeRegister16(INA226_REG_ALERTLIMIT, value);
}

void INA226::setShuntVoltageLimit(double voltage)
{
 uint16_t value = voltage * 25000;
 writeRegister16(INA226_REG_ALERTLIMIT, value);
}

void INA226::setPowerLimit(double watts)
{
 uint16_t value = watts / powerLSB;
 writeRegister16(INA226_REG_ALERTLIMIT, value);
}

void INA226::setAlertInvertedPolarity(bool inverted)
{
 uint16_t temp = getMaskEnable();
 if (inverted) temp |= INA226_BIT_APOL;
          else temp &= ~INA226_BIT_APOL;
 setMaskEnable(temp);
}

void INA226::setAlertLatch(bool latch)
{
 uint16_t temp = getMaskEnable();
 if (latch) temp |= INA226_BIT_LEN;
       else temp &= ~INA226_BIT_LEN;
 setMaskEnable(temp);
}

//**************************************************************
//*     get methods                                            *
//**************************************************************
//_____get number of values for average as enum value___________
ina226_averages_t INA226::getAverages(void)
{
 uint16_t value;
 value = readRegister16(INA226_REG_CONFIG);
 value &= 0b0000111000000000;
 value >>= 9;
 return (ina226_averages_t)value;
}

//_____get bus conversation time as enum value__________________
ina226_busConvTime_t INA226::getBusConversionTime(void)
{
 uint16_t value;
 value = readRegister16(INA226_REG_CONFIG);
 value &= 0b0000000111000000;
 value >>= 6;
 return (ina226_busConvTime_t)value;
}

//_____get shunt conversation time as enum value________________
ina226_shuntConvTime_t INA226::getShuntConversionTime(void)
{
 uint16_t value;
 value = readRegister16(INA226_REG_CONFIG);
 value &= 0b0000000000111000;
 value >>= 3;
 return (ina226_shuntConvTime_t)value;
}

//_____get mode as enum value___________________________________
ina226_mode_t INA226::getMode(void)
{
 uint16_t value;
 value = readRegister16(INA226_REG_CONFIG);
 value &= 0b0000000000000111;
 return (ina226_mode_t)value;
}

//_____get maximal possible (allowed) current___________________
double INA226::getMaxPossibleCurrent(void) 
{ return (vShuntMax / rShunt); }

//_____get maximal configured current___________________________
double INA226::getMaxCurrent(void)
{ 
 double maxCurrent = (currentLSB * 32767);
 double maxPossible = getMaxPossibleCurrent();
 if (maxCurrent > maxPossible) return maxPossible;
 return maxCurrent;
}

//_____get maximal shunt voltage (configured or hardware)_______
double INA226::getMaxShuntVoltage(void)
{
 double maxVoltage = getMaxCurrent() * rShunt;
 if (maxVoltage >= vShuntMax) return vShuntMax;
 return maxVoltage;
}

//_____get maximal power________________________________________
double INA226::getMaxPower(void)
{ return (getMaxCurrent()*vBusMax); }

//_____is overflow______________________________________________
bool INA226::isMathOverflow(void)
{ return ((getMaskEnable()&INA226_BIT_OVF) == INA226_BIT_OVF); }

//_____is alert_________________________________________________
bool INA226::isAlert(void)
{ return ((getMaskEnable()&INA226_BIT_AFF) == INA226_BIT_AFF); }

//**************************************************************
//*     config info as string messages                         *
//**************************************************************

//_____INA226 mode as string____________________________________
String INA226::getsMode()
{
 switch (getMode())
 {
  case INA226_MODE_POWER_DOWN:    return("Power-Down");
  case INA226_MODE_SHUNT_TRIG:    return("Shunt Voltage, Triggered");
  case INA226_MODE_BUS_TRIG:      return("Bus Voltage, Triggered");
  case INA226_MODE_SHUNT_BUS_TRIG:return("Shunt and Bus, Triggered");
  case INA226_MODE_ADC_OFF:       return("ADC Off");
  case INA226_MODE_SHUNT_CONT:    return("Shunt Voltage, Continuous");
  case INA226_MODE_BUS_CONT:      return("Bus Voltage, Continuous");
  case INA226_MODE_SHUNT_BUS_CONT:return("Shunt and Bus, Continuous");
  default: return("unknown");
 }
}

//_____INA226 number of samples average as string_______________
String INA226::getsAverages()
{
 switch (getAverages())
 {
  case INA226_AVERAGES_1:    return("1 sample");
  case INA226_AVERAGES_4:    return("4 samples");
  case INA226_AVERAGES_16:   return("16 samples");
  case INA226_AVERAGES_64:   return("64 samples");
  case INA226_AVERAGES_128:  return("128 samples");
  case INA226_AVERAGES_256:  return("256 samples");
  case INA226_AVERAGES_512:  return("512 samples");
  case INA226_AVERAGES_1024: return("1024 samples");
  default: return("unknown");
 }
}

//_____INA226 bus conversation time_____________________________
String INA226::getsBusConversionTime()
{
 switch(getBusConversionTime())
 {
  case INA226_BUS_CONV_TIME_140US:  return("140uS");
  case INA226_BUS_CONV_TIME_204US:  return("204uS");
  case INA226_BUS_CONV_TIME_332US:  return("332uS");
  case INA226_BUS_CONV_TIME_588US:  return("558uS");
  case INA226_BUS_CONV_TIME_1100US: return("1.100ms");
  case INA226_BUS_CONV_TIME_2116US: return("2.116ms");
  case INA226_BUS_CONV_TIME_4156US: return("4.156ms");
  case INA226_BUS_CONV_TIME_8244US: return("8.244ms");
  default: return("unknown");
 }
}

//_____INA226 shunt conversation time___________________________
String INA226::getsShuntConversionTime()
{
 switch (getShuntConversionTime())
 {
  case INA226_SHUNT_CONV_TIME_140US:  return("140uS");
  case INA226_SHUNT_CONV_TIME_204US:  return("204uS");
  case INA226_SHUNT_CONV_TIME_332US:  return("332uS");
  case INA226_SHUNT_CONV_TIME_588US:  return("558uS");
  case INA226_SHUNT_CONV_TIME_1100US: return("1.100ms");
  case INA226_SHUNT_CONV_TIME_2116US: return("2.116ms");
  case INA226_SHUNT_CONV_TIME_4156US: return("4.156ms");
  case INA226_SHUNT_CONV_TIME_8244US: return("8.244ms");
  default: return("unknown");
 }
}

//**************************************************************
//*     Helper functions                                       *
//**************************************************************

void INA226::setMaskEnable(uint16_t mask)
{ writeRegister16(INA226_REG_MASKENABLE, mask); }

uint16_t INA226::getMaskEnable(void)
{ return readRegister16(INA226_REG_MASKENABLE); }

//**************************************************************
//*     i2c access                                             *
//**************************************************************

//_____read 16bit register (2 bytes)____________________________
// return: value, property status
int16_t INA226::readRegister16(uint8_t reg)
{
 int16_t value;
 Wire.beginTransmission(i2cAddress);
 Wire.write(reg);
 status_=Wire.endTransmission();
 delay(1);
 Wire.beginTransmission(i2cAddress);
 Wire.requestFrom(i2cAddress, 2);
 if(Wire.available()<1) 
 {
  Wire.endTransmission();
  status_=INA226_ERR_NUM_BYTES;
  return 0;
 }
 // while(!Wire.available()) {};
 uint8_t vha = Wire.read();
 uint8_t vla = Wire.read();
 status_=Wire.endTransmission();
 if(status_!=INA226_OK) return 0;
 value = vha << 8 | vla;
 return value;
}

//_____write 16bit register (2 bytes)___________________________
// return: status
int INA226::writeRegister16(uint8_t reg, uint16_t val)
{
 uint8_t vla;
 vla = (uint8_t)val;
 val >>= 8;
 Wire.beginTransmission(i2cAddress);
 Wire.write(reg);
 Wire.write((uint8_t)val);
 Wire.write(vla);
 status_=Wire.endTransmission();
 return status_;
}


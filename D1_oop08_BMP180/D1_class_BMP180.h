//_____D1_class_BMP180.h______________________170415-170416_____
// D1 mini class for temperature and pressure sensor BMP180.
// BMP180: Pressure    300..1100hPa, 0,01hPa, 16..19bit
//         Temperature -40°..+85°C, 0,1°, 16bit
// Default i2c address is 0x77
// See also SparkFun Electronics library SFE_BMP180-master
#ifndef D1_CLASS_BMP180_H
#define D1_CLASS_BMP180_H
//#define MEASURING_DELAY_MS 500
#include <Wire.h>
#include <stdio.h>
#include <math.h>

#define BMP180_ADDR                0x77     // 7-bit address
#define BMP180_REG_CONTROL         0xF4
#define BMP180_REG_RESULT          0xF6
#define BMP180_COMMAND_TEMPERATURE 0x2E
#define BMP180_COMMAND_PRESSURE0   0x34
#define BMP180_COMMAND_PRESSURE1   0x74
#define BMP180_COMMAND_PRESSURE2   0xB4
#define BMP180_COMMAND_PRESSURE3   0xF4

class BMP180 {
 public:
  BMP180();
  BMP180(int i2c_address);
  int getStatus();
  int getValues(double &temperature, double &pressure);
  double sealevel(double pressure, double altitude);
  double altitude(double P, double P0);
  void setPressureMode(int mode);
 private:
  int    i2cAddress;
  double pressure_;
  double temperature_;
  int    pressure_mode;
  int    pressure_command[4];
  int    pressure_delay[4];
  int    status;
  //-----internal parameter-------------------------------------
  int AC1,AC2,AC3,VB1,VB2,MB,MC,MD;
  unsigned int AC4,AC5,AC6; 
  double c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;
 private:  
  void setup();
  void measuring();
  bool readInt(char address, int &value);
  bool readUInt(char address, unsigned int &value);
  bool readBytes(unsigned char *values, char length);
  bool writeBytes(unsigned char *values, char length);
};

//_____constructor (default 0x5C)_______________________________
BMP180::BMP180() { i2cAddress=BMP180_ADDR; setup(); }

//_____constructor with i2c Address_____________________________
BMP180::BMP180(int i2c_address) {
 i2cAddress=i2c_address; 
 setup(); 
}

//_____system status____________________________________________
// 0: success
// 1: data too long to fit in transmit buffer
// 2: received NACK on transmit of address
// 3: received NACK on transmit of data
// 4: other error 
// 5: no bytes received
// 7: wrong chip id
// 8: calibration data error
// 9: 
// 10..  : measuring error
// 128...: setup failed
int BMP180::getStatus() { return status; }

//_____start messure and return sensor values___________________
// return: status
// if status unequal 0 sensor values are not changed
int BMP180::getValues(double &temperature, double &pressure)
{
 measuring();
 if(status!=0) return status;
 temperature=temperature_;
 pressure=pressure_;
 return 0;
}

//_____pressure at sealevel (for weather measurements)__________
// uses pressure_ and sealevel_
double BMP180::sealevel(double pressure, double altitude)
{
 return(pressure/pow(1-(altitude/44330.0),5.255));
}

//_____convert absolute pressure to altitude (in meters)________
// needs a given baseline pressure P0 (sea-level, base camp,...)
// P: absolute pressure (mbar = hPa)
// P0: fixed baseline pressure (mbar = hPa)
// return: signed altitude (meters) above baseline
double BMP180::altitude(double P, double P0)
{
 return(44330.0*(1-pow(P/P0,1/5.255)));
}

//_____set resolution of measurement (0..3), higher=slower______
void BMP180::setPressureMode(int mode)
{
 pressure_mode=mode;
 if((mode<0)||(mode>3)) pressure_mode=0;
}

//**************************************************************
// private functions
//**************************************************************

//_____setup device BMP180______________________________________
// called by constructor or measuring
void BMP180::setup()
{
 double c3,c4,b1;
 //-----set default values--------------------------------------
 pressure_=-999;
 temperature_=-999;
 pressure_mode=0;
 pressure_command[0]=BMP180_COMMAND_PRESSURE0;
 pressure_command[1]=BMP180_COMMAND_PRESSURE1;
 pressure_command[2]=BMP180_COMMAND_PRESSURE2;
 pressure_command[3]=BMP180_COMMAND_PRESSURE3;
 pressure_delay[0]=5;                       // from data sheet
 pressure_delay[1]=8;                       // from data sheet
 pressure_delay[2]=14;                      // from data sheet
 pressure_delay[3]=26;                      // from data sheet
 status=128;
 //-----check: read chip id register (must be 0x55)-------------
 int status_=0;
 Wire.begin();                    //(4,5): SDA on 4, SCL on 5
 Wire.beginTransmission(i2cAddress);
 Wire.write(0xD0);                // select register
 status_=Wire.endTransmission();
 if(status_!=0) { status|=status_; return; }
 Wire.beginTransmission(i2cAddress);
 Wire.requestFrom(i2cAddress, 1);
 if(Wire.available()<1) {
  status|=5; Wire.endTransmission(); return; 
 }
 byte b = Wire.read();
 status=Wire.endTransmission();
 if(status_!=0) { status|=status_; return; }
 if(b!=0x55) { status|=7; return; }
 //-----Retrieve calibration data from device-------------------
 if ( !(
  readInt(0xAA,AC1) &&
  readInt(0xAC,AC2) &&
  readInt(0xAE,AC3) &&
  readUInt(0xB0,AC4) &&
  readUInt(0xB2,AC5) &&
  readUInt(0xB4,AC6) &&
  readInt(0xB6,VB1) &&
  readInt(0xB8,VB2) &&
  readInt(0xBA,MB) &&
  readInt(0xBC,MC) &&
  readInt(0xBE,MD) ))
 {
  //-----error reading calibration data-------------------------
  status|=8; 
  return;
 }
 //-----Compute floating-point polynominals---------------------
 c3 = 160.0 * pow(2,-15) * AC3;
 c4 = pow(10,-3) * pow(2,-15) * AC4;
 b1 = pow(160,2) * pow(2,-30) * VB1;
 c5 = (pow(2,-15) / 160) * AC5;
 c6 = AC6;
 mc = (pow(2,11) / pow(160,2)) * MC;
 md = MD / 160.0;
 x0 = AC1;
 x1 = 160.0 * pow(2,-13) * AC2;
 x2 = pow(160,2) * pow(2,-25) * VB2;
 y0 = c4 * pow(2,15);
 y1 = c4 * c3;
 y2 = c4 * b1;
 p0 = (3791.0 - 8.0) / 1600.0;
 p1 = 1.0 - 7357.0 * pow(2,-20);
 p2 = 3038.0 * 100.0 * pow(2,-36);
 status=0;
}

//_____read temperature and humidity from sensor________________
// pressure value is absolute mbars (hPa). To compensate for 
//   altitude call sealevel()!
void BMP180::measuring()
{
 unsigned char data[3];
 //char result;
 double tu, a, pu, s, x, y, z;
 //-----was setup successful?-----------------------------------
 if(status>0) setup();
 if(status!=0) return;
 //-----start temperature measuring-----------------------------
 data[0] = BMP180_REG_CONTROL;
 data[1] = BMP180_COMMAND_TEMPERATURE;
 if(!writeBytes(data, 2)) { status=10; return; }
 delay(5);                        // 5ms, from data sheet
 //-----read temperature----------------------------------------
 data[0] = BMP180_REG_RESULT;
 if(!readBytes(data, 2)) { status=11; return; }
 tu = (data[0] * 256.0) + data[1];
 a = c5 * (tu - c6);
 this->temperature_ = a + (mc / (a + md));
 //-----start pressure measuring--------------------------------
 if((pressure_mode<0)||(pressure_mode>3)) pressure_mode=0;
 data[0] = BMP180_REG_CONTROL;
 data[1] = pressure_command[pressure_mode];
 if(!writeBytes(data, 2)) { status=12; return; }
 delay(pressure_delay[pressure_mode]);
 //-----read pressure-------------------------------------------
 data[0] = BMP180_REG_RESULT;
 if(!readBytes(data, 3)) { status=13; return; }
 pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);
 s = this->temperature_ - 25.0;
 x = (x2 * pow(s,2)) + (x1 * s) + x0;
 y = (y2 * pow(s,2)) + (y1 * s) + y0;
 z = (pu - x) / y;
 this->pressure_ = (p2 * pow(z,2)) + (p1 * z) + p0;
 status=0;
}

//-----read an integer value (two bytes) from BMB180------------
// address: register to start reading (plus subsequent register)
// value  : external variable to store data (value modified)
// return : true=success, false=error
bool BMP180::readInt(char address, int &value)
{
 unsigned char data[2];
 data[0] = address;
 if (readBytes(data,2))
 {
  value = (((int)data[0]<<8)|(int)data[1]);
if (value & 0x8000) value |= 0xFFFF0000; // sign extend if negative
  return true;
 }
 value = 0;
 return false;
}

//-----read an unsigned integer value (two bytes) from BMB180---
// address: register to start reading (plus subsequent register)
// value  : external variable to store data (value modified)
// return : true=success, false=error
bool BMP180::readUInt(char address, unsigned int &value)
{
 unsigned char data[2];
 data[0] = address;
 if (readBytes(data,2))
 {
  value = (((unsigned int)data[0]<<8)|(unsigned int)data[1]);
  return true;
 }
 value = 0;
 return false;
}

//-----Read an array of bytes from BMP180-----------------------
// values: array to hold data. Put start register in values[0]
// length: number of bytes to read
// return: true=success, false=error
bool BMP180::readBytes(unsigned char *values, char length)
{
 char i;
 Wire.beginTransmission(i2cAddress);
 Wire.write(values[0]);
 status=Wire.endTransmission();
 if(status!=0) return false;
 Wire.requestFrom(i2cAddress, length);
 delay(1);
 if(Wire.available()<1) {
  Wire.endTransmission(); 
  status=5;
  return false; 
 }
 for(i=0;i<length;i++)
 {
  values[i] = Wire.read();
 }
 return true;
}

//-----Write an array of bytes to BMP180------------------------
// values: array to hold data. Put start register in values[0]
// length: number of bytes to read
// return: true=success, false=error
bool BMP180::writeBytes(unsigned char *values, char length)
{
 char i;
 Wire.beginTransmission(i2cAddress);
 Wire.write(values,length);
 status = Wire.endTransmission();
 if(status!=0) return false;
 return true;
}

#endif

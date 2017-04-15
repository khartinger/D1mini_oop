//_____D1_class_BH1750.h______________________170414-170414_____
// D1 mini class for digital light sensor BH1750.
// Default i2c address is 0x23 (other option 0x5C).
#ifndef D1_CLASS_BH1750_H
#define D1_CLASS_BH1750_H
#define MEASURING_DELAY_MS 500
#include <Wire.h>

class BH1750 {
 private:
  int  i2cAddress;
  int  status;
  long brightness;
 private:  
  void setup();
  void measuring();
 public:
  BH1750();
  BH1750(int i2c_address);
  int    getStatus();
  double getBrightness();
  long   getBi();

};

//_____constructor (default 0x5C)_______________________________
BH1750::BH1750() { i2cAddress=0x23; setup(); }

//_____constructor with i2c Address_____________________________
BH1750::BH1750(int i2c_address) {
 i2cAddress=i2c_address; 
 setup(); 
}

//_____setup device BH1750______________________________________
void BH1750::setup()
{
 //-----wake up sensor------------------------------------------
 Wire.begin();                    //(4,5): SDA on 4, SCL on 5
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x01);                // power on, waiting for cmd
 status=Wire.endTransmission();
 if(status!=0) return;
 //-----Reset Data register-------------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x07);                // reset data register
 status=Wire.endTransmission();   
 if(status!=0) return;
 measuring();
}

//_____system status____________________________________________
// 0: success
// 1: data too long to fit in transmit buffer
// 2: received NACK on transmit of address
// 3: received NACK on transmit of data
// 4: other error 
// 9: first status after setup
int BH1750::getStatus() { return status; }

//_____get temperature as double________________________________
double BH1750::getBrightness() { return(getBi()); }

//_____get temperature as integer_______________________________
long BH1750::getBi() { 
 measuring();
 return brightness; 
}

//_____read temperature and humidity from sensor________________
void BH1750::measuring()
{
 //-----set Continuously H-Resolution Mode, Resolution 1 lx-----
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x10);                // Continuously H-Resol. Mode
 status=Wire.endTransmission();
 if(status!=0)                    // try a second time
 {
  delay(120);                     //Measurement Time(data sheet)
  Wire.beginTransmission(i2cAddress);
  Wire.write(0x10);               // Continuously H-Resol. Mode
  status=Wire.endTransmission();
  if(status!=0) return;
 }
 //-----read 8 bytes from sensor (6xdata, 2xcrc)----------------
 Wire.requestFrom(i2cAddress, 2);
 if(Wire.available())             // (read) data bytes ready?
 {
  int temp_=Wire.read()<<8;
  temp_|=Wire.read();
  brightness=temp_;
 }
 status=Wire.endTransmission(); 
}
#endif

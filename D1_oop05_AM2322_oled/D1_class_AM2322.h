//_____D1_class_AM2322.h______________________170413-170413_____
// D1 mini class for temperature and humidity sensor AM2322.
// AM2322: Temperature -40°..+80°C, +-0,5°C/+-0,2°C, 16bit
//         Humidity    0%..99,0%, +-3%/+-0,1%, 16bit
// Default i2c address is 0x5C
#ifndef D1_CLASS_AM2322_H
#define D1_CLASS_AM2322_H
#define AM2322_ADDR 0x5C          // 7-bit address
#define MEASURING_DELAY_MS 500
#include <Wire.h>

class AM2322 {
 private:
  int i2cAddress;
  int hum_;
  int tmp_;
  int status;
  unsigned long next_measuring_;
 private:  
  void setup();
  void measuring();
  unsigned short crc16(unsigned char *ptr, unsigned char len);
 public:
  AM2322();
  AM2322(int i2c_address);
  int getStatus();
  int getValues(double &temperature, double &humidity);
  int getValues10(int &temperature, int &humidity);
};

//_____constructor (default 0x5C)_______________________________
AM2322::AM2322() { i2cAddress=AM2322_ADDR; setup(); }

//_____constructor with i2c Address_____________________________
AM2322::AM2322(int i2c_address) {
 i2cAddress=i2c_address; 
 setup(); 
}

//_____setup device AM2322______________________________________
void AM2322::setup()
{
 Wire.begin();                    //(4,5): SDA on 4, SCL on 5
 hum_=-999;
 tmp_=-999;
 status=9;
 next_measuring_=0;
}

//_____system status____________________________________________
// 0: success
// 1: data too long to fit in transmit buffer
// 2: received NACK on transmit of address
// 3: received NACK on transmit of data
// 4: other error 
// 8: crc error
// 9: first status after setup
int AM2322::getStatus() { return status; }

//_____get temperature and humidity as float____________________
int AM2322::getValues(double &temperature, double &humidity)
{
 this->measuring();
 if(status!=0) return status;
 temperature=0.1*tmp_;
 humidity=0.1*hum_;
 return status;
}
//_____get temperature and humidity as integer__________________
int AM2322::getValues10(int &temperature, int &humidity)
{
 this->measuring();
 if(status!=0) return status;
 temperature=tmp_;
 humidity=hum_;
 return status;
}
//_____read temperature and humidity from sensor________________
void AM2322::measuring()
{
 uint8_t buf_[6], neg_=0;
 unsigned short crc=0, crc_in=0;
 //-----make a delay between two measuringments-----------------
 if(millis()<next_measuring_) return;
 next_measuring_=millis()+MEASURING_DELAY_MS;
 //-----wake up sensor------------------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.endTransmission();
 delay(1);
 Wire.beginTransmission(i2cAddress);
 status=Wire.endTransmission();
 if(status!=0) return; //{ tmp_=-999; hum_=-999; return; }
 //-----set function 03-----------------------------------------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x03);
 Wire.write(0);
 Wire.write(4);
 Wire.endTransmission();
 delay(2);
 //-----read 8 bytes from sensor (6xdata, 2xcrc)----------------
 Wire.requestFrom(i2cAddress, 8);
 if(Wire.available()<1) { Wire.endTransmission(); return; }
 for(int i=0; i<6; i++)
  buf_[i]=Wire.read();
 crc_in=Wire.read();
 crc_in|=(Wire.read() << 8);
 Wire.endTransmission(); 
 //-----check crc-----------------------------------------------
 crc=crc16(buf_, 6);
 //Serial.print("crc_in="); Serial.print(crc_in);
 //Serial.print(", crc="); Serial.println(crc);
 if(crc!=crc_in) { status=8; return; }
 //-----calcuate temperature and humidity-----------------------
 hum_ =buf_[2]<<8;
 hum_ +=buf_[3];
 if((buf_[4]&0x80)>0) neg_=1;
 buf_[4]=buf_[4]&0x7F; 
 tmp_ =buf_[4]<<8;
 tmp_ +=buf_[5];
 if(neg_) tmp_=-tmp_;
 status=0;
}

//_____CRC code calculation (from data sheet ;)_________________
unsigned short AM2322::crc16(unsigned char *ptr, unsigned char len)
{
 unsigned short crc =0xFFFF;
 unsigned char i;
 while(len--)
 {
  crc ^=*ptr++;
  for(i=0;i<8;i++)
  {
   if(crc & 0x01)
   {
    crc>>=1;
    crc^=0xA001;
   }else
   {
    crc>>=1;
   }
  }
 }
 return crc;
}

#endif

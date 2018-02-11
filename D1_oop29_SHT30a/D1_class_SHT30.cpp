//_____D1_class_SHT30.cpp_____________________180129-190210_____
// D1 mini class for temperature and humidity sensor SHT30.
// SHT30: Temperature -40°C...125°C +-0,7% (0°..+65°C, +-0,3°C)
//        Humidity    +-3%RH
// Default i2c address is 0x45 (other 0x44)
#include "D1_class_SHT30.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor (default 0x5C)_______________________________
SHT30::SHT30() { i2cAddress=SHT30_ADDR; setup(); }

//_____constructor with i2c Address_____________________________
SHT30::SHT30(int i2c_address) {
 i2cAddress=i2c_address; 
 setup();
}

//_____setup device SHT30______________________________________
void SHT30::setup()
{
 Wire.begin();                    //(4,5): SDA on 4, SCL on 5
 hum_=0xFFFF;
 tmp_=0xFFFF;
 status=SHT30_ERR_NO_MEAS;
 next_measuring_=millis();
}

//_____start (the first) measuring______________________________
void SHT30::begin() {
 softReset();
 measuring(); 
}

//_____reset/re-initialization of sensor (see datasheet 4.9)____
void SHT30::softReset() {
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x30);
 Wire.write(0xA2);
 status=Wire.endTransmission();
 delay(100);
}

//**************************************************************
//    setter and getter methods
//**************************************************************

//_____system status as int_____________________________________
int SHT30::getStatus() { return status; }

//_____system status as german text string______________________
String SHT30::getsStatus() 
{ 
 switch(status)
 {
  case SHT30_OK:
   return String("OK");
  case SHT30_ERR_TOO_LONG:
   return String("I2C Sendepuffer ist zu klein für die Daten");
  case SHT30_ERR_NACK_ADDR:
   return String("Keine Bestaetigung (ACK) nach Adressbyte");
  case SHT30_ERR_NACK_DATA:
   return String("Keine Bestaetigung (ACK) nach Datenbyte");
  case SHT30_ERR_OTHER:
   return String("I2C Fehler");
  case SHT30_ERR_NUM_BYTES:
   return String("Falsche Anzahl Bytes");
  case SHT30_ERR_NO_BYTE:
   return String("Kein Byte empfangen");
  case SHT30_ERR_NUM_CHARS:
   return String("Falsche Anzahl an Bytes");
  case SHT30_ERR_RANGE:
   return String("Wert ausserhalb des zulaessigen Bereichs");
  case SHT30_ERR_CRC:
   return String("Pruefsummenfehler");
  case SHT30_ERR_NO_MEAS:
   return String("Es wurde noch keine Messung durchgefuehrt");
  default:
   return String("Unbekannter Fehler Nummer ")+String(status);
 }
 return String("Schwerwiegender, unbekannter Fehler");
}

//_____measure temperature and return it as float_______________
// return: temperature or SHT30_ERR_VALUE (on error)
float SHT30::getTemperature()
{ 
 measuring();
 if(status!=SHT30_OK) return SHT30_ERR_VALUE;
 return SHT30_FT(tmp_);
}

//_____measure temperature and return it as String______________
// return: temperature or "" (on error)
String SHT30::getsTemperature(int len, int decimals)
{
 measuring();
 return getsLastTemperature(len, decimals);
}

//_____measure humidity and return it as float__________________
// return: temperature or SHT30_ERR_VALUE (on error)
float SHT30::getHumidity()
{ 
 measuring();
 if(status!=SHT30_OK) return SHT30_ERR_VALUE;
 return SHT30_FH(hum_);
}

//_____measure humidity and return it as String_________________
String SHT30::getsHumidity(int len, int decimals)
{
 this->measuring();
 return getsLastHumidity(len, decimals);
}

//_____get temperature and humidity as float____________________
int SHT30::getValues(float &temperature, float &humidity)
{
 this->measuring();
 return getLastValues(temperature, humidity);
}

//**************************************************************
//    get (last) values without making a measurement
//**************************************************************

//_____get last temperature as string (no measurement)__________
String SHT30::getsLastTemperature(int len, int decimals)
{
 String s1="";
 if(status!=SHT30_OK) return s1;
 float temperature=SHT30_FT(tmp_);
 s1=this->float2String(temperature, len, decimals);
 return s1;
}

//_____get last humidity as string (no measurement)_____________
String SHT30::getsLastHumidity(int len, int decimals)
{
 String s1="";
 if(status!=0) return s1;
 float humidity=SHT30_FH(hum_);
 s1=this->float2String(humidity, len, decimals);
 return s1;
}

//_____get temperature and humidity as float____________________
int SHT30::getLastValues(float &temperature, float &humidity)
{
 if(status!=SHT30_OK)
 {
  temperature=SHT30_ERR_VALUE;
  humidity=SHT30_ERR_VALUE;
 }
 else
 {
  temperature=SHT30_FT(tmp_);
  humidity=SHT30_FH(hum_);
 }
 return status;
}

//**************************************************************
//    helper functions
//**************************************************************

//_____read temperature and humidity from sensor________________
void SHT30::measuring()
{
 unsigned char buf_[6], neg_=0;
 unsigned short crc=0, crc_in=0;
 //-----make a delay between two measuringments-----------------
 if(millis()<=next_measuring_) return;
 next_measuring_=millis()+MEASURING_DELAY_MS;
 //-----start i2c transmission, send measurement command--------
 Wire.beginTransmission(i2cAddress);
 Wire.write(0x2C);                // clock stretching enabled
 Wire.write(0x06);                // high repeatability measurem
 status=Wire.endTransmission();
 if(status!=0) return;
 delay(20);
 //-----read 6 bytes from sensor: 2x tmp, 1x crc, 2x hum, 1x crc
 Wire.requestFrom(i2cAddress, 6);
 if(Wire.available()<1) 
 {
  Wire.endTransmission();
  status=SHT30_ERR_NUM_BYTES;
  return;
 }
 for (int i=0; i<6; i++) { buf_[i]=Wire.read(); };
 delay(20);
 if(Wire.available()!=0) { status=SHT30_ERR_NUM_BYTES; return; }
 //-----check data----------------------------------------------
 crc=calcCRC(buf_, 2);
 if(buf_[2]!=crc)  { status=SHT30_ERR_CRC; return; }
 crc=calcCRC(&buf_[3], 2);
 if(buf_[5]!=crc)  { status=SHT30_ERR_CRC; return; }
 //-----accept data---------------------------------------------
 tmp_= buf_[0]<<8 | buf_[1];
 hum_= buf_[3]<<8 | buf_[4];
 status=SHT30_OK;
 return;
}

//_____Build crc (see datasheet 4.12)___________________________
// Polynomial: 0x31 (x8 + x5 + x4 + 1)
// Initialization: 0xFF, Final XOR: 0x00
unsigned char SHT30::calcCRC(unsigned char *idata, int len)
{
 const unsigned char POLYNOMIAL(0x31);
 unsigned char CRC(0xFF);
 for ( int j = len; j; --j )
 {
  CRC ^= *idata++;
  for ( int i = 8; i; --i )
  {
   CRC = ( CRC & 0x80 )
   ? (CRC << 1) ^ POLYNOMIAL
   : (CRC << 1);
  }
 }
 return CRC;
}

//_____convert float to String with given decimals_______________
String SHT30::float2String(float f, int len, int decimals)
{
 char carray[24];                           // convert helper
 String s1="";                              // help string
 if((len<1)||(len>24)||(decimals<0)) return s1;
 dtostrf(f,len,decimals,carray);            // format float l.d
 s1=String(carray);                         // array to string
 if(len<=decimals) s1.trim();               // remove blanks
 return s1;
}

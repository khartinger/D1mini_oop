//_____D1_class_Anemometer1.h_________________170430-170430_____
// D1 mini class for an anemometer that outputs impulses.
// A PCF8583 counts these impulses and calculates wind speed
//  (in km/h or m/s) when wind speed or wind average is read.
// Default time between 2 measurements is 2s
// Default i2c address for PCF8583 is 0x50 (other option 0x51).
#ifndef D1_CLASS_ANEMOMETER1_H
#define D1_CLASS_ANEMOMETER1_H
#define MEASURING_DELAY_MS 2000
#define AVINDEX_MAX        5
extern "C" {
#include "user_interface.h"
}
//#include <Wire.h>
#include "D1_class_PCF8583.h"

class Anemometer1 {
 private:
  PCF8583 pcf8583;
  int     status;
  unsigned long measuring_delay_ms;
  unsigned long next_measuring_;
  double  speed;
  double  speedaverage;
  double  ufactor;
  double  avValue[AVINDEX_MAX];
  int     avIndex;
 private:
  void    setup();
  void    measuring();
 public:
  Anemometer1();
  Anemometer1(int i2c_address);
  int     setMeasuringDelay(unsigned long ms_);
  int     setUnit(int unit_);
  int     getStatus();
  double  getSpeed();
  double  getSpeedAverage();
};

//_____constructor (default 0x5C)_______________________________
Anemometer1::Anemometer1(){ setup(); }

//_____constructor with i2c Address_____________________________
Anemometer1::Anemometer1(int i2c_address):pcf8583(i2c_address)
{ setup(); }

//_____setup device Anemometer1_________________________________
void Anemometer1::setup()
{
 speed=0;                              // start value
 measuring_delay_ms=MEASURING_DELAY_MS;
 setUnit(0);                           // set ufactor
 //-----setup average calculation (ring memory)-----------------
 speedaverage=0;                       // start value
 avIndex=0;                            // value to replace
 for(int i=0; i<AVINDEX_MAX; i++) avValue[i]=0; // all values=0
 //-----setup impulse counter pcf8583---------------------------
 pcf8583.setMode(MODE_COUNTER);        // PCF8583 as counter
 status=pcf8583.setCounter(0);         // reset counter value
 next_measuring_=millis();             // measuring allowed
}

//_____change measuring time____________________________________
int Anemometer1::setMeasuringDelay(unsigned long ms_) {
 measuring_delay_ms=ms_;
}

//_____set output unit__________________________________________
// unit_ 0=km/h (default), 1=m/s
int Anemometer1::setUnit(int unit_)
{
 switch(unit_)
 {
  case 1: ufactor=2000.0/3.0;
          break;
  default: ufactor=2400.0;
 }
}

//_____system status____________________________________________
// 0: success
// 1: data too long to fit in transmit buffer
// 2: received NACK on transmit of address
// 3: received NACK on transmit of data
// 4: other error 
// 8: crc error
// 9: first status after setup
int Anemometer1::getStatus() { return status; }

//_____get wind speed___________________________________________
double Anemometer1::getSpeed() { measuring(); return speed; }

//_____get average of wind speed________________________________
double Anemometer1::getSpeedAverage() { 
 measuring(); return speedaverage; }

//_____read temperature and humidity from sensor________________
void Anemometer1::measuring()
{
 long counter;
 long T_now, T_ms; 
 //-----check delay between two measuringments------------------
 T_now=millis();
 if(T_now<next_measuring_) return;
 //-----get counter value and reset it--------------------------
 counter=pcf8583.getCounter();
 status=pcf8583.getStatus();
 if(status!=0) return;
 status=pcf8583.setCounter(0);
 //-----measuring ok, set next measuring time-------------------
 T_ms=T_now-next_measuring_+measuring_delay_ms;
 next_measuring_=T_now+measuring_delay_ms;  // next measuring
 //-----calculate speed and average-----------------------------
 speed=ufactor/T_ms * counter;
 avValue[avIndex++]=speed;                  // replace oldest
 if(avIndex>=AVINDEX_MAX) avIndex=0;        // check index
 double sum_=0;
 for(int i=0; i<AVINDEX_MAX; i++) sum_=sum_+avValue[i];
 speedaverage = sum_ / AVINDEX_MAX;
}

#endif

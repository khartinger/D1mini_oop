//_____D1_class_CurrentAC.cpp_________________201203-201206_____
// The class CurrentAC is a class for D1 mini or ESP32 D1mini
// for measuring AC current using a selfmade shield D1_INA122_V3
// (with precision instrumentation amplifier INA122P) and a 
// current sensor ASM010.
// The AC-current generates a INA122P-output voltage that is
// measured by Analog In. This value (0..1023|4095) is converted
// to an ampere value using arrays with support points (because
// the function iac=f(uin) usually is not linear).
// Hardware:
// (1) WeMos D1 mini or ESP32 D1mini
// (2) Selfmade shield D1_INA122_V3 181108 (Analog In)
// (3) Current transformer (ASM-010)
//
// Created by Karl Hartinger, December 06, 2020.
// Updates:
// 2020-12-06 add nominal (rated) current (y4nom..)
// Released into the public domain.

#include "D1_class_CurrentAC.h"

// *************************************************************
//    constructor & co
// *************************************************************

//_____constructor (default board D1 mini, A0, default ion)_____
CurrentAC::CurrentAC() {
 int   boardType=CURRENTAC_BOARD_DEFAULT; // 1=D1 mini
 int   pin=A0;                         // analog in @ D1 mini
 float ion_A=CURRENTAC_A_ON;           // 10W @ 230V
 float inom_A=y4nomD1mini_;            // nominal 2A
 setup(boardType, inom_A, ion_A, pin);
}

//_____constructor (default board D1 mini, A0)__________________
CurrentAC::CurrentAC(float inom_A, float ion_A) {
 int   boardType=CURRENTAC_BOARD_DEFAULT; // 1=D1 mini
 int   pin=A0;                         // analog in @ D1 mini
 setup(boardType, inom_A, ion_A, pin);
}

//_____constructor (default analog in pin, on current)__________
CurrentAC::CurrentAC(int boardType) {
 int   pin=A0;                         // analog in @ D1 mini
 float inom_A=y4nomD1mini_;            // nominal 2A
 if(boardType==2) {
  pin=CURRENTAC_PIN_AIN_ESP;           // 2=ESP32 D1 mini
  inom_A=y4nomESP32D1_;                // nominal 2A
 }
 float ion_A=CURRENTAC_A_ON;           // 10W @ 230V
 setup(boardType, inom_A, ion_A, pin);
}

//_____constructor (default analog in pin)______________________
CurrentAC::CurrentAC(int boardType, float inom_A, float ion_A) {
 int   pin=A0;                         // analog in @ D1 mini
 if(boardType==2) pin=CURRENTAC_PIN_AIN_ESP; // 2=ESP32 D1 mini
 setup(boardType, inom_A, ion_A, pin);
}

//_____constructor (no default values)__________________________
CurrentAC::CurrentAC(int boardType, float inom_A, float ion_A, int pin) {
 setup(boardType, inom_A, ion_A, pin);
}

//_____set properties to default values_________________________
void CurrentAC::setup(int boardType, float inom_A, float ion_A, int pin)
{
 a_=0;                                 // start: 0 Ampere
 aOn_=ion_A;                           // limit current "on"
 ainLast_=0;                           // start ain=0 (ADC)
 bLastOn_=false;                       // current off 
 num50HzPeriods_=CURRENTAC_50HZ_PERIODS;//# measure 50Hz periods
 //------hardware specific values-------------------------------
 if(boardType==2)
 {//.....2=ESP32 D1 mini........................................
  iBoard_=boardType;                   // 2=ESP32D1
  ainPin_=pin;                         // several pins allowed 
  ainMax_=CURRENTAC_ADC_MAX12;         // 12bit ADC
  x4nomDefault_=x4nomESP32D1_;         // nominal x-value
  y4nomDefault_=y4nomESP32D1_;         // nominal y-value (2A)
 }
 else
 {//.....default board: Wemos D1 mini...........................
  iBoard_=CURRENTAC_BOARD_DEFAULT;     // 1=D1mini
  ainPin_=CURRENTAC_PIN_AIN_D1;        // A0=17 default D1mini
  ainMax_=CURRENTAC_ADC_MAX10;         // 10bit ADC
  x4nomDefault_=x4nomD1mini_;          // nominal x-value 
  y4nomDefault_=y4nomD1mini_;          // nominal y-value (2A)
 }
 //------support points (limits)--------------------------------
 x4nom_=x4nomDefault_;
 if(inom_A>0) y4nom_=inom_A;
         else y4nom_=y4nomDefault_;
 //...set properties numPoints_, xPoints_, yPoints_.............
 setRefPoints();                       // use default values
}

// *************************************************************
//    get measurement values
// *************************************************************

//_____measure and calculate current in A_______________________
// change: ainLast_
// return: current in Ampere
float CurrentAC::getCurrent()
{
 ainLast_=measure();
 return getCurrent(ainLast_);
}

//_____dac value of last measured current_______________________
// das not force a measurement!
int CurrentAC::getLastAin() { return ainLast_; }

//_____measure current and return true, if current is on________
// return true: current is on (>aOn_)
bool CurrentAC::isOn() {
 ainLast_=measure();
 return(getCurrent(ainLast_)>aOn_);
}

//_____measure current and return true, if current is off_______
// return true: current is off (<=aOn_)
bool CurrentAC::isOff() { return(!isOn()); }

//_____return true, if current on|off changed since last call___
bool CurrentAC::isChange() {
 bool bNow=isOn();
 if(bLastOn_!=bNow)
 {
  bLastOn_=bNow;
  return true;
 }
 return false;
}

//_____return current limit for "on"____________________________
float CurrentAC::getCurrentOn() { return aOn_; }

// return nominal current_______________________________________
float CurrentAC::getNominalCurrent() { return y4nom_; }

// *************************************************************
//    set parameter
// *************************************************************

//_____set current limit for "on"_______________________________
// e.g. for P=10W: I=P/U=10/230=0,0435A
bool CurrentAC::setCurrentOn(float onAmpere)
{
 if(onAmpere<0) return false;
 aOn_=onAmpere;
 return true;
}

//_____set nominal (rated) current______________________________
// Note: The nominal point is usually not the end point of the 
// compensation curve, as this is not linear - especially for 
// high values.
bool CurrentAC::setNominalCurrent(float inom_A)
{
 if(inom_A<=0) return false;
 y4nom_=inom_A;
 return true;
}

//_____set ADC value for maximal current________________________
bool CurrentAC::setx4nom(int x4nom)
{
 if(x4nom<=0) return false;
 x4nom_=x4nom;
 return true;
}

//_____set number of 50Hz periods to measure____________________
// duration of measurement: periods*20ms
bool CurrentAC::setNumberOf50HzPeriods(int periods)
{
  if(periods<1 ||periods>1000) return false;
  num50HzPeriods_=periods;
  return true;
}

//_____set new board type (1=D1mini, 2=ESP32D1)_________________
// Note: All properties are reset!
bool CurrentAC::setBoardType(int boardType)
{
 int pin=A0;                           // analog in @ D1 mini
 int inom_A=y4nomD1mini_;              // nominal current D1 mini
 if(boardType<1 || boardType>2) return false;
 if(boardType==2) {
  pin=CURRENTAC_PIN_AIN_ESP; // 2=ESP32 D1 mini
  inom_A=y4nomESP32D1_;
 }
 float ion_A=CURRENTAC_A_ON;           // 10W @ 230V
 setup(boardType, inom_A, ion_A, pin);
 return true;
}

//_____set analog in pin________________________________________
// D1 mini: only A0 is available!
// ESP32 mini: more pins available
bool CurrentAC::setPinAin(int pin)
{
 if(iBoard_==CURRENTAC_BOARD_DEFAULT && pin==A0) return true;
 if(iBoard_==CURRENTAC_BOARD_ESP32D1) {
  ainPin_=pin;
  return true;
 }
 return false;
}

// *************************************************************
//    support points to calculate current
// *************************************************************

//_____set reference points for given board_____________________
// change: numPoints_, xPoints_, yPoints_
// return: true=OK, false=error
bool CurrentAC::setRefPoints()
{
 if(iBoard_==CURRENTAC_BOARD_ESP32D1)
 {
  numPoints_=numPointsESP32D1_;
  for(int i=0; i<numPoints_; i++)
  {
   xPoints_[i]=xPointsESP32D1_[i];
   yPoints_[i]=yPointsESP32D1_[i];
  }
  return true;
 }
 numPoints_=numPointsD1mini_;
 for(int i=0; i<numPoints_; i++)
 {
  xPoints_[i]=xPointsD1mini_[i];
  yPoints_[i]=yPointsD1mini_[i];
 }
 return true;
}

//_____set suppert points (minimum 2, max CURRENTAC_POINTS_MAX)_
// change: numPoints_, xPoints_, yPoints_
// return: true=OK, false=error
bool CurrentAC::setRefPoints(int num, int xValues[], float yValues[])
{
 //------check number of support points-------------------------
 if(num<2 || num>CURRENTAC_POINTS_MAX) return false;
 //------check x-limits-----------------------------------------
 int xOld=xValues[0];
 if(xOld<0 || xOld>ainMax_) return false;
 //------check for increasing x-values--------------------------
 for(int i=1; i<num; i++)
 {
  if(xValues[i]<=xOld) return false;
  xOld=xValues[i];
 }
 //-----values ok, accept them----------------------------------
 numPoints_=num;
 for(int i=0; i<numPoints_; i++)
 {
  xPoints_[i]=xValues[i];
  yPoints_[i]=yValues[i];
 }
 return true;
}

//_____return reference points as String________________________
// return: reference points as String
String CurrentAC::getsRefPoints()
{
 char   s0[18];
 String s1;
 int    decimals=3;
 for(int i=0; i<numPoints_; i++)
 {
  sprintf(s0,"(%04d/",xPoints_[i]);
  s1+=String(s0);
  s1+=String(dtostrf(yPoints_[i],decimals+2,decimals,s0));
  s1+=") ";
 }
 return s1;
}

// *************************************************************
//    some more methods
// *************************************************************

//_____calculate current in Ampere for given ADC-value__________
// called by getCurrent()
// changes: ainLast_
// return : current (float)
float CurrentAC::getCurrent(int ain_code)
{
 int     i=0;                          // counter
 int     i1;                           // start index
 float   f1;
 double  x1, y1, x2, y2;
 double  ain_k, ain_d;
 if(ain_code<0) { return 0; }
 //if(ain_code>ainMax_) { return yPoints_[numPoints_-1];}
 long ain1=ain_code;
 if(x4nom_!=0) ain1=ain1*x4nomDefault_/x4nom_;
 ain_code=ain1;
 int imax=numPoints_-1;
 for(i=0; i<imax; i++)
 {
  if(ain_code < xPoints_[i])
  {
   if(i==0) i1=i;
   else i1=i-1;
   break;
  }
 }
 if(i>=imax) i1=numPoints_-2;
 x1=xPoints_[i1];
 y1=yPoints_[i1];
 x2=xPoints_[i1+1];
 y2=yPoints_[i1+1];
 if(x2==x1) 
 {
   f1=y1;
   ainLast_=x1;
 }
 else
 {
  ain_k=(y2-y1)/(x2-x1);
  ain_d=(x2*y1-x1*y2)/(x2-x1);
  f1=ain_d + ain_k*ain_code;
  ainLast_=ain_code;
 }
 if(y4nomDefault_!=0) f1=f1*y4nom_/y4nomDefault_;
 return f1;
}

//_____measure current (result: ADC value)______________________
// uses   : num50HzPeriods_
// changes: -
// return : voltage average of 2 periods (40ms @ 50Hz)
int CurrentAC::measure()
{
 long totalsum=0;                      // total sum
 int  ain1;                            // help: one input value
 //------measuring: "warm up"-----------------------------------
 ain1=analogRead(ainPin_);
 delay(1);
 //------measuring some periods---------------------------------
 for(int p=0; p<num50HzPeriods_; p++)
 {//-----measure one period-------------------------------------
  for(int p1=0; p1<20; p1++)
  {
   ain1=analogRead(ainPin_);
   totalsum=totalsum+ain1;
   delay(1);
  }
 }
 //------calc total average value-------------------------------
 int totalavg=totalsum/(num50HzPeriods_*20);
 return(totalavg);
}

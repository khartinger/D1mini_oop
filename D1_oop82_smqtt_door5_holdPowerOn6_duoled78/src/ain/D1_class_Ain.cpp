//_____D1_class_Ain.cpp_______________________171129-201103_____
// The class Ain reads an analog value and converts it to
// a voltage using an array of support points to calculate the
// value [if the function v=f(ainValue) is not linear].
// Default are two points (numPoints=2) for a stright line
// from 0/0.00 to 1023/3.20.
// Maximum of support points: 16
// Two board types are supported:
//   1=D1mini: pin A0, 10 bit = AIN 0..1023
//   2=ESP32 D1mini: pin 36 (= pin A0), 12 bit = AIN 0..4095
// Created by Karl Hartinger, November 29, 2017.
// Updates:
// 2020-11-03 New design (support points, ESP32, ...)
// 2021-01-03 add getAinValueMax()
// Released into the public domain.
#include "D1_class_Ain.h"

// *************************************************************
//     constructor & co
// *************************************************************

//_____constructor 1____________________________________________
Ain::Ain() {setup(1,AIN_PIN_D1MINI);}  //(1,A0) = D1 mini

//_____constructor 2____________________________________________
Ain::Ain(int boardType) { 
 if(boardType==2) setup(2,AIN_PIN_ESP32D1); //ESP32D1 pin A0
 else setup(1,AIN_PIN_D1MINI);              //(1,A0) = D1 mini
}

//_____constructor 3____________________________________________
Ain::Ain(int boardType, int pin) {
 if(boardType==2) setup(2,pin);        //(2,pin) = ESP32D1 pin ?
  else setup(1,AIN_PIN_D1MINI);        //(1,A0) = D1 mini
}

//_____setup properties_________________________________________
void Ain::setup(int boardType, int pin) {
 //------hardware specific values-------------------------------
 iBoard=boardType;                     // 1=D1mini, 2=ESP32D1
 pinAin=pin;                           // D1mini A0=17, ESP32D1 36
 ainMax=AIN_MAX10;                     // 1=D1mini  = 10 bit ADC
 if(iBoard==2) ainMax=AIN_MAX12;       // 2=ESP32D1 = 12 bit ADC
 //------support points (limits)--------------------------------
 setRefPoints();                       // use default values
 //------additional properties----------------------------------
 ainValue=0;                           // read value
 decimals=2;                           // number decimals
}

//_____check input and calculate straight line parameter________
// for two given points P1(x1/y1) and P2(x2/y2)
// changes ain_k, ain_d
// return: status
int Ain::calcKD(int x1, float y1, int x2, float y2)
{
 if((x1<0) || (x1>ainMax)) return(status=AIN_ERR_X_RANGE);
 if((x2<0) || (x2>ainMax)) return(status=AIN_ERR_X_RANGE);
 if((y1<0) || (y2<0))      return(status=AIN_ERR_Y_RANGE);
 if(x2==x1) return(status=AIN_ERR_X_EQUAL);
 //if(y2==y1) return(status=AIN_ERR_Y_EQUAL);
 ain_k=((double)y2-y1)/(x2-x1);
 ain_d=((double)x2*y1-x1*y2)/(x2-x1);
 return (status=AIN_OK);  
}

// *************************************************************
//     get (new) measurement values
// *************************************************************

//_____return status____________________________________________
int Ain::getStatus() { return status; }

//_____read analog in and return value (old method)_____________
int Ain::getAin1023() { return getAinValue(); }

//_____read analog in, set ainValue and return value____________
// called by getVoltage()
// return: read analog value 0...ainMax (= 1023 | 4095 ...)
int Ain::getAinValue(){ return (ainValue=analogRead(pinAin)); }

//_____return analog in maximum value (=1023|4095|...)__________
int Ain::getAinValueMax() { return ainMax; }

//_____read analog in, calculate voltage and return it as float_
float Ain::getVoltage() {
 int ain=getAinValue();
 return getVoltage(ain);
}

//_____read analog in, calculate voltage, return String_________
String Ain::getsVoltage() {
 return(String(getVoltage(), decimals));
}

//_____read analog in, calculate voltage, return String_________
String Ain::getsVoltage(int decimal_places) {
 return(String(getVoltage(), decimal_places));
}

// *************************************************************
//     get last measurement values
// *************************************************************

//_____return last analog in value______________________________
int Ain::getLastAin1023() { return ainValue; }

//_____return last analog in value______________________________
int Ain::getLastAinValue() { return ainValue; }

//_____calculate last voltage and return it as float____________
float Ain::getLastVoltage() { return(ain_d + ain_k*ainValue); }

//_____read analog in, calculate voltage, return String_________
String Ain::getsLastVoltage() {
 return(String(getLastVoltage(), decimals));
}

//_____read analog in, calculate voltage, return String_________
String Ain::getsLastVoltage(int decimal_places) {
 return(String(getLastVoltage(), decimal_places));
}
// *************************************************************
//     support points
// *************************************************************

//_____set (only) two suppert points (default values)___________
// change: status, numPoints, xPoints, yPoints, ain_k, ain_d
// return: status (0=OK)
int Ain::setRefPoints()
{
 numPoints=2;                          // number of points
 xPoints[0]=0;                         // reference point 1 x
 yPoints[0]=0.0;                       // reference point 1 y
 xPoints[1]=ainMax;                    // reference point 2 x
 yPoints[1]=AIN_MAX_VOLTAGE;           // reference point 2 y
 calcKD(xPoints[0], yPoints[0], xPoints[1], yPoints[1]);
 return(status=AIN_OK);
}

//_____set (only) two suppert points____________________________
// change: status, numPoints, xPoints, yPoints, ain_k, ain_d
// return: status (0=OK)
int Ain::setRefPoints(int x1, float y1, int x2, float y2)
{
 //-----check input---------------------------------------------
 int status1=calcKD(x1, y1, x2, y2);
 if(status1!=AIN_OK) return(status=status1);
 numPoints=2;
 if(x1<x2) {
  xPoints[0]=x1; yPoints[0]=y1;
  xPoints[1]=x2; yPoints[1]=y2;
 } 
 else {
  xPoints[0]=x2; yPoints[0]=y2;
  xPoints[1]=x1; yPoints[1]=y1;
 }
 calcKD(x1, y1, x2, y2);
 return(status=AIN_OK);
}

//_____set suppert points (minimum 2, maximum AIN_POINTS_MAX)___
// sets value of property status
// return: status (0=OK)
int Ain::setRefPoints(int num, int xValues[], float yValues[])
{
 //------check number of support points-------------------------
 if(num<2 || num>AIN_POINTS_MAX) return(status=AIN_ERR_RANGE);
 //------check x-limits-----------------------------------------
 int xOld=xValues[0];
 if(xOld<0 || xOld>ainMax) return(status=AIN_ERR_X_RANGE);
 //------check for increasing x-values--------------------------
 for(int i=1; i<num; i++)
 {
  if(xValues[i]<=xOld) return(status=AIN_ERR_RANGE);
  xOld=xValues[i];
 }
 //-----values ok, accept them----------------------------------
 numPoints=num;
 for(int i=0; i<numPoints; i++)
 {
  xPoints[i]=xValues[i];
  yPoints[i]=yValues[i];
 }
 return(status=AIN_OK);
}

//_____return reference points as String________________________
String Ain::getsRefPoints()
{
 char s0[18];
 String s1;
 for(int i=0; i<numPoints; i++)
 {
  sprintf(s0,"(%04d/",xPoints[i]);
  s1+=String(s0);
  s1+=String(dtostrf(yPoints[i],decimals+2,decimals,s0));
  s1+=") ";
 }
 return s1;
}

// *************************************************************
//     some more methods
// *************************************************************

//_____set number of decimal places for String value____________
void Ain::setDecimals(int decimal_places) {
 if((decimal_places<0)||(decimal_places>10))
 { status=AIN_ERR_DECIMALS; return; }
 decimals=decimal_places;
}

//_____get number of decimal places_____________________________
int Ain::getDecimals() { return decimals; }

//_____calculate voltage and return it as float_________________
// sets value of property status
// called by getVoltage()
// return: voltage
float Ain::getVoltage(int ain_code) {
 int i=0;
 if(ain_code<0) { status=AIN_ERR_X_RANGE; return 0; }
 if(ain_code>ainMax) {status=AIN_ERR_X_RANGE; return yPoints[numPoints-1];}
 int imax=numPoints-1;
 for(i=0; i<imax; i++)
 {
  if(ain_code < xPoints[i])
  {
   if(i==0)
    calcKD(xPoints[i], yPoints[i], xPoints[i+1], yPoints[i+1]);
   else
    calcKD(xPoints[i-1], yPoints[i-1], xPoints[i], yPoints[i]);
   status=AIN_OK;
   float f1=ain_d + ain_k*ain_code;
   return f1;
  }
 }
 i=numPoints-2;
 calcKD(xPoints[i], yPoints[i], xPoints[i+1], yPoints[i+1]);
 status=AIN_OK;
 float f1=ain_d + ain_k*(double)ain_code;
 return f1;
}


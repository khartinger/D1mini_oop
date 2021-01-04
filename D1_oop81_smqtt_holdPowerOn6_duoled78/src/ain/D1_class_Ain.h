//_____D1_class_Ain.h_________________________171129-201103_____
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
#ifndef D1_CLASS_AIN_H
#define D1_CLASS_AIN_H
#include "Arduino.h"    

//-------status-------------------------------------------------
#define  AIN_OK                   0    // success
#define  AIN_ERR_X_RANGE          1    // x<0 or x>1023
#define  AIN_ERR_X_EQUAL          2    // x1=x2
#define  AIN_ERR_Y_RANGE          4    // y<0
#define  AIN_ERR_DECIMALS         8    // decimals <0, >20
#define  AIN_ERR_RANGE           16    // wrong number of points
//-------hardware specific--------------------------------------
#define  AIN_PIN_D1MINI          A0    // A0=17
#define  AIN_PIN_ESP32D1         36    // GPIO36
//-------analog in parameter------------------------------------
#define  AIN_POINTS_DEFAULT       2    // default support points
#define  AIN_POINTS_MAX          16    // maximum support points
#define  AIN_MAX10             1023    // maximum 10bit: 2^10 -1
#define  AIN_MAX12             4095    // maximum 12bit: 2^12 -1
#define  AIN_MAX_VOLTAGE        3.2    // maximum 3.2V

class Ain {
 protected:
  //-----properties---------------------------------------------
  int    iBoard;                       // 1=D1mini, 2=ESP32D1
  int    pinAin;                       // D1mini A0=17, ESP32D1 36
  int    numPoints;                    // number support points
  int    status;                       // 0..6
  int    ainMax;                       // 1023 | 4095 | ..
  int    ainValue;                     // voltage 0..1023 (4095)
  int    xPoints[AIN_POINTS_MAX];      // voltages 0..1023 (4095)
  float  yPoints[AIN_POINTS_MAX];      // voltages in Volt
  double ain_k, ain_d;                 // line parameters
  int    decimals;                     // for display as string
 public:
  //-----constructor & co---------------------------------------
         Ain();                        // selects D1mini
         Ain(int boardType);           // 1=D1mini, 2=ESP32D1
         Ain(int boardType, int pin);  // type 1|2, pin number
 protected:
  void   setup(int boardType,int pin); // setup properties
  int    calcKD(int x1, float y1, int x2, float y2);
  
 public:
  //-----get (new) measurement values---------------------------
  int    getStatus();                  // status value
  int    getAin1023();                 // old: get analog in
  int    getAinValue();                // new: get analog in
  int    getAinValueMax();             // =ainMax=1023|4095|...
  float  getVoltage();                 // read ain, calc voltage
  String getsVoltage();                // getVoltage() as string
  String getsVoltage(int decimal_places);
  //-----get last measurement values (without new measurement)--
  int    getLastAin1023();             //old: get last analog in
  int    getLastAinValue();            //new: get last analog in
  float  getLastVoltage();             // last voltage
  String getsLastVoltage();            // last voltage as string
  String getsLastVoltage(int decimal_places);
  //-----support points-----------------------------------------
  int    setRefPoints();               // use default values
  int    setRefPoints(int x1, float y1, int x2, float y2);
  int    setRefPoints(int num, int xValues[], float yValues[]);
  String getsRefPoints();

  //-----some more methods--------------------------------------
  void   setDecimals(int decimal_places);
  int    getDecimals();
  float  getVoltage(int ain_code);     // just calc voltage
};
#endif

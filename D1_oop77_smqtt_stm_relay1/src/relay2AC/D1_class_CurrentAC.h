//_____D1_class_CurrentAC.h___________________201203-201206_____
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

#ifndef D1_CLASS_CURRENT_AC_H
#define D1_CLASS_CURRENT_AC_H

#define  CURRENTAC_BOARD_DEFAULT     1 // 1 = Wemos D1 mini
#define  CURRENTAC_BOARD_ESP32D1     2 // 1 = ESP32 D1 mini
#define  CURRENTAC_PIN_AIN_D1       A0 // A0=17 default D1mini
#define  CURRENTAC_PIN_AIN_ESP      36 // default pin ESP32 D1
#define  CURRENTAC_A_ON          0.044 // I=P/U=10W/230V=0.044A
#define  CURRENTAC_ADC_MAX10      1023 // maximum 10bit: 2^10 -1
#define  CURRENTAC_ADC_MAX12      4095 // maximum 12bit: 2^12 -1
#define  CURRENTAC_MAX_VOLTAGE     3.2 // maximum 3.2V
#define  CURRENTAC_50HZ_PERIODS      4 // # measure 50Hz periods
#define  CURRENTAC_POINTS_MAX       20 // maximum support points

#include "Arduino.h"                   // D1
//_____default support points for D1 mini_______________________
const int   numPointsD1mini_ = 11;
const int   x4nomD1mini_     = 947;    // ain=947 -> iAC=2A
const float y4nomD1mini_     = 2.0;    // 2A (@ ain=947)
const int   xPointsD1mini_[] = {0,3,    4,   10,   50,   100,   800,  900, 947, 1000, 1010};
const float yPointsD1mini_[] = {0,0,0.011,0.044,0.144,0.2509,1.6833,1.883,2.00,2.133,2.428};
//_____default support points for ESP32 D1 mini_________________
const int   numPointsESP32D1_ = 14;
const int   x4nomESP32D1_     = 3920;  // ain=3920 -> iAC=2A
const float y4nomESP32D1_     = 2.0;   // 2A (@ ain=3920)
const int   xPointsESP32D1_[] = {0,    1,    2,   10, 100,   500,  2500, 3000, 3550,3920, 4000,4060,4094,4095};
const float yPointsESP32D1_[] = {0,0.118,0.119,0.128,0.18,0.3952,1.4852,1.718,1.892,   2,2.036,2.09,2.16,2.17};

class CurrentAC {
 //------properties---------------------------------------------
 protected:
  //.....general properties.....................................
  int    iBoard_;                      // 1=D1mini, 2=ESP32D1
  //.....properties for current measuring.......................
  float  a_;                           // actual current in A
  float  aOn_;                         // current limit for "on"
  int    ainLast_;                     // ain value to a_
  int    ainMax_;                      // 10bit: 1023, 12: 2095
  int    x4nom_;                       // nominal x-value
  int    x4nomDefault_;                // default nominal x (947,3920...)
  float  y4nom_;                       // nominal y-value=current
  float  y4nomDefault_;                // default nominal y (2A)
  bool   bLastOn_;                     // true=on|false=off
  int    num50HzPeriods_;              // # measure 50Hz periods
  //.....properties for analog in...............................
  int    ainPin_;                      // D1mini A0=17, ESP32D1 36
  int    numPoints_;                   // number support points
  int    xPoints_[CURRENTAC_POINTS_MAX];// current 0..1023 (4095)
  float  yPoints_[CURRENTAC_POINTS_MAX];// current in ampere

 //------constructor & co---------------------------------------
 public:
  CurrentAC();                         // type 1=D1mini,2A,0.1A
  CurrentAC(float inom_A, float ion_A);// type 1=D1mini
  CurrentAC(int boardType);            // 1=D1mini, 2=ESP32D1
  CurrentAC(int boardType, float inom_A, float ion_A);
  CurrentAC(int boardType, float inom_A, float ion_A, int pin);
 protected:
  void setup(int boardType, float inom_A, float ion_A, int pin);
 
 //------get measurement values---------------------------------
 public:
  float  getCurrent();                 // measure current in A
  int    getLastAin();                 // ADC value of current
  bool   isOn();                       // true: current is on (>aOn_)
  bool   isOff();                      // true: current is off (<=aOn_)
  bool   isChange();                   // change since last call
  float  getCurrentOn();               // current limit for "on"
  float  getNominalCurrent();          // 
 //------set parameter------------------------------------------
  bool   setCurrentOn(float onAmpere); // current limit for "on"
  bool   setNominalCurrent(float inom_A); // nominal current
  bool   setx4nom(int x4nom);          //ADC value for nom current
  bool   setNumberOf50HzPeriods(int periods); //number 50Hz per.
  bool   setBoardType(int boardType);  // 1=D1mini, 2=ESP32D1
  bool   setPinAin(int pin);           // A0
 
 //-----support points to calculate current---------------------
  bool   setRefPoints();               // use default values
  bool   setRefPoints(int num, int xValues[], float yValues[]);
  String getsRefPoints();              // Ref points as string
  
 //------some more methods--------------------------------------
  float  getCurrent(int ain_code);      //calculate current in A
  int    measure();                     // measure current
};
#endif

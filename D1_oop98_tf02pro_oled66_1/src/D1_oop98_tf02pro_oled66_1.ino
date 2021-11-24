//_____D1_oop98_tf02pro_oled66_1.ino_____________khartinger_____
// This program for the D1 mini or ESP32mini (ESP32-WROOM-32) 
// does the following:
// 1. Periodically read TFmini data via serial interface and 
//    calculate the distance (in cm).
// 2. Display of the distance on a display (OLED 0,66").
// 3. Output of a melody, if a minimum distance (limit) is 
//    not reached.
// 4. Setting of the limit value by means of a potentiometer.
//    (Read in as analog value) 
// Hardware:
// 1. D1 mini or ESP32mini (ESP32-WROOM-32)
// 2. do-it-yourself TF adapter shield: potentiometer for input
//    of analog value, pins for LIDAR connection (TFmini or TF02)
// 3. buzzer shield @ D5 (GPIO18)
// 4. OLED Shield 0.66" (64x48 pixel, I2C-addr 0x3C)
// 5. LIDAR TFmini (0.1m to 12m) or TF02 (0.1m to 40m)
// Created by Karl Hartinger, November 24, 2021.
// Changes: 
// 2021-11-24 First release
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include  "src/screen1b/D1_class_Screen1b.h"
#include "src/buzzer1/D1_class_Buzzer1.h"
#include <HardwareSerial.h>

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define PIN_LED        2              // D4=IO2
 #define PIN_AIN        36             // default pin ESP32 D1
 #define PIN_BUZZER     18             // D5 -> 18
 #define _SERI_         mySerial       // Serial, Serial1, Serial2
 HardwareSerial         _SERI_(2);     // 0..2
 #define PIN_RXD        1              // 1 ESP32! 0:3, 2:16
 #define PIN_TXD        3              // 3 ESP32! 0:1, 2:17 (0...31)
 #define AIN_MASK       0xFFF0         // changing mask
#else
 #define PIN_LED        D4             // D4=IO2
 #define PIN_AIN        A0             // A0=17 default D1mini
 #define PIN_BUZZER     D5             // D5 -> 18
 #define _SERI_         Serial         // default Serial
 #define AIN_MASK       0xFFF8         // 
#endif
#define  BUFFER_SIZE    80             // receive buffer size
#define  IIN_MAX        8              // data package max index
const int HEADER=0x59;                 // data package frame header
uint8_t  caIn[BUFFER_SIZE];            // receive buffer
Screen1  display_;                     // display object

//.......buzzer object and interrupt service routine............
Buzzer1 buzzer(PIN_BUZZER);            // buzzer object
#if defined(ESP32) || defined(ESP32D1)
 IRAM_ATTR void buzzer1_onTime() { buzzer.timer_ISR(NULL); }
#else
 ICACHE_RAM_ATTR void buzzer1_ISR(void *pArg) { buzzer.timer_ISR(NULL); }
#endif

String melody1="$T120L8MSO5 GC GC";
//String melody1="$T120L16MSO5 CG8 CG4";
//String melody1="$T120L8MS CDEF GAG4 FDB4 GE>C4 <CDEF GAG4 FDB>D C4P4"
//               "<AF>C<MLB16MSA16 GEG4 FDB4 GE>C4 <AF>C<MLB16MSA16"
//               "GEG4 FDB>D C4P4";

//_______Global values__________________________________________
int iIn=-1;                            // index of received byte
int distOld=-1;                        // old distance value
int strengthOld=-1;                    // old strength value
int distMin=0;                         // distance limit
int distMinOld=-1;                     // old distance value

//_____setup____________________________________________________
void setup() 
{
 //-----start Serial--------------------------------------------
 #if defined(ESP32) || defined(ESP32D1)
  _SERI_.begin(115200, SERIAL_8N1, PIN_RXD, PIN_TXD);
 #else
  _SERI_.begin(115200, SERIAL_8N1);
 #endif
 //-----init pins-----------------------------------------------
 pinMode(PIN_LED, OUTPUT);                  // led pin as output
 pinMode(PIN_AIN, OUTPUT);                  // analog in
 //------setup buzzer-------------------------------------------
 #if defined(ESP32) || defined(ESP32D1)
  buzzer.setISR32(buzzer1_onTime);
 #else
  buzzer.setISR(buzzer1_ISR);               // set name of ISR
 #endif
 buzzer.setMelody(melody1);                 // set new melody
 //------I2C begin, init 0.66" OLED display---------------------
 if(!display_.begin())                      // start I2C+display
 {
   //----blink led----------------------------------------------
   digitalWrite(PIN_LED, digitalRead(PIN_LED)^1); // invert LED
   delay(250);
 }
 display_.screen112Clear(1,"Abstand", 'l');       // clear oled
}

//_____loop_____________________________________________________
void loop() {
 //===========read analog in: distance < ain -> buzzer melody===
 int ain = analogRead(PIN_AIN);
 if((ain & AIN_MASK) != distMinOld)
 {
   distMinOld = ain & AIN_MASK;
   distMin = ain;
   String sDistMin=String(distMin);
   if(distMin<1000) sDistMin=" "+sDistMin;
   display_.screen112(3,"Limit:"+sDistMin);
 }
 //===========check serial interface for bytes==================
 size_t len = _SERI_.available();      // byte(s) received?
 display_.screen112(4,"len: "+String(len));
 if(len)
 {//----------yes: byte(s) received-----------------------------
  size_t i=0;
  display_.screen112(4,"len: "+String(len));
  digitalWrite(PIN_LED, digitalRead(PIN_LED)^1);  // invert LED
  while(i<len)
  {//---------for all received bytes----------------------------
   if(iIn<0)
   {//--------not in valid data package: search for HEADER------
    if(_SERI_.read()==HEADER) { iIn=0; caIn[iIn++]=HEADER; }
   }
   else
   {//--------receiving data package----------------------------
    if(iIn==1) {
     if(_SERI_.read()==HEADER) caIn[iIn++]=HEADER;
     else iIn=-1;                      // read error
    }
    else
    { //------receiving byte [2]..[8]---------------------------
     if(iIn<IIN_MAX)
      caIn[iIn++]=_SERI_.read();
     else
     {//-------read last byte of data package-------------------
      caIn[iIn]=_SERI_.read();
      if(buzzer.finished()) 
      {
        int check=caIn[0]+caIn[1]+caIn[2]+caIn[3]+caIn[4]+caIn[5]+caIn[6]+caIn[7];
        if(caIn[8]==(check&0xff))// check the received data
        { //--checksum ok---------------------------------------
         int dist=caIn[2]+caIn[3]*256;        // calc distance value
         //int strength=caIn[4]+caIn[5]*256;    // calc signal strength
         //---empirical correction of the distance--------------
         // dist=dist-5; if(dist<0) dist=0;
         if(dist!=distOld)
         {
          String sDist=String(dist);
          display_.screen112(2,sDist);
          distOld=dist;
          if(dist<distMin) buzzer.start();
         }
        } // end checksum ok
      } // end buzzer finished
      iIn=-1;                           // data package finished
     }
    } // end receiving byte [2]..[8]
   } // end receiving data package
   i++;                                // read next byte
  } // end for all received bytes

 } // end if(len)
}

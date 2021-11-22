//_____D1_oop95_buzzersound2.ino_________________khartinger_____
// This demo program is used to demonstrate the capabilities 
// of the Buzzer1 class for the D1 mini or ESP32 D1mini:
// setup():
// 1. Play a short 440Hz piep tone for 4 times (= 1s)
// 2. Wait for 1.5 seconds (1sec tone, only 0.55sec silence ;)
// 3. Start to play the Big Ben melody
// 4. Pause after 1,5 seconds for 1 second
// 5. Continue playing the Big Ben Melody
// main():
// 6. When the playback of the Big Ben Melody is finished,
//    another medody is started (song of joy).
//    This melody should be played twice.
// 7. To show that the main loop is not blocked, it is counted 
//    there from 0 to 99 in 0.5 sec intervals.
// * All actions are displayed via serial interface. (115200Bd)
// * To use the program for a D1 mini or ESP32D1mini, only the
//   corresponding #define statement must be used. :)

// The software is based on a Arduino Melody Player by ericBcreator.
// https://www.hackster.io/ericBcreator/a-m-p-arduino-music-player-8013b6
// commands:
// CDEFGABn  notes, n = note length: 1, 2, 4, 8, 16, 32, 64,
//       2, 4, 8, 16, 32 dotted (or 3, 6, 12, 24, 48),
//       4, 8, 16, 32 triplet 34, 38, 316, 332
//       defaults to Ln if n is omitted
// Rn    rest, n = note length. Uses Ln if n is omitted
// Tn    tempo n = 32-255 (default is 120 BPM, 4/4 time base)
// On    octave n = 1-8 (default is 4)
// Ln    n = default note length (default is 4, quarter note)
// MN    music normal:      7/8 note length duration (default)
// ML    or music legato:   full length duration
// MS    or music staccato: 3/4 note length duration
// MU    mute mode

// <     octave down (range 1-8)
// >     octave up (range 1-8)
// [     transpose down a step (range -12 to 12)
// ]     transpose up a step (range -12 to 12)
// #     sharp (place after note, D#n)
// -     flat (place after note, D-n)
// .     dotted note (note length will be increased with 50%)
// $     reset the default settings.
//       Recommended at the start of a new song.
// Note: spaces are ignored, use them for readability
// Example: Big Ben melody:
//       playMelody("$T120 L4 O5 ECD<GR2 G>DEC R2");
//
// Hardware: 1. WeMos D1 Mini or ESP32-D1mini
//           2. Buzzer Shield (@ D5)
// Created by Karl Hartinger, June 06, 2021.
// Changes:
// 2021-11-22 add ESP32 functionality
// Released into the public domain.

//#define D1MINI          1              // ESP8266 D1mini +pro
#define ESP32D1         2              // ESP32 D1mini
#include "src/buzzer1/D1_class_Buzzer1.h"

#define  DEBUG95        true
#define  VERSION95      "2021-11-22 D1_oop95_buzzersound2"

//_______sensors, actors, global variables______________________
//.......buzzer object and interrupt service routine............
Buzzer1 buzzer;                             // buzzer object
#if defined(ESP32) || defined(ESP32D1)
 IRAM_ATTR void buzzer1_onTime() { buzzer.timer_ISR(NULL); }
#else
 ICACHE_RAM_ATTR void buzzer1_ISR(void *pArg) { buzzer.timer_ISR(NULL); }
#endif

String melodyBigBen="$T120 L4 O5 ECD<GR2 G>DEC R2";
String melodyFreude="$T240 MN EEFG GFED CCDE ML E. MN D8D2 R1R1";
//String melodyJingleBells="$T240 EEE2 EEE2 EGC.D8 E1 FFF.F8 FEEE8E8 EDDE D2G2 EEE2 EEE2 EGC.D8 E1 FFF.F8 FEEE8E8 GGFD C1 R1";

//_____SETUP____________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 //if(DEBUG95)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}

 //------buzzer: set interrupt service routine------------------
 //buzzer.setISR(buzzer1_ISR);                // set name of ISR
 #if defined(ESP32) || defined(ESP32D1)
  buzzer.setISR32(buzzer1_onTime);
 #else
  buzzer.setISR(buzzer1_ISR);               // set name of ISR
 #endif

 //------output sound-------------------------------------------
 if(DEBUG95) Serial.println("4x Piep");
 buzzer.setTone(440,200,50);                // one tone
 buzzer.start(4);                           // piep 4x
 if(DEBUG95) Serial.println("...wait 1,5 sec... (0,55sec without sound ;))");
 buzzer.delay(1500);                        // wait 2s
 if(DEBUG95) Serial.println("Set Big Ben melody");
 buzzer.setMelody(melodyBigBen);            // set new melody
 if(DEBUG95) Serial.println("Start Big Ben melody");
 buzzer.start();                            // start melody
 if(DEBUG95) Serial.println("...wait 1.5 sec while playing melody...");
 buzzer.delay(1500);                        // wait 1.5s
 buzzer.pause();                            // pause playing
 if(DEBUG95) Serial.println("Pause Big Ben melody for 1 sec");
 buzzer.delay(1000);                        // wait 1 sec
 buzzer.goon();                             // restart playing
 if(DEBUG95) Serial.println("Go on playing Big Ben melody #1");
 Serial.println("setup(): --Finish2--");
}

int counter=0;                              // start value
bool start1=true;                           // do playing 1
bool start2=false;                          // do playing 2

//_______MAIN LOOP______________________________________________
void loop() {
 //------Show that program is alive: counting + serial output---
 counter++;                                 // increment counter
 if(counter>99) counter=0;                  // avoid overflow
 if(DEBUG95) {
  if(counter==1) Serial.print("loop(): counting... ");
  Serial.print(counter); Serial.print(" "); // print counter val
  if(counter%20==0) Serial.println();       // new line after 20
 }
 //------Melody #1 (Big Ben) finished?--------------------------
 if(start1 && buzzer.finished())
 {//.....Melody #1 finished. Play melody #2.....................
  if(DEBUG95) Serial.println("\nFinished melody #1");
  start1=false;                             // start only 1 time
  start2=true;                              // prepare finish2
  if(DEBUG95) Serial.println("Set melody #2");
  buzzer.setMelody(melodyFreude);           // set new melody
  if(DEBUG95) Serial.println("Start melody #2 2x");
  buzzer.start(2);                          // play it 2x
 }
 
 //------Melody #2 finished?------------------------------------
 if(start2 && buzzer.finished())
 {//.....yes: print 'playing #2 finished' message...............
  start2=false;                             // finished #2
  if(DEBUG95) Serial.print("\nFinished melody #2 ==> ");
  if(DEBUG95) Serial.println("Only counting process active anymore");
 }
 buzzer.delay(500);                         // wait (counter)
}

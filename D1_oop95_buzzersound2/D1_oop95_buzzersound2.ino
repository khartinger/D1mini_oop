//_____D1_oop95_buzzersound2.ino_________________khartinger_____
// This demo program is used to demonstrate the capabilities 
// of the Buzzer1 class for the D1 mini:
// setup():
// 1. Play a short 880Hz piep tone for 4 times (= 1s)
// 2. Wait for 2 seconds (1sec tone, 1sec silence ;)
// 3. Start to play the Big Ben melody
// 4. Pause after 1,5 seconds for 2 seconds
// 5. Continue playing the Big Ben Melody
// main():
// 6. When the playback of the Big Ben Melody is finished,
//    another medody is started (song of joy).
//    This melody should be played twice.
// 7. To show that the main loop is not blocked, it is counted 
//    there from 0 to 999 in 0.5 sec intervals.

// The software is based on a Arduino Melody Player by ericBcreator.
// https://www.hackster.io/ericBcreator/a-m-p-arduino-music-player-8013b6
// commands:
// CDEFGABn  notes, n = note length: 1, 2, 4, 8, 16, 32, 64,
//       2, 4, 8, 16, 32 dotted (or 3, 6, 12, 24, 48),
//       4, 8, 16, 32 triplet 34, 38, 316, 332
//       defaults to Ln if n is omitted
// Rn    rest, n = note length. defaults to Ln if n is omitted
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
// Hardware: 1. WeMos D1 Mini
//           2. Buzzer Shield (@ D5)
// Created by Karl Hartinger, June 06, 2021.
// Changes: -
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
#include "src/buzzer1/D1_class_Buzzer1.h"

#define  DEBUG95        true
#define  VERSION95      "2021-06-04 D1_oop95_buzzersound2"

//_______sensors, actors, global variables______________________
//.......buzzer object and interrupt service routine............
Buzzer1 buzzer;                             // buzzer object
ICACHE_RAM_ATTR void buzzer1_ISR(void *pArg) { buzzer.timer_ISR(NULL); }

String melodyBigBen="$T120 L4 O5 ECD<GR2 G>DEC R2";
String melodyFreude="$T240 MN EEFG GFED CCDE ML E. MN D8D2 R1R1";
//String melodyJingleBells="$T240 EEE2 EEE2 EGC.D8 E1 FFF.F8 FEEE8E8 EDDE D2G2 EEE2 EEE2 EGC.D8 E1 FFF.F8 FEEE8E8 GGFD C1 R1";

//_____SETUP____________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 //if(DEBUG95)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------buzzer: set interrupt service routine------------------
 buzzer.setISR(buzzer1_ISR);                // set name of ISR
 //------output sound-------------------------------------------
 if(DEBUG95) Serial.println("4x Piep");
 buzzer.setTone(880,200,50);                // one tone
 buzzer.start(4);                           // piep 4x
 if(DEBUG95) Serial.println("Wait 2 secs");
 buzzer.delay(2000);                        // wait 2s
 if(DEBUG95) Serial.println("Set Big Ben melody");
 buzzer.setMelody(melodyBigBen);            // set new melody
 if(DEBUG95) Serial.println("Start Big Ben melody");
 buzzer.start();                            // start melody
 buzzer.delay(1000);                        // wait 1s
 buzzer.pause();                            // pause playing
 if(DEBUG95) Serial.println("Pause Big Ben melody for 2 sec");
 buzzer.delay(2000);                        // wait 2 sec
 buzzer.goon();                             // restart playing
 if(DEBUG95) Serial.println("Go on playing Big Ben melody #1");
 Serial.println("setup(): --Finish2--");
}

int counter=-1;                             // start value
bool start1=true;                           // do playing 1
bool start2=false;                          // do playing 2

//_______MAIN LOOP______________________________________________
void loop() {
 //------Show that program is alive: counting + serial output---
 counter++;                                 // increment counter
 if(counter>999) counter=0;                 // avoid overflow
 Serial.print(counter); Serial.print(" ");  // print counter val
 if(counter%20==0) Serial.println();        // lew line after 20
 
 //------Melody #1 (Big Ben) finished?--------------------------
 if(start1 && buzzer.finished())
 {//.....Melody #1 finished. Play melody #2.....................
  if(DEBUG95) Serial.println("\nFinished melody #1");
  start1=false;                             // start only 1 time
  start2=true;                              // prepare finish2
  buzzer.setMelody(melodyFreude);           // set new melody
  if(DEBUG95) Serial.println("Start melody #2");
  buzzer.start(2);                          // play it 2x
 }
 
 //------Melody #2 finished?------------------------------------
 if(start2 && buzzer.finished())
 {//.....yes: print 'playing #2 finished' message...............
  start2=false;                             // finished #2
  if(DEBUG95) Serial.println("\nFinished melody #2");
 }
 buzzer.delay(500);                         // wait (counter)
}

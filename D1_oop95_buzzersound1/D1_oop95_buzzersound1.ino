//_____D1_oop95_buzzersound1.ino_________________khartinger_____
// This simple demo program shows the use of the Buzzer1 class
// for the D1mini to play a song:
// * play the Big Ben melody once
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

#include "src/buzzer1/D1_class_Buzzer1.h"

//_______sensors, actors, global variables______________________
//.......buzzer object and interrupt service routine............
Buzzer1 buzzer;                             // buzzer object
ICACHE_RAM_ATTR void buzzer1_ISR(void *pArg) {buzzer.timer_ISR(NULL);}

//_____SETUP____________________________________________________
void setup() {
 String melody="$T120 L4 O5 ECD<GR2 G>DEC R2";
 buzzer.setISR(buzzer1_ISR);                // set name of ISR
 buzzer.setMelody(melody);                  // set melody
 buzzer.start();                            // start playing
}

//_______MAIN LOOP______________________________________________
void loop() {
 yield();
}

//_____D1_class_Buzzer1.h________________________khartinger_____
// The Buzzer1 class for the D1mini or ESP32-D1mini is used to 
// output tones and melodies.
// In order not to block the main loop during the sound output,
// it uses the sound function (for PWM tone frequency) and 
// timer interrupt (for duration) for sound output.
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
// Hardware: 1. WeMos D1 Mini
//           2. Buzzer Shield (@ D5)
// Created by Karl Hartinger, June 06, 2021.
// Changes:
// 2021-11-22 add ESP32D1mini timer and sound handling
// Released into the public domain.

#ifndef D1_CLASS_BUZZER1_H
#define D1_CLASS_BUZZER1_H
#include "Arduino.h"                        // D1, String, ...
#include <iostream>                         // cout

//_______defines________________________________________________
#if defined(ESP32) || defined(ESP32D1)
 #define DEFAULT_BUZZER_PIN  18             // D5=18
#else
 #define DEFAULT_BUZZER_PIN  D5             // D5=18
 extern "C" {
 #include "user_interface.h"                // os_timer_t
 }
#endif

#define DEBUG_B              false // true // false
#define DEFAULT_TEMPO        120            // 120 beats/minute
#define DEFAULT_OCTAVE       4              // octave #4
#define DEFAULT_NOTELENGTH   4              // quarter note
#define DEFAULT_NOTELENTYPE  0              // 0: normal 7/8
#define DEFAULT_TRANSPOSE    0              // note up/down in pitch
#define TIMER32_NUM          0              // 0..3
#define TIMER32_PRESCALER    8000           // 80MHz: 80=1us
#define TIMER32_EDGE         true           // true=rising edge
#define BUZZER_CHANNEL       0              // 0..

class Buzzer1 {
 //======properties=============================================
 protected:
  int              pinBuzzer_;              // buzzer pin (D5)
  String           melody_;                 // melody notes
  int              repeat_;                 // number of repeat
  int              melr_;                   // actual repeat #
  int*             pMelf_;                  // frequency [Hz]
  int*             pMeld_;                  // duration  [ms]
  int              maxmem_;                 // reserved mem
  int              icurf_;                  // index of f array
  int              icurd_;                  // index of d array
  int              imaxf_;                  // max. index of f array
  int              imaxd_;                  // max. index of d array
  int              numNotes_;               // number of notes
#if defined(ESP32) || defined(ESP32D1)
  int              timer32Num_;             // timer number 0..3
  int              buzzerChannel_;          // PWM channel 0..15
  hw_timer_t       *pMyTimer;               // pointer to timer var
  void             (*pISR)();               // pointer to ISR
#else
  os_timer_t       myTimer;                 // timer object
#endif
  bool             finished_;                // playback finished
  //.....general melody parameter (d=duration, f=frequency).....
  unsigned char    d_tempo;                 // BPM=beats/minute (120)
  unsigned int     d_defaultNoteLength;     // general noteLength (4)
  unsigned int     d_noteLengthType;        // articulation (0)
  // 0: normal 7/8, 1:  legato 1/1, 2: staccato 3/4, 9: mute
  unsigned char    f_octave;                // octave (default 4)
  int              f_transpose;             // note up/down in pitch (0)
  //=====constructor & co=======================================
 public:
  Buzzer1();                                // use default value
  Buzzer1(int pin);                         // specification pin#
 protected:
  //_____initialize properties. Called by constructor(s)________
  void setup(int pin);                      // called by constructors
 public:
  //=====restore properties and objects=========================
  bool begin();                             // reset properties
  void restoreSoundSettings();              // reset melody param
 //======setter and getter methods==============================
  bool setMelody(String melody);            // set melody
  bool setTone(int f, int msOn, int msOff); // set only one tone
  bool finished();                          // playing finished
 //======working methods========================================
  void setISR(void (*isr_)(void *pArg));    // interrupt service
  void setISR32(void (*isr_)());            // interrupt service
  void start();                             // start playing 1x
  void start(int repeat);                   // start, play x times
  void stopp();                             // stopp all playing
  void pause();                             // pause playing
  void goon();                              // start after pause
  void delay(unsigned long waitMillis);     // wait milli secs
  bool isPlaying();                         // = not finished
 //======help methods===========================================
  void timer_ISR(void *pArg);               // play the notes
  bool parseMelody(String melody);          // notes to dur,freq
  bool isNum(unsigned char value);          // is char 0..9?
  int  countNotes(String melody);           // count A..GR,a..gr
  double noteLengthToMS(unsigned int noteLength); // --> millis
  unsigned int noteToFrequency(int note);   // --> frequency
 //======special helper functions===============================
 protected:
  void _noTone();
  void _tone(int freq_);
  bool _startTimer(int duration_ms_);
  void _endTimer();
};
#endif
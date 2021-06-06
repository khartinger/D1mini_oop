//_____D1_class_BuzzerList.cpp______________________khartinger_____
// The BuzzerList class for the D1mini is used to output 
//tones and melodies.
// In order not to block the main loop during the sound output,
// it uses the timer interrupt for sound output.
// SimpleLists are used to store note duration and frequency.
// The software is based on a Arduino Melody Player by ericBcreator.
// https://www.hackster.io/ericBcreator/a-m-p-arduino-music-player-8013b6
//
// Hardware: 1. WeMos D1 Mini
//           2. Buzzer Shield (@ D5)
// Created by Karl Hartinger, June 06, 2021.
// Changes: -
// Released into the public domain.

#include "D1_class_BuzzerList.h"

// *************************************************************
//       constructor & co
// *************************************************************

//_______constructor____________________________________________
BuzzerList::BuzzerList() {
 setup(DEFAULT_BUZZER_PIN);
}

//_______constructor 2__________________________________________
BuzzerList::BuzzerList(int pin) {
 setup(pin);
}

//_______setup (called by constructor)__________________________
void BuzzerList::setup(int pin)
{
 pinBuzzer_=pin;                            // set buzzer pin
 pinMode(pinBuzzer_, OUTPUT);               // pin is output
 begin();                                   //init props,objects
}

//_______init objects___________________________________________
bool BuzzerList::begin()
{
 int ret=true;
 //------set properties-----------------------------------------
 melf_.clear();                             // frequency [Hz]
 meld_.clear();                             // duration  [ms]
 melody_="";                                // melody notes
 melr_=1;                                   // first play
 repeat_=1;                                 // play melody 1x
 finished_=true;                            // not playing ;)
 restoreSoundSettings();                    // init melody param
 return ret;
}

//_______reset melody parameter_________________________________
void BuzzerList::restoreSoundSettings()
{
 d_tempo=DEFAULT_TEMPO;                     // 120 beats/minute
 d_defaultNoteLength=DEFAULT_NOTELENGTH;    // general noteLength
 d_noteLengthType=DEFAULT_NOTELENTYPE;      // articulation (0)
 f_octave=DEFAULT_OCTAVE;                   // octave (default 4)
 f_transpose = DEFAULT_TRANSPOSE;           // move note up/down in pitch (0)
}

// *************************************************************
//       setter and getter methods
// *************************************************************

//_______set melody_____________________________________________
bool BuzzerList::setMelody(String melody) {
 return parseMelody(melody);
}

//_______set a tone_____________________________________________
// repeat=-1: endless
// result in SimpleList melf_ and meld_
bool BuzzerList::setTone(int f, int msOn, int msOff) {
 if(f<1 || msOn<1 || msOff<1) return false;
 melf_.clear();                             // reset frequency
 meld_.clear();                             // reset duration
 melf_.append(f);                           // tone on
 meld_.append(msOn);                        // tone on
 melf_.append(0);                           // tone off
 meld_.append(msOff);                       // tone off
 melf_.append(0);                           // end f=0
 meld_.append(0);                           // end d=0
 return true;
}

//_______Is the playback of the music finished?_________________
bool BuzzerList::finished() { return finished_; }

// *************************************************************
//       working methods
// *************************************************************

//_______set name of external interrupt routine_________________
void BuzzerList::setISR(void (*isr_)(void *pArg)) {
 os_timer_setfn(&myTimer, isr_, NULL);
}

//_______start playing the melody once__________________________
void BuzzerList::start() { start(1); }

//_______start playing the melody_______________________________
void BuzzerList::start(int repeat) { 
 melr_=1;
 repeat_=repeat;
 meld_.moveToStart();
 melf_.moveToStart();
 finished_=false;
 restoreSoundSettings();
 os_timer_arm(&myTimer, 1, false);
}

//_______stopp playing the melody_______________________________
void BuzzerList::stopp() { 
 pause();                                   //stopp timer+noTone
 meld_.moveToStart();                       // reset melody
 melf_.moveToStart();                       // reset melody
 finished_=true;                            // stopp playing
}

//_______pause playing the melody_______________________________
void BuzzerList::pause() { 
 os_timer_disarm(&myTimer);                 // stopp timer
 noTone(pinBuzzer_);                        // sound finished
}

//_______start melody after pause_______________________________
void BuzzerList::goon() { os_timer_arm(&myTimer, 1, false); }

//_______delay ms (standard delay() does not work)______________
void BuzzerList::delay(unsigned long milli)
{
 unsigned long start=millis();
 while((millis()-start)<milli) yield();
}

// *************************************************************
//       help methods
// *************************************************************

//_______timer routine to play a tone___________________________
void BuzzerList::timer_ISR(void *pArg)
{
 os_timer_disarm(&myTimer);                 // stopp timer
 int d_=meld_.getCurrent();                 // get duration
 int f_=melf_.getCurrent();                 // get freqency
 bool ok=true;                              // this tone is OK
 ok&=meld_.next();                          // point to next d
 ok&=melf_.next();                          // point to next f
 if(!ok)                                    // no next tone
 {//-----no next tone-------------------------------------------
  if(melr_<repeat_ || repeat_==-1)
  {//----start melody again-------------------------------------
   if(repeat_!=-1) melr_++;                 // next repeat
   ok=true;                                 // this tone OK
   meld_.moveToStart();                     // reset d
   melf_.moveToStart();                     // reset f
   d_=meld_.getCurrent();                   // first duration
   f_=melf_.getCurrent();                   // first frequency
   ok&=meld_.next();                        // point to next d
   ok&=melf_.next();                        // point to next f
  }
 }
 if(d_<1) ok=false;                         // no duration
 if(!ok)
 {//-----number of repetitions reached--------------------------
  noTone(pinBuzzer_);                       // sound finished
  finished_=true;                            // playback finished
  return;                                   // finished
 }
 //------output tone--------------------------------------------
 if(f_<1) noTone(pinBuzzer_);               // silence
 else tone(pinBuzzer_,f_);                  // tone
 os_timer_arm(&myTimer, d_, false);         // start timer
}

//_____convert melody string to frequency and duration__________
// result in SimpleList melf_ and meld_
// return: true=successful translation
bool BuzzerList::parseMelody(String melody) {
 byte notes[] = { 9, 11, 0, 2, 4, 5, 7 };   // f-index of ABCDEFG

 byte curChar = 0;                          // current char
 byte curFunc = 0;                          // char as function
 unsigned int curVal = 0;                   // help value
 bool skip = true;                          // dont write df pair

 unsigned int curNoteLength = 0;            // current note length
 int  f_sharpFlat = 0;                      // no accidental (# b..)
 bool d_dotted = false;                     // note is not dotted
 int  f_octaveOffset = 0;                   // no offset

 int curNote = 0;                           // cur note = f-index
 int curNoteFreq;                            // cur note frequency
 double curNoteLengthMS = 0;               // cur note millisecs

 //----------------prepare parse process------------------------
 melf_.clear();
 meld_.clear();
 restoreSoundSettings();

 //----------------for all characters in melody-----------------
 for(unsigned int i=0; i < melody.length(); i++) {
  curNoteLength = 0;                        // no note length yet
  curVal = 0;                               // no value
  skip = true;                              // do not write df pair
  f_octaveOffset = 0;                       // no octave offset
    
  //---------------get character, skip blank--------------------
  curChar = melody[i];                      // get character
  if(curChar==' ') continue;                // skip space

  //---------------convert lowercase to uppercase---------------
  if (curChar>=97 && curChar<=122) curChar-=32;

  switch (curChar) {
   case '$'://-----restore default settings---------------------
    restoreSoundSettings();
    if(DEBUG_B) std::cout << "Restore settings" << std::endl;
   break;

   case '<'://-----f_octave lower---------------------------------
    f_octave--;
    if (f_octave < 1) f_octave = 1;
    if(DEBUG_B) std::cout << std::endl << "Command <  Octave set to " << (int)f_octave;
   break;

   case '>'://-----f_octave higher--------------------------------
    f_octave++;
    if (f_octave > 8) f_octave = 8;
    if(DEBUG_B) std::cout << std::endl << "Command >  Octave set to " << (int)f_octave; 
   break;

   case '['://------f_transpose down (move note down in pitch)-----
    f_transpose--;
    if (f_transpose < -12) f_transpose = -12;
    if(DEBUG_B) std::cout << std::endl << "Command [  Transpose down " << f_transpose << std::endl;
   break;

   case ']'://-----f_transpose up (move note up in pitch)---------
    f_transpose++;
    if (f_transpose > 12) f_transpose = 12;
    if(DEBUG_B) std::cout << std::endl << "Command ]  Transpose up " << f_transpose << std::endl;
   break;

   //--------------note, pause, # (pitch up), - (pitch down)----
   // A B C D E F G notes and R rest and # sharp and - flat
   case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'R': case '#': case '-':
    skip = false;
    if (i+1 < melody.length())              // is there a next char?
    { //...........check next char..............................
     if (isNum(melody[i + 1])) {
      skip = true;
     }
     if (melody[i + 1] == '#') {            // sharp
      f_sharpFlat = 1;
      skip = true;
     } 
     else
     {
      if (melody[i+1] == '-') {             // flat
       f_sharpFlat = -1;
       skip = true;
      } 
      else
      {
       if (melody[i+1] == '.') {            // dotted
        d_dotted = true;
        i++;
       }
      }
     }
    }
    if(curChar != '#' && curChar != '-') {
     curFunc = curChar;
    }
   break; // end case A B C D E F G R # - 

   case 'M'://-----articulation: music note length type---------
    if (i+1 < melody.length()) {            // is there a next char?
     switch  (melody[i + 1]) {              // yes: evaluate it
      case 'N': case 'n':                   // MN: music normal
       d_noteLengthType = 0;                // normal 7/8
       i++;                                 // goto next note
      break;
      case 'L': case 'l':                   // ML: music legato
       d_noteLengthType = 1;                // legato 1/1
       i++;                                 // goto next note
      break;
      case 'S': case 's':                   // MS: music staccato
       d_noteLengthType = 2;                // staccato 3/4
       i++;                                 // goto next note
      break;
      case 'U': case 'u':                   // MU: mute mode
       d_noteLengthType = 9;                // mute
       i++;                                 // goto next note
      break;
      default: break;                       // ignore Mx
     }
    }
   case 'L':                                // L default note/rest length
   case 'O':                                // O f_octave
   case 'T':                                // T tempo
    curFunc = curChar;                      // command T L or O
   break; // end M L O T

   default://------maybe number or dot--------------------------
           // but NOT  $<>[]ABCDEFGR#-MLOT
    if(isNum(curChar)) {
     curVal = curChar - 48;                 // first number
     //------------look ahead to get next 2 numbers or dot------
     for (int j = 0; j <= 2; j++) 
     {
      if (i+1 < melody.length())            // is there a next char?
      { //.........yes! check next char.........................
       if (isNum(melody[i + 1]))
       {//.........next char is number -> calc value............
        curVal = curVal * 10 + melody[i + 1] - 48;
        i++;                                // point to next char
       }
       else
       if (melody[i + 1] == '.') {
        d_dotted = true;
        break;                              // dot found!
       } else {
        break;                              // not a number
       }
      }
     } // next j
     curNoteLength=curVal;                  // given length
     skip = false;                          //
    } 
   break; // end default
  }
  //===============function or note=============================
  if (curFunc > 0 && !skip) 
  { //-----------..do note ABCDEFGR or function TOL-------------
   if(DEBUG_B) std::cout << std::endl<<"Command " << curFunc << "  value " << curVal << " | ";
   if ((curFunc >= 65 && curFunc <= 71) || curFunc == 82) 
   {//-------------note ABCDEFG or R (=ASCII 82)----------------
    //.............calculate note length........................
    if (!curNoteLength) curNoteLength = d_defaultNoteLength;
    if (d_dotted) curNoteLength = curNoteLength * 1.5;
    //.............convert note length to milliseconds..........
    curNoteLengthMS = noteLengthToMS(curNoteLength);

    //.............calculate frequency index....................
    if (curFunc == 82) 
    {//............82 = R = pause...............................
     curNote = 0;
     curNoteFreq=0;
     if(DEBUG_B) std::cout << " Pause length " << curNoteLength << " = " << curNoteLengthMS << " ms | ";
    }
    else
    {//............other note ABCDEFG: calc f index.............
     curNote = notes[curFunc - 65];         // frequency index
     curNote = curNote + f_transpose + f_sharpFlat;
     while (curNote < 0) {
      curNote += 12;
      if (f_octave > 1) f_octaveOffset -= 1;
     }
     while (curNote > 11) {
      curNote -= 12;
      if (f_octave < 8) f_octaveOffset += 1;
     }
     //............convert note to f-index......................
     if(DEBUG_B) std::cout<<" Transpose "<<f_transpose<<", Octave "<<(f_octave+f_octaveOffset)<<" | ";
     curNote = ((f_octave + f_octaveOffset) * 12) + curNote;
     curNoteFreq=noteToFrequency(curNote);
     if(DEBUG_B) std::cout<<" Notenumber "<<curNote<<", Frequency "<<curNoteFreq<<", length "<<curNoteLength<<" = "<<curNoteLengthMS<<" ms | ";
    }
    //-------------calculate tone duration and pause duration---
    switch (d_noteLengthType) {
     case 0: //....normal 7/8...................................
      melf_.append(curNoteFreq);
      meld_.append(7*curNoteLengthMS/8);
      melf_.append(0);
      meld_.append(curNoteLengthMS/8);
     break;
     case 1: //....legato 1/1...................................
      melf_.append(curNoteFreq);
      meld_.append(curNoteLengthMS);
     break;
     case 2: //....staccato 3/4.................................
      melf_.append(curNoteFreq);
      meld_.append(3*curNoteLengthMS/4);
      melf_.append(0);
      meld_.append(curNoteLengthMS/4);
     break;
     case 9: //....mute.........................................
      melf_.append(0);
      meld_.append(curNoteLengthMS);
     break;
     default:
     break;
    }
    d_dotted = false;                       // reset for next note
    curNoteLength = 0;                      // reset for next note
    f_sharpFlat = 0;                        // reset for next note
   } // end note ABCDEFG or R (=ASCII 82)
   else 
   {//-------------not a note => general command L O or T-------
    switch (curFunc) 
    { //-----------general command?-----------------------------
     case 'L'://...change note length (default 4)...............
      switch (curVal) {
       case 1: case 2: case 3: case 4: case 6: case 8: case 12: case 16: case 24: case 32: case 48: case 64:
       case 34: case 38: case 316: case 332:
        d_defaultNoteLength = curVal;
       break;
      }
      break;
      case 'O'://..change octave (default 4)....................
       if(curVal<1) f_octave=1;
       else {
        if(curVal>8) f_octave=8;
        else f_octave=curVal;
       }
      break;
      case 'T'://..change tempo (default 120 BPM)...............
      if(curVal<32) d_tempo=32;
      else {
       if(curVal>255) d_tempo=255;
       else d_tempo=curVal;
      }
      break;
    } // end switch general command
   } // end not a note => general command L O or T
   curFunc = 0;                             // reset for next note
  } // end do note ABCDEFGR or function TOL
 } // end for
 melf_.append(0);                           // end melody
 meld_.append(0);                           // end melody
 return true;
}

//_______is byte a number 0..9?_________________________________
// called by parseMelody()
 bool BuzzerList::isNum(unsigned char value) {
  if(value>='0' && value<='9') return true;
 return false;
}

//_______convert note length to milliseconds____________________
// unit of [d_tempo] is BPM = beats per minute (1min = 60000ms)
// noteLength is relative to BPM
// called by parseMelody()
double BuzzerList::noteLengthToMS(unsigned int noteLength) {
 double timeBase = 60000 / d_tempo;
 switch (noteLength) {
  case  1:  return (timeBase * 4);          // whole note
  case  2:  return (timeBase * 2);          // half
  case  4:  return (timeBase);              // quarter
  case  8:  return (timeBase / 2);          // 8th
  case 16:  return (timeBase / 4);          // 16th
  case 32:  return (timeBase / 8);          // 32nd
  case 64:  return (timeBase / 16);         // 64th

  case  3:  return (timeBase * 3);          // d_dotted half
  case  6:  return (timeBase * 3 / 2);      // d_dotted 4th
  case 12:  return (timeBase * 3 / 4);      // d_dotted 8th
  case 24:  return (timeBase * 3 / 8);      // d_dotted 16th
  case 48:  return (timeBase * 3 / 16);     // d_dotted 32th

  case 34:  return (timeBase / 1.5);        // triplet quarter
  case 38:  return (timeBase / 3);          // triplet 8th
  case 316: return (timeBase / 6);          // triplet 16th
  case 332: return (timeBase / 12);         // triplet 32th
 }
 return 0;
}

//_______convert note to frequency______________________________

unsigned int BuzzerList::noteToFrequency(int note) {
 unsigned int note2frequency[] = {
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, //  0- 11  undefined
    31,  35,  37,  39,  41,  44,  46,  49,  52,  55,  58,  62, // 12- 23  C1 C#1 D1 D#1 E1 F1 F#1 G1 G#1 A1 A#1 B1
    65,  69,  73,  78,  82,  87,  93,  98, 104, 110, 117, 123, // 24- 35  C2 to B2
   131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, // 36- 47  C3 to B3
   262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, // 48- 59  C4 to B4 (middle C)
   523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, // 60- 71  C5 to B5
  1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976, // 72- 83  C6 to B6
  2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951, // 84- 95  C7 to B7
  4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902  // 96-107  C8 to B8
 };
 if(note>=0 && note <=107) return note2frequency[note];
 return 0;
}

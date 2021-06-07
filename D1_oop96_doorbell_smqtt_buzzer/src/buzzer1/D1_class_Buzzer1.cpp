//_____D1_class_Buzzer1.cpp______________________khartinger_____
// The Buzzer1 class for the D1mini is used to output 
// tones and melodies.
// In order not to block the main loop during the sound output,
// it uses the timer interrupt for sound output.
// The software is based on a Arduino Melody Player by ericBcreator.
// https://www.hackster.io/ericBcreator/a-m-p-arduino-music-player-8013b6
//
// Hardware: 1. WeMos D1 Mini
//           2. Buzzer Shield (@ D5)
// Created by Karl Hartinger, June 06, 2021.
// Changes: -
// Released into the public domain.

#include "D1_class_Buzzer1.h"

// *************************************************************
//       constructor & co
// *************************************************************

//_______constructor____________________________________________
Buzzer1::Buzzer1() {
 setup(DEFAULT_BUZZER_PIN);
}

//_______constructor 2__________________________________________
Buzzer1::Buzzer1(int pin) {
 setup(pin);
}

//_______setup (called by constructor)__________________________
void Buzzer1::setup(int pin)
{
 pinBuzzer_=pin;                            // set buzzer pin
 pinMode(pinBuzzer_, OUTPUT);               // pin is output
 begin();                                   //init props,objects
}

//_______init objects___________________________________________
bool Buzzer1::begin()
{
 int ret=true;
 //------set properties-----------------------------------------
 pMelf_=NULL;                               // no frequency array
 pMeld_=NULL;                               // no duration array
 maxmem_=0;                                 // reserved mem
 icurf_=0;                                  // current index of f
 icurd_=0;                                  // current index of d
 imaxf_=0;                                  // max. index of f array
 imaxd_=0;                                  // max. index of d array
 numNotes_=0;                               // number of notes
 melody_="";                                // no melody notes
 melr_=1;                                   // first play
 repeat_=1;                                 // play melody 1x
 finished_=true;                            // not playing ;)
 os_timer_disarm(&myTimer);                 // stopp timer
 restoreSoundSettings();                    // init melody param
 return ret;
}

//_______reset melody parameter_________________________________
void Buzzer1::restoreSoundSettings()
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
bool Buzzer1::setMelody(String melody) {
 return parseMelody(melody);
}

//_______set a tone_____________________________________________
// repeat=-1: endless
// result in SimpleList melf_ and meld_
bool Buzzer1::setTone(int f, int msOn, int msOff) {
 if(f<1 || msOn<1 || msOff<1) return false;
 //------free old and reserve new memory------------------------
 maxmem_=2;                                // reserved new mem
 if(pMelf_!=NULL) free(pMelf_);            // delete old melody
 if(pMeld_!=NULL) free(pMeld_);            // delete old melody
 pMelf_=(int*) malloc(maxmem_*sizeof(int));// allocate memory...
 pMeld_=(int*) malloc(maxmem_*sizeof(int));// ...for new melody
 imaxf_=maxmem_-1;
 imaxd_=maxmem_-1;
 //------set frequency and duration values----------------------
 pMelf_[0]=f;                               // tone on
 pMeld_[0]=msOn;                            // tone on
 pMelf_[1]=0;                               // tone off
 pMeld_[1]=msOff;                           // tone off
 icurd_=0;                                  // start index
 icurf_=0;                                  // start index
 return true;
}

//_______Is the playback of the music finished?_________________
bool Buzzer1::finished() { return finished_; }

// *************************************************************
//       working methods
// *************************************************************

//_______set name of external interrupt routine_________________
void Buzzer1::setISR(void (*isr_)(void *pArg)) {
 os_timer_setfn(&myTimer, isr_, NULL);
}

//_______start playing the melody once__________________________
void Buzzer1::start() { start(1); }

//_______start playing the melody_______________________________
void Buzzer1::start(int repeat) { 
 melr_=1;                                   // current repeat #
 repeat_=repeat;                            // # od repeats
 icurd_=0;                                  // reset melody
 icurf_=0;                                  // reset melody
 finished_=false;                           // playing not fin.
 restoreSoundSettings();                    // old settings
 os_timer_arm(&myTimer, 1, false);          // start timer
}

//_______stopp playing the melody_______________________________
void Buzzer1::stopp() { 
 pause();                                   //stopp timer+noTone
 icurd_=0;                                  // reset melody
 icurf_=0;                                  // reset melody
 finished_=true;                            // stopp playing
}

//_______pause playing the melody_______________________________
void Buzzer1::pause() { 
 os_timer_disarm(&myTimer);                 // stopp timer
 noTone(pinBuzzer_);                        // sound finished
}

//_______start melody after pause_______________________________
void Buzzer1::goon() { os_timer_arm(&myTimer, 1, false); }

//_______delay ms (standard delay() does not work)______________
void Buzzer1::delay(unsigned long waitMillis)
{
 unsigned long start=millis();              // start millis
 unsigned long curMillis=millis();          // current millis
 while ((curMillis-start)<waitMillis) {     // timve over?
  curMillis=millis();                       // current millis
  yield();                                  // do something else
 };
}

// *************************************************************
//       help methods
// *************************************************************

//_______timer routine to play a tone___________________________
void Buzzer1::timer_ISR(void *pArg)
{
 os_timer_disarm(&myTimer);                 // stopp timer
 bool ok=true;                              // this tone is OK
 int d_=0;                                  // duration
 int f_=0;                                  // frequency
 if(pMeld_==NULL || pMelf_==NULL) ok=false;
 else 
 {
  if(icurd_<=imaxd_) d_=pMeld_[icurd_++];   // get duration
  else ok=false;                            // wrong index
  if(icurf_<=imaxf_) f_=pMelf_[icurf_++];   // get frequency
  else ok=false;                            // wrong index
  if(!ok)                                   // wrong index
  {//-----no next tone------------------------------------------
   if(melr_<repeat_ || repeat_==-1)
   {//----start melody again------------------------------------
    if(repeat_!=-1) melr_++;                // next repeat
    ok=true;                                // this tone OK
    icurd_=0;                               // set to 1st note
    icurf_=0;                               // set to 1st note
    if(icurd_<=imaxd_) d_=pMeld_[icurd_++]; // get duration
    else ok=false;                          // wrong index
    if(icurf_<=imaxf_) f_=pMelf_[icurf_++]; // get duration
    else ok=false;                          // wrong index
   }
  }
 }
 if(d_<1) ok=false;                         // no duration
 if(!ok)
 {//-----no melody or number of repetitions reached-------------
  noTone(pinBuzzer_);                       // sound finished
  finished_=true;                           // playback finished
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
bool Buzzer1::parseMelody(String melody) {
 byte notes[] = { 9, 11, 0, 2, 4, 5, 7 };   // f-index of ABCDEFG

 bool bRet=true;                            // return value
 byte curChar = 0;                          // current char
 byte curFunc = 0;                          // char as function
 unsigned int curVal = 0;                   // help value
 bool skip = true;                          // dont write df pair

 unsigned int curNoteLength = 0;            // current note length
 int  f_sharpFlat = 0;                      // no accidental (# b..)
 bool d_dotted = false;                     // note is not dotted
 int  f_octaveOffset = 0;                   // no offset

 int curNote = 0;                           // cur note = f-index
 int curNoteFreq;                           // cur note frequency
 double curNoteLengthMS = 0;                // cur note millisecs

 //----------------count number of notes in melody--------------
 numNotes_=countNotes(melody);
 if(numNotes_<1) return false;
 //----------------free old and reserve new memory--------------
 maxmem_=2*numNotes_;
 if(pMelf_!=NULL) free(pMelf_);            // delete old melody
 if(pMeld_!=NULL) free(pMeld_);            // delete old melody
 pMelf_=(int*) malloc(maxmem_*sizeof(int));// allocate memory...
 pMeld_=(int*) malloc(maxmem_*sizeof(int));// ...for new melody
 imaxf_=0;                                 // start with first..
 imaxd_=0;                                 // f and d index
 restoreSoundSettings();
 Serial.print("numNotes_="); Serial.println(numNotes_);

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
      if((imaxf_<1+maxmem_) && (imaxd_<1+maxmem_)) {
       pMelf_[imaxf_++]=curNoteFreq;
       pMelf_[imaxf_++]=0;
       pMeld_[imaxd_++]=7*curNoteLengthMS/8;
       pMeld_[imaxd_++]=  curNoteLengthMS/8;
      } else bRet=false;
     break;
     case 1: //....legato 1/1...................................
      if((imaxf_<maxmem_) && (imaxd_<maxmem_)) {
       pMelf_[imaxf_++]=curNoteFreq;
       pMeld_[imaxd_++]=curNoteLengthMS;
      } else bRet=false;
     break;
     case 2: //....staccato 3/4.................................
      if((imaxf_<1+maxmem_) && (imaxd_<1+maxmem_)) {
       pMelf_[imaxf_++]=curNoteFreq;
       pMelf_[imaxf_++]=0;
       pMeld_[imaxd_++]=3*curNoteLengthMS/4;
       pMeld_[imaxd_++]=  curNoteLengthMS/4;
      } else bRet=false;
     break;
     case 9: //....mute.........................................
      if((imaxf_<maxmem_) && (imaxd_<maxmem_)) {
       pMelf_[imaxf_++]=0;
       pMeld_[imaxd_++]=curNoteLengthMS;
      } else bRet=false;

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
 if(imaxd_>0 && imaxf_>0) 
 {//-----melody ok, correct max. index--------------------------
  imaxd_--;
  imaxf_--;
 } else
 {//-----should never be reached: no note -> free memory--------
  bRet=false;
  if(pMelf_!=NULL) free(pMelf_);           // delete old melody
  if(pMeld_!=NULL) free(pMeld_);           // delete old melody
  pMelf_=NULL;                             // no melody frequency
  pMeld_=NULL;                             // no melody duration
 }
 return bRet;
}

//_______is byte a number 0..9?_________________________________
// called by parseMelody()
 bool Buzzer1::isNum(unsigned char value) {
  if(value>='0' && value<='9') return true; // 0 to 9: ok
 return false;                              // not 0 to 9
}

//_______Count the number of ABCDEFGR and abcdefgr notes________
// in the string melody
int Buzzer1::countNotes(String melody)
{
 int num=0;                                 // number of notes
 char c=0;                                  // current note
 for(unsigned int i=0; i<melody.length(); i++) {
  c=melody[i];                              // get current note
  if(c>64 && c<72) num++;                   // A to G
  else {
   if(c==82) num++;                         // R
   else {
    if((c>96 && c<104) || c==114) num++;    // a to g or r
   }
  }
 }
 return num;
}

//_______convert note length to milliseconds____________________
// unit of [d_tempo] is BPM = beats per minute (1min = 60000ms)
// noteLength is relative to BPM
// called by parseMelody()
double Buzzer1::noteLengthToMS(unsigned int noteLength) {
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

unsigned int Buzzer1::noteToFrequency(int note) {
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

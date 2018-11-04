//_____D1_oop41_serial_outin_oled.ino______________20181104_____
// Send bytes 'A' to 'Z' continuous to Serial (19200 Bd) and
// show received byte(s) on oled.
// Blue LED blinks with 2Hz to demonstrate main loop is working.
// For test: connect pin TX to RX after (!) programming D1mini
// Hardware: (1) WeMos D1 mini
//           (2) OLED  Shield (64x48 pixel, I2C-addr 0x3C)
// Created by Karl Hartinger, November 04, 2018.
// Last Change: -
// Released into the public domain.

#include  "src/screen1/D1_class_Screen1.h"
#define   BUFFER_SIZE 80               // receive buffer size
uint8_t   charIn[BUFFER_SIZE];         // receive buffer
const int pinLED=D4;                   // blue led on board
#define   OLED_RESET 0                 // OLED_RESET=GPIO 0
Screen1   display_;                    // display object
char      charOut='A';                 // char A to Z
int       counterPos=0;                // count 0..59 (oled pos)

//_____draw character on oled___________________________________
void drawChar1(int cnt, char ch)
{
 int x=2+6*(cnt%10);                   // char x position [pixel]
 int y=8*int(cnt/10);                  // char y position [pixel]
 if((cnt%10)==0)                       // new line
 {
  int z=1+int(cnt/10);                 // line number
  display_.screen6(z, "");             // erase line content
  display_.screen6(++z, "");           // erase line+1 content
 }
 display_.setCursor(x,y);              // set cursor and...
 display_.print(ch);                   // ...print character
 display_.display();                   // show display content
}

//_____setup____________________________________________________
void setup() 
{
 pinMode(pinLED, OUTPUT);              // set led pin as output
 display_.screen6Clear(1,"");          // clear oled
 //-----start serial--------------------------------------------
 Serial.begin(19200); Serial.println();// start serial
 Serial.flush();                       // wait for empty out buf
 while(Serial.available())            
 {                                     // + empty
  Serial.read();                       // + incomming
  delay(1);                            // + buffer
 }
}

//_____loop_____________________________________________________
void loop() {
 //-----send char-----------------------------------------------
 Serial.write(charOut++);              // print char + increment
 if(charOut>'Z') charOut='A';          // if >Z: start with A
 //-----check for received character(s)-------------------------
 size_t len = Serial.available();      // byte(s) received?
 if(len)
 {
  //-----read available bytes-----------------------------------
  if(len>BUFFER_SIZE) len=BUFFER_SIZE;
  Serial.readBytes(charIn, len);
  //-----print char(s) on oled----------------------------------
  for(int i=0; i<len; i++) 
  {
   drawChar1(counterPos,charIn[i]);
   counterPos++;
   if(counterPos>59) counterPos=0;
  }
 }
 //-----blink led-----------------------------------------------
 digitalWrite(pinLED, digitalRead(pinLED)^1);    // invert LED
 delay(250);
}

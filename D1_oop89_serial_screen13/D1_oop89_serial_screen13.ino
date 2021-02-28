//_____D1_oop89_serial_screen13.ino________________20210228_____
// A text received via the serial interface is displayed on a
// 0.66" OLED display (baud rate 115200Bd) and - similar to the 
// display on the OLED - sent back via the serial interface.
// For the display `Screen13` from the class `Screen1` is used.
// This prefabricated screen has a double sized first line with
// 5 characters and three further lines for 10 characters each.
// In order for the text to be displayed in the correct line,
// the individual lines must be separated by a double underscore
// character (__). If a line text is longer than 10 characters,
//  the text continues in the next line.
// Furthermore, the built-in blue LED flashes at 1Hz to indicate
// that the D1 mini is running.
// Hardware: (1) WeMos D1 mini
//           (2) OLED  Shield (64x48 pixel, I2C-addr 0x3C)
// Created by Karl Hartinger, February 28 2021.
// Changes: 
// 2021-02-28 First release
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include  "src/screen1b/D1_class_Screen1b.h"

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define PIN_LED         2             // D4=IO2
#else
 #define PIN_LED         2             // D4=IO2
#endif
Screen1  display_;                     // display object
String   sRec;                         // received string
String   aLine[4];                     // max 4 lines
int      counter_10ms;                 // 10ms counter
uint32_t millisLast;                   // last start
uint32_t millisDelay=5;                // wait 5ms

//_______split string to array__________________________________
// uses aLine[] as result array
// return: number of elements in array aStr
int splitString(String str)
{
 String delimiter="__";                // separates the lines
 int imax=4;                           // max number of lines
 int anz=0;                            // real number of lines
 int len1=delimiter.length();          // length of separator
 if(len1<1) return anz;                // no delimiter/saparator
 if(str.length()<1) return anz;        // no input string
 int pos1=0,pos2=-1;                   // first start
 while((pos2=str.indexOf(delimiter,pos1))>=0) 
 {//-----delimiter found----------------------------------------
  aLine[anz++]=str.substring(pos1,pos2);// seperate line
  if(anz>=imax) return anz;            // too many lines
  pos1=pos2+len1;                      // start of next line
 }
 aLine[anz++]=str.substring(pos1);     // last line
 return anz;
}

//_____setup____________________________________________________
void setup() 
{
 //------start serial-------------------------------------------
 Serial.begin(115200);                 // start serial
 Serial.flush();                       // wait for empty out buf
 while(Serial.available())             //
 {                                     // + empty
  Serial.read();                       // + incomming
  delay(1);                            // + buffer
 }
 Serial.setTimeout(10000);             //wait max. 10s for lines
 Serial.println("\nProgram name: D1_oop89_serial_screen13");
 //------set pin mode-------------------------------------------
 pinMode(PIN_LED, OUTPUT);             // set led pin as output
 millisLast=millis();                  // start time
 //------display program name on OLED for min 1 sec-------------
 display_.begin();                     // start I2C
 display_.screen13Clear(1,"oop89",'c');// clear oled
 display_.screen13(2,"Serial to ");    // function of
 display_.screen13(3,"screen13  ");    // program
 display_.screen13(4,"2x_ nextln");    // info
 delay(1000);                          // wait 1s
 //------message for loop---------------------------------------
 Serial.println("\nWaiting for serial input...");
}

//_____loop_____________________________________________________
void loop() {
 //-----check for received character(s)-------------------------
 size_t len = Serial.available();           // byte(s) received?
 if(len)
 {
  //-----read String until character \n (line feed)-------------
  sRec=Serial.readStringUntil('\n');        // read line
  sRec.replace("\r","");                    // if eol = \r\n
  Serial.println(sRec);                     // send back line
  //-----split received string to lines-------------------------
  int numLines=splitString(sRec);           // get lines
  if(aLine[0].length()>5) aLine[0]=aLine[0].substring(0,5);
  //-----display lines on OLED and Serial-----------------------
  if(numLines>0)
  {
   //....display line 0 (double hight)..........................
   // clear screen, if more than 1 line
   Serial.println("  |----------");         // new screen begins
   if(numLines==1)                          // only 1 line: ...
    display_.screen13(1,aLine[0],'c');      // ...display line 1
   else                                     // clear oled and...
    display_.screen13Clear(1,aLine[0],'c'); // ...display line 1
   Serial.println("1 |"+aLine[0]);          // print line 1
   //....display other lines (2 to 4)...........................
   for(int i=1; i<numLines; i++) {          // display ...
    display_.screen13(i+1,aLine[i],'l');    // ... lines
    //...display line on Serial (more lines if linesX>10).......
    int linesX=(aLine[i].length()+9)/10;    // lines on Serial
    if(linesX>3) linesX=3;                  // max. 3 lines
    for(int j=0; j<linesX; j++) {           // print lines
     Serial.print(1+i+j);                   // line number
     Serial.print(" |");                    // vertical stroke
     Serial.println(aLine[i].substring(j*10,10*(j+1)));
    }
   }
  }
 }
 //------blink led----------------------------------------------
 if(counter_10ms%50==0)                     // every 500ms
  digitalWrite(PIN_LED, digitalRead(PIN_LED)^1); // invert LED
 counter_10ms++;                            // increment counter
 if(counter_10ms>99) counter_10ms=0;        // reset counter
 //------wait 10 ms---------------------------------------------
 while((millis()-millisLast) < millisDelay) {delay(1); yield();}
 millisLast=millis();
}

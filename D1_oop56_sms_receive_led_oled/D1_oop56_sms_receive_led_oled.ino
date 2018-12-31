//___D1_oop56_sms_receive_led_oled.ino________181230-181230_____
// D1 mini waits for a SMS with content #led on# or #led off#
// and turns blue led on or off.
// The command SMS is always stored on the first location in
// memory: for "SM" it is #1, for "ME" it is #21 (default)
// If an error  occurs while initializing SIM module, 
// the blue led blinks quickly, D1 mini waits for 2 seconds
// and tries again.
// SIM-Connection: Hardware Serial @ TX, RX or
//                 Software Serial @ D5 (=TX), D6 (=RX)
// DEBUG info    : OLED 0.66" 64x48 px
//
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
// While programming: disconnect hardware serial on TX,RX!
// After prog + hardware serial: connect TX,RX, reset D1 mini
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.

#define  VERSION56  "2018-12-30 D1_oop56_sms_receive_led_oled"
#define  FUNCTION56 "Waiting for SMS #led on# or #led off#"
#define  PIN_LED_BLUE           D4     // pin blue led @ D1 mini

#include "src/screen1a/D1_class_Screen1a.h"
//...use class GSMmodule0 (choose type of serial by #define)....
// see file D1_class_GSMmodule0.h, #define GSMMOD_SWS true|false
//#include "src/gsmmodule0/D1_class_GSMmodule0.h"
//GSMmodule0 mod1;                     // SIM module soft serial
//...use class GSMmodule (choose type of serial by constructor).
#include "src/gsmmodule/D1_class_GSMmodule.h"
GSMmodule mod1;                        // SIM module soft serial
//GSMmodule mod1(false);               // false=Hardware serial

Screen1  display_;                     // OLED 0.66"

//_____print a string to OLED___________________________________
void displayScroll(String s1)
{
 int imax=int(s1.length()/10);
 display_.screen6(7,s1);
 for(int i=0; i<imax; i++){display_.screen6(7,""); delay(100);}
}

//_____print SMS to OLED________________________________________
void printSMS(SMS sms1)
{
 display_.screen6(7,"Numb "+String(sms1.number));
 delay(100);
 displayScroll("Phon "+sms1.phone);
 delay(100);
 displayScroll(sms1.date);
 delay(100);
 displayScroll(sms1.text);
 delay(100);
/* 
 Serial.println("Number: "+String(sms1.number)+" |");
 Serial.println("Phone : "+sms1.phone+" | len="+sms1.phone.length());
 Serial.println("Date  : "+sms1.date +" | len="+sms1.date.length());
 Serial.println("Text  : "+sms1.text +" | len="+sms1.text.length());
*/
}

//_____check if SMS is a command________________________________
String doSMScommand(String sCmd) {
 //Serial.println("doSMScommand |"+sCmd+"|");
 String sRet="No command";
 if(sCmd.indexOf("#led on#")>=0) {
  digitalWrite(PIN_LED_BLUE,0);        // led on
  sRet="LED on";
 }
 if(sCmd.indexOf("#led off#")>=0) {
  digitalWrite(PIN_LED_BLUE,1);        // led off
  sRet="LED off";
 }
 return sRet;
}

//_____SETUP____________________________________________________
void setup() {
 //-----setup led-----------------------------------------------
 pinMode(PIN_LED_BLUE, OUTPUT);        // pin blue led = output
 digitalWrite(PIN_LED_BLUE,1);         // blue led off
 //-----setup hardware serial (baudrate, clear buffer)----------
 //Serial.begin(9600);                 // init Serial
 //Serial.println("\r\n"+String(VERSION55));
 //-----init sim module-----------------------------------------
 display_.screen6iClear(4,"Init modem");
 display_.screen6(5,mod1.getsResult());
 int i=0;                              // counter while ERROR
 while(mod1.begin()!=GSMMOD_OK) {
  display_.screen6iClear(1,"ModemERROR");
  display_.screen6(2,mod1.getsStatus());
  display_.screen6(6,String(++i),'c');
  for(int j=0; j<10; j++) {
   digitalWrite(PIN_LED_BLUE,0); delay(100); // blue led on
   digitalWrite(PIN_LED_BLUE,1); delay(100); // blue led off
  }
 }
 display_.screen6(6,"");               // delete counter value
 display_.screen6i(7,"Init Modem");    //
 display_.screen6(7,"Modem OK!",'c');
 //-----prepare location for command sms (= 1. location in mem)-
 int status=mod1.deleteSMS1();         //delete 1. message
 if(status==GSMMOD_OK) display_.screen6(7,"Delete1 OK");
 else display_.screen6(7,mod1.getsStatus());
 display_.screen6(7,"Wait-4-SMS");
}

//_____LOOP_____________________________________________________
void loop() {
 if(mod1.isSMS1())
 {
  display_.screen6(7,"SMS receiv");    // info: SMS here
  SMS smsRec=mod1.readSMS1();          // read SMS
  //printSMS(smsRec);
  String s2=smsRec.text;               // get SMS text
  s2=doSMScommand(s2);                 // analyse cmd
  display_.screen6i(7,s2);             // display result
  mod1.deleteSMS1();                   // prepare for next SMS
 }
 delay(200);                           // wait a little bit
}

//_____D1_oop52_sms_receive_led_serial.ino____181230-181230_____
// D1 mini waits for a SMS with content #led on# or #led off#
// and turns blue led on or off.
// The command SMS is always stored on the first location in
// memory: for "SM" it is #1, for "ME" it is #21
// If an error  occurs while initializing SIM module, 
// the blue led blinks quickly, D1 mini waits for 2 seconds
// and tries again.
// SIM-Connection  : Software Serial @ D5 (=TX), D6 (=RX)
// DEBUG info to PC: Hardware Serial @ TX, RX (9600Bd)
//
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.

#define  VERSION52  "2018-12-30 D1_oop52_sms_receive_led_serial"
#define  FUNCTION52 "Wait for SMS #led on# or #led off#"
#define  PIN_LED_BLUE           D4     // pin blue led @ D1 mini

#include "src/gsmmodule/D1_class_GSMmodule.h"
GSMmodule mod1;                        // SIM module

//_____print SMS to (hardware) Serial___________________________
void printSMS(SMS sms1)
{
 Serial.println("Number: "+String(sms1.number)+" |");
 Serial.println("Phone : "+sms1.phone+" | len="+sms1.phone.length());
 Serial.println("Date  : "+sms1.date +" | len="+sms1.date.length());
 Serial.println("Text  : "+sms1.text +" | len="+sms1.text.length());
}

//_____check if SMS is a command________________________________
String doSMScommand(String sCmd) {
 //Serial.println("doSMScommand() received |"+sCmd+"|");
 String sRet="Received: "+sCmd;
 if(sCmd.indexOf("#led on#")>=0) {
  digitalWrite(PIN_LED_BLUE,0);        // led on
  sRet="*** LED on  ***";
 }
 if(sCmd.indexOf("#led off#")>=0) {
  digitalWrite(PIN_LED_BLUE,1);        // led off
  sRet="*** LED off ***";
 }
 return sRet;
}

//_____SETUP____________________________________________________
void setup() {
 //-----setup led-----------------------------------------------
 pinMode(PIN_LED_BLUE, OUTPUT);        // pin blue led = output
 digitalWrite(PIN_LED_BLUE,1);         // blue led off
 //-----setup hardware serial (baudrate, clear buffer)----------
 Serial.begin(9600);                   // init Serial
 Serial.println("\r\n"+String(VERSION52)+"\r\n"+String(FUNCTION52));
 //-----check sim module----------------------------------------
 Serial.println("-----Init Modem---"); // show OK message
 int i=0;                              // counter while ERROR
 while(mod1.begin()!=GSMMOD_OK) {      // is module ready?
  Serial.print((++i));                 // print counter & error
  Serial.println(": "+mod1.getsCommand()+" -> "+mod1.getsResult());
  for(int j=0; j<10; j++) {
   digitalWrite(PIN_LED_BLUE,0); delay(100); // blue led on
   digitalWrite(PIN_LED_BLUE,1); delay(100); // blue led off
  }
 }; 
 Serial.println("-----Modem OK-----");      // show OK message
 //-----prepare location for command sms (= 1. location in mem)-
 Serial.print("Delete location for command sms: ");
 mod1.deleteSMS1();                         //delete 1. message
 Serial.println(mod1.getsResult(false));    // false: ~ \n # \r
 Serial.println("-----Ready--------");      // modem ready
}

//_____LOOP_____________________________________________________
void loop() {
 if(mod1.isSMS1())
 {
  Serial.println("SMS received!");     // info: SMS here
  SMS smsRec=mod1.readSMS1();          // read SMS
  printSMS(smsRec);                    // SMS data to Serial
  String s2=smsRec.text;               // get SMS text
  s2=doSMScommand(s2);                 // analyse cmd
  Serial.println(s2);                  // display result
  mod1.deleteSMS1();                   // prepare for next SMS
 }
 delay(200);                           // wait a little bit
}

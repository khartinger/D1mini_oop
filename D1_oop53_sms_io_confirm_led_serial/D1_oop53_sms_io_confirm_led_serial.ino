//_____D1_oop53_sms_io_confirm_led_serial.ino_181230-181230_____
// D1 mini waits for a SMS with content #led on# or #led off#
// and turns blue led on or off. Afterwards a SMS is sent back
// to confirm the command/SMS.
// The command SMS is always stored on the first location in
// memory: for "SM" it is #1, for "ME" it is #21 (default).
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

#define  VERSION53  "2018-12-30 D1_oop53_sms_io_confirm_led_serial.ino"
#define  FUNCTION53 "Wait for SMS #led on# or #led off#, send answer SMS"
#define  PIN_LED_BLUE           D4     // pin blue led @ D1 mini

#include "src/gsmmodule/D1_class_GSMmodule.h"
GSMmodule mod1;                        // SIM module

//_____check if SMS is a command________________________________
String doSMScommand(String sCmd) {
 //Serial.println("doSMScommand() received |"+sCmd+"|");
 String sRet="Received: "+sCmd;
 if(sCmd.indexOf("#led on#")>=0) {
  digitalWrite(PIN_LED_BLUE,0);        // led on
  sRet="*** LED is on  ***";
 }
 if(sCmd.indexOf("#led off#")>=0) {
  digitalWrite(PIN_LED_BLUE,1);        // led off
  sRet="*** LED is off ***";
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
 Serial.println("\r\n"+String(VERSION53)+"\r\n"+String(FUNCTION53));
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
  SMS sms1=mod1.readSMS1();            // read SMS
  String s2=sms1.text;                 // get SMS text
  s2=doSMScommand(s2);                 // analyse cmd
  Serial.println(s2);                  // display result
  sms1.text=s2;                        // prepair answer SMS 
  mod1.sendSMS(sms1);                  // send answer to sender
  Serial.println(mod1.getsResult());   // result of SMS sending
  mod1.deleteSMS1();                   // prepair for next SMS
}
 delay(200);                           // wait a little bit
}

//_____D1_oop51_sms_send_D3_serial.ino________181230-181230_____
// When button D3 is pressed a test sms is sent to phone
// number TEL_NUMBER1 using SIM800C GSM GPRS module STM32.
// During sending the SMS, the blue LED is turned on.
// If an error  occurs while initializing SIM module, 
// the blue led blinks quickly, D1 mini waits for 2 seconds
// and tries again.
// SIM-Connection   : Software Serial @ D5 (=TX), D6 (=RX)
// Status info to PC: Hardware Serial @ TX, RX
//
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
//           (4) 1-Button Shield D3
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.

#define  TEL_NUMBER1 "0680xxxxxxx"  // CHANGE TO YOUR OWN #!!
#define  VERSION51   "2018-12-30 D1_oop51_sms_send_D3_serial"
#define  FUNCTION51  "Press D3 to send a SMS"
#define  PIN_BUTTON             D3     // D3 is default button
#define  PIN_LED_BLUE           D4     // pin blue led @ D1 mini

#include "src/gsmmodule/D1_class_GSMmodule.h"
GSMmodule mod1;                        // SIM module
int       button3;                     // button value
int       buttonOld;                   // old value of button

//_____SETUP____________________________________________________
void setup() {
 //-----setup led-----------------------------------------------
 pinMode(PIN_BUTTON, INPUT);           // pin button = input
 pinMode(PIN_LED_BLUE, OUTPUT);        // pin blue led = output
 digitalWrite(PIN_LED_BLUE,1);         // blue led off
 buttonOld=digitalRead(PIN_BUTTON);    // button start value
 //-----setup hardware serial (baudrate, clear buffer)----------
 Serial.begin(9600);                   // init Serial
 Serial.println("\r\n"+String(VERSION51)+"\r\n"+String(FUNCTION51));
 //-----is a sim module connected?------------------------------
 Serial.println(mod1.getsResult());    // hard-/software serial
 Serial.print("isModule() | Search for GSM Module...");
 int i=0;                              // counter
 while(!mod1.isModule()) {             // is module present?
  Serial.print((++i));                 // increment counter
  Serial.println(": GSM module NOT found.");// error message
  for(int j=0; j<10; j++) {
   digitalWrite(PIN_LED_BLUE,0); delay(100); // blue led on
   digitalWrite(PIN_LED_BLUE,1); delay(100); // blue led off
  }
 };
 Serial.println("found.");             // Module found
 //-----init sim module-----------------------------------------
 Serial.print("begin()    | Init module: ");
 while(mod1.begin()!=GSMMOD_OK) {      // is module ready?
  Serial.print((++i));                 // print counter & error
  Serial.println(": "+mod1.getsCommand()+" -> "+mod1.getsResult());
  for(int j=0; j<10; j++) {
   digitalWrite(PIN_LED_BLUE,0); delay(100); // blue led on
   digitalWrite(PIN_LED_BLUE,1); delay(100); // blue led off
  }
 }; 
 Serial.println("Module OK!");         // message module OK
 Serial.println("Waiting for button D3 to be pressed...");
}

//_____LOOP_____________________________________________________
void loop() {
 String sRet;                          // command, return string
 int buttonX=digitalRead(PIN_BUTTON);  // 1. read button value
 delay(25);                            // wait button bouncing
 int buttonY=digitalRead(PIN_BUTTON);  // 2. read button value
 if(buttonX==buttonY) button3=buttonX; // no pulse spike
 if((buttonOld==1)&&(button3==0))      // falling edge
 {
  digitalWrite(PIN_LED_BLUE,0);        // blue led on
  Serial.println("-----Button pressed!-----");
  String s1="\r\nUp-Time: "+String(millis())+"ms ";
  if(mod1.isSoftwareSerial()) s1+="Software Serial";
                         else s1+="Hardware Serial";
  SMS sms1;                            // SMS object
  sms1.phone=String(TEL_NUMBER1);      //
  sms1.text=String(VERSION51)+s1;      //
  mod1.sendSMS(sms1);                  // send SMS
  Serial.println("SMS-Text: "+sms1.text);
  Serial.print("SMS-Result: "+mod1.getsResult());
  Serial.println("-----Sent finished-----");
  digitalWrite(PIN_LED_BLUE,1);       // blue led off
 }
 buttonOld=button3;                    // remember old value
 delay(100);                           // wait a little bit
}

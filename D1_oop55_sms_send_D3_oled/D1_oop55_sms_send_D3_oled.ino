//_____D1_oop55_sms_send_D3_oled.ino__________181230-181230_____
// When button D3 is pressed a test sms is sent to phone
// number TEL_NUMBER1 using SIM800C GSM GPRS module STM32.
// During sending the SMS, the blue LED is turned on.
// If an error  occurs while initializing SIM module, 
// the blue led blinks quickly, D1 mini waits for 2 seconds
// and tries again.
// SIM-Connection: Hardware Serial @ TX, RX OR
//                 Software Serial @ D5 (=TX), D6 (=RX)
// Status info   : OLED shield 0.66" 64x48px (Screen6)
//
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
//           (4) 2-button OLED shield (D3)
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.

#define  TEL_NUMBER1 "0680xxxxxxx"  // CHANGE TO YOUR OWN #!!
#define  VERSION55   "2018-12-30 D1_oop55_sms_send_D3_oled"
#define  FUNCTION55  "Press D3 to send a SMS"
#define  PIN_BUTTON             D3     // D3 is default button
#define  PIN_LED_BLUE           D4     // pin blue led @ D1 mini
#define  OLED_RESET            RST     // D3=GPIO0, D0=GPIO16

#include "src/screen1a/D1_class_Screen1a.h"

//...use class GSMmodule0 (choose type of serial by #define)....
// see file D1_class_GSMmodule0.h, #define GSMMOD_SWS true|false
//#include "src/gsmmodule0/D1_class_GSMmodule0.h"
//GSMmodule0 mod1;                     // SIM module soft serial
//...use class GSMmodule (choose type of serial by constructor).
#include "src/gsmmodule/D1_class_GSMmodule.h"
GSMmodule mod1;                        // SIM module soft serial
//GSMmodule mod1(false);               // false=Hardware serial

int       button3;                     // button value
int       buttonOld;                   // old value of button 
Screen1   display_;                    // OLED 0.66"

//_____print a string to OLED___________________________________
void displayScroll(String s1)
{
 int imax=int(s1.length()/10);
 display_.screen6(7,s1);
 for(int i=0; i<imax; i++){display_.screen6(7,""); delay(100);}
}

//_____SETUP____________________________________________________
void setup() {
 //-----setup led-----------------------------------------------
 pinMode(PIN_BUTTON, INPUT);           // pin button = input
 pinMode(PIN_LED_BLUE, OUTPUT);        // pin blue led = output
 digitalWrite(PIN_LED_BLUE,1);         // blue led off
 buttonOld=digitalRead(PIN_BUTTON);    // button start value
 //-----setup hardware serial (baudrate, clear buffer)----------
 //Serial.begin(9600);                   // init Serial
 //Serial.println("\r\n"+String(VERSION54));
 //-----is a sim module connected?------------------------------
 String s1=mod1.getsResult();          // hard-/software serial
 display_.screen6iClear(1,"Is Module?"); // text module present?
 display_.screen6(2,s1);               // type serial
 display_.screen6(4,"Module",'c');     // text
 int i=0;                              // counter
 while(!mod1.isModule()) {             // module present?
  display_.screen6(5,"NOT found!");    // text
  display_.screen6(6,String(++i),'c'); // counter
  for(int j=0; j<10; j++) {
   digitalWrite(PIN_LED_BLUE,0); delay(100); // blue led on
   digitalWrite(PIN_LED_BLUE,1); delay(100); // blue led off
  }
 }
 display_.screen6(5,"found!",'c');     // text modem found
 delay(2000);                          // time to read display
 //-----init sim module-----------------------------------------
 i=0;
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
 display_.screen6(7,"Press D3..");
}

//_____LOOP_____________________________________________________
void loop() {
 int buttonX=digitalRead(PIN_BUTTON);  // 1. read button value
 delay(25);                            // wait button bouncing
 int buttonY=digitalRead(PIN_BUTTON);  // 2. read button value
 if(buttonX==buttonY) button3=buttonX; // no pulse spike
 if((buttonOld==1)&&(button3==0))      // falling edge
 {
  digitalWrite(PIN_LED_BLUE,0);        // blue led on
  display_.screen6i(7,"D3 pressed");
  String s1="\r\nUp-Time: "+String(millis())+"ms ";
  if(mod1.isSoftwareSerial()) s1+="Software Serial";
                         else s1+="Hardware Serial";
  SMS sms1;                            // SMS object
  sms1.phone=String(TEL_NUMBER1);      //
  sms1.text=String(VERSION55)+s1;      //
  mod1.sendSMS(sms1);                  // send SMS
  displayScroll(mod1.getsStatus());    // display result
  digitalWrite(PIN_LED_BLUE,1);        // blue led off
 }
 buttonOld=button3;                    // remember old value
 delay(100);                           // wait a little bit
}

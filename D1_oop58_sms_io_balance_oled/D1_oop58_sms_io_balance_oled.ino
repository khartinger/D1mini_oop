//_____D1_oop58_sms_io_balance_oled.ino_______181230-190113_____
// Demo class GSMmodule: Send and receive SMS.
// Program asks for the prepaid balance and displays all program
// steps and SIM prepaid balance on OLED.
// SIM-Connection: Hardware Serial @ TX, RX OR
//                 Software Serial @ D5 (=TX), D6 (=RX)
// Status info   : OLED shield 0.66" 64x48px (Screen6)
//
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
//           (4) OLED shield 0.66"
// Created by Karl Hartinger, December 30, 2018.
// Last modified: 190113...line 82 screen6iClear, 114 mod1.begin()
// Released into the public domain.

#define  VERSION58  "2019-01-13 D1_oop58_sms_io_balance_oled"
#define  FUNCTION58 "Display prepaid SIM balance on oled."
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

Screen1   display_;                    // OLED 0.66"
String    sNetwork;                    // provider name

//_____Display balance on OLED__________________________________
void displayBalance(String network1, String text1, String date1)
{
 String oledDate="??.??.????";
 String oledTime="??:??:??";
 String oledBalance="?.??";
 yield();
 //-----prepair SMS date and time for OLED----------------------
 if(date1.length()>=17) {
  oledDate=date1.substring(6,8)+"."+date1.substring(3,5)+".20"+date1.substring(0,2);
  oledTime=date1.substring(9,17);      // extract time
 }
 oledBalance=mod1.extractBalance(text1);
 //-----display values on OLED----------------------------------
 display_.screen6i(1,network1,'c');    // display head
 display_.screen6(2,"");               // empty line
 display_.screen6(3,oledBalance+" EUR",'c'); // display balance
 display_.screen6(4,"");               // empty line
 display_.screen6(5,oledDate);         // display date dd.mm.yyyy
 display_.screen6(6,oledTime,'c');     // display time hh:MM:ss
}

//_____SETUP____________________________________________________
void setup() {
 String s1;
 //-----setup hardware serial (baudrate, clear buffer)----------
 //Serial.begin(9600);                   // init Serial
 //Serial.println("\n"+String(VERSION58)+"\n"+String(FUNCTION58));
 //-----check gsm module----------------------------------------
 display_.screen6iClear(1,"Init Modem");
 display_.screen6(3,"GSM module");
 int i=0;
 while(!mod1.isModule())
 {
  display_.screen6(4,"NOT found!");
  display_.screen6(6,String(++i),'c');
  delay(2000); 
 };
 display_.screen6(4,"found!",'c');
 while(mod1.begin()!=GSMMOD_OK)
 {
  display_.screen6iClear(1,"ModemERROR");
  display_.screen6(2,mod1.getsStatus());
  display_.screen6(6,String(++i),'c');
  delay(2000);
 }; 
 //-----Module OK -> network info-------------------------------
 sNetwork=mod1.getsNetwork();
 display_.screen6iClear(1,sNetwork);
 display_.screen6(6,"Module OK!");
 delay(4000);
 //-----change network manually (if necessary)------------------
 String s3=mod1.readSimId();
 if(s3.indexOf("8943102101")>=0) {
  mod1.sendNetwork(String(GSMMOD_NET_ID3));
  sNetwork=mod1.getsNetwork();
  display_.screen6i(1,sNetwork);
 }
 //-----END change network manually-----------------------------
 //-----Info about SMS memory-----------------------------------
 display_.screen6(2,"");
 s1=mod1.getsMem();
 display_.screen6(3,"Memory: "+s1);
 s1=String(mod1.getNumOfSms1());
 if(s1.length()==1) s1=" "+s1;
 display_.screen6(4,"SMS1=# "+s1);
 s1=String(mod1.readNumSmsInMem());
 if(s1.length()==1) s1=" "+s1;
 display_.screen6(5,"SMSsto:"+s1);
 s1=String(mod1.readMaxSmsInMem());
 if(s1.length()==1) s1=" "+s1;
 display_.screen6(6,"SMSmax:"+s1);
 delay(6000);
 //-----Request balance SMS (might take up to 30sec)------------
 i=0;
 display_.screen6iClear(1,sNetwork);
 display_.screen6(2,"Request",'c');
 display_.screen6(3,"balance",'c');
 display_.screen6(4,"SMS (<30s)");
 while(mod1.requestBalanceSMS()!=GSMMOD_OK) {
  mod1.begin();
  display_.screen6(6,String(++i),'c');
  delay(2000);
 }
 //-----Waiting for SMS with balance info...--------------------
 display_.screen6iClear(1,sNetwork);
 display_.screen6(2,"Waiting 4",'c');
 display_.screen6(3,"balance",'c');
 display_.screen6(4,"info SMS",'c');
 delay(1000);
 i=101;
 bool ok=false;
 while((i>0)&&(!ok)) {
  display_.screen6(6,String(--i),'c');
  delay(1000);
  if(mod1.isSMS1()) ok=true;
 }
 if(ok) {
  SMS sms1=mod1.readSMS1();
  displayBalance(sNetwork, sms1.text, sms1.date);
 } else
 {
  display_.screen6(2,"-- NO --",'c');
  display_.screen6(3,"balance",'c');
  display_.screen6(4,"info",'c');
 }
}

int i1=0;

//_____LOOP_____________________________________________________
void loop() {
 delay(5000);
 display_.screen6(6,String(++i1),'c');
 int status=mod1.begin();
 if(status!=GSMMOD_OK) {
  display_.screen6iClear(1,sNetwork);
  display_.screen6(3,mod1.getsStatus());
 } else {
  SMS sms3=mod1.readSMS1();
  if(sms3.text!="")
  {
   displayBalance(sNetwork, sms3.text, sms3.date);
   //mod3.deleteSMS1();                // delete 1. sms location
  }
 }
 delay(10000);
}

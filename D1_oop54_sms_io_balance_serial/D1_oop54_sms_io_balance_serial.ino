//_____D1_oop54_sms_io_balance_serial.ino_____181230-181230_____
// Demo class GSMmodule: Send and receive SMS.
// Program asks for the prepaid balance and 
// displays all program steps on Serial.
// SIM-Connection: Software serial @ D5 (=TX), D6 (=RX)
// Display info  : hardware serial (9600Bd) @ TX, RX
//
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.

#define  VERSION54  "2018-12-30 D1_oop54_sms_io_balance_serial"
#define  FUNCTION54 "Display prepaid SIM balance on Serial."

#include "src/gsmmodule/D1_class_GSMmodule.h"
GSMmodule mod1;                        // SIM module
String    sNetwork;                    // provider name

//_____Display balance (credit)_________________________________
void displayBalance(String text1, String date1)
{
 String oledDate="??.??.????";
 String oledTime="??:??:??";
 String oledBalance="?.??";
 //-----prepair SMS date and time for OLED----------------------
 if(date1.length()>=17) {
  oledDate=date1.substring(6,8)+"."+date1.substring(3,5)+".20"+date1.substring(0,2);
  oledTime=date1.substring(9,17);      // extract time
 }
 oledBalance=mod1.extractBalance(text1);
 Serial.print(oledBalance+" EUR");     // balance value String
 Serial.print(", "+oledDate);          // display date dd.mm.yyyy
 Serial.println(" "+oledTime);         // display time hh:MM:ss
}

//_____SETUP____________________________________________________
void setup() {
 String   s1;                           // help string
 //-----setup hardware serial (baudrate, clear buffer)----------
 Serial.begin(9600);                   // init Serial
 Serial.println("\n"+String(VERSION54)+"\n"+String(FUNCTION54));
 //-----check gsm module----------------------------------------
 Serial.print("isModule()        | Search for GSM Module...");
 int i=0;                              // counter
 while(!mod1.isModule()) {             // is module present?
  Serial.print((++i));                 // increment counter
  Serial.println(": GSM module NOT found.");// error message
  delay(800);                          // wait for module
 };
 Serial.println("found.");             // Module found
 Serial.print("begin()           | Init module: ");
 while(mod1.begin()!=GSMMOD_OK) {      // is module ready?
  Serial.print((++i));                 // print counter & error
  Serial.println(": "+mod1.getsCommand()+" -> "+mod1.getsResult());
  delay(1800);                         // wait for module ready
 }; 
 Serial.println("Module OK!");         // message module OK
 //-----Module OK -> Network info-------------------------------
 sNetwork=mod1.getsNetwork();          // registered in network?
 Serial.println("getsNetwork()     | Registered Network: "+sNetwork);
 //-----change network manually (if necessary)------------------
 String s3=mod1.readSimId();
 Serial.println("readSimId()       | SIM-ID: "+s3);
 if(s3.indexOf("8943102101")>=0) {
  s3=String(GSMMOD_NET_ID3);
  Serial.println("Try to change network to "+s3);
  mod1.sendNetwork(s3);
  sNetwork=mod1.getsNetwork();
  Serial.println("2. getsNetwork()   | Registered Network: "+sNetwork);
 }
 //-----END change network manually-----------------------------
 //-----Info about SMS memory-----------------------------------
 s1=mod1.getsMem();
 Serial.println("getsMem()         | Used memory: "+s1);
 s1=String( mod1.getNumOfSms1() );
 Serial.println("getNumOfSms1()    | Location of first SMS: "+s1);
 s1=String( mod1.readNumSmsInMem() );
 Serial.println("readNumSmsInMem() | Number of stored SMS : "+s1);
 s1=String( mod1.readMaxSmsInMem() );
 Serial.println("readMaxSmsInMem() | Maximum SMS in memory: "+s1);
 //-----request balance SMS-------------------------------------
 Serial.println("--Request balance (might take up to 30sec)--");
 i=0;
 while(mod1.requestBalanceSMS()!=GSMMOD_OK) {
  i++; Serial.print(i); 
  Serial.println(" "+mod1.getsResult()+" "+mod1.getsStatus());
  delay(2000);
 }
 //-----Waiting for SMS with balance info...--------------------
 Serial.println("Waiting for SMS with balance info...");
 i=101;
 bool ok=false;
 while((i>0)&&(!ok)) {
  i--;
  Serial.print(i);Serial.println(" waiting for balance info...");
  delay(1000);
  if(mod1.isSMS1()) ok=true;
 }
 Serial.print(sNetwork+": ");
 if(ok) {
  SMS sms1=mod1.readSMS1();
  displayBalance(sms1.text, sms1.date);
 }
 else Serial.println(" No balance info.");
}

//_____LOOP_____________________________________________________
void loop() {
 delay(5000);
 int status=mod1.begin();
 if(status!=GSMMOD_OK) 
  Serial.println(mod1.getsStatus());
 else {
  SMS sms3=mod1.readSMS1();
  if(sms3.text!="")
  {
   Serial.println("=====Display balance========================");
   Serial.print(sNetwork+": "); displayBalance(sms3.text, sms3.date);
   //mod3.deleteSMS1();               // delete sms on 1. location
  }
  Serial.println("-----List of all SMS------------------------");
  Serial.println(mod1.listAllSMS());
  Serial.println("-----END------------------------------------");
 }
 delay(10000);
}

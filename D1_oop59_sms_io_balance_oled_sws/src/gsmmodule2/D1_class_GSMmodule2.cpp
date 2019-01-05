//_____D1_class_GSMmodule2.cpp________________181230-181230_____
// D1 mini class for sending and receiving SMS with a
// SIM800C GSM GPRS module STM32
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.
#include "D1_class_GSMmodule2.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____default constructor______________________________________
GSMmodule2::GSMmodule2() { setup(true,-1,-1,GSMMOD2_SMS_MEM); }

//_____constructor: set software/hardware serial________________
GSMmodule2::GSMmodule2(bool swSerial) 
{ setup(swSerial, -1,-1,GSMMOD2_SMS_MEM); }

//_____constructor: set software/hardware serial and memory_____
GSMmodule2::GSMmodule2(bool swSerial, String mem) 
{ setup(swSerial, -1,-1,mem); }
  
//_____constructor: default pins, specified memory for sms______
GSMmodule2::GSMmodule2(String mem) { setup(true, -1,-1, mem); }
 
//_____constructor: specified pins, default memory for sms______
GSMmodule2::GSMmodule2(int pinRx, int pinTx) 
{ setup(true, pinRx, pinTx, GSMMOD2_SMS_MEM); } 
 
//_____constructor: specified pins, memory for sms______________
GSMmodule2::GSMmodule2(int pinRx, int pinTx, String mem)
{ setup(true, pinRx, pinTx, mem); }

//_____destructor_______________________________________________
GSMmodule2::~GSMmodule2() {
 if(SWSerial_) delete(seri);
};

//_____setup GSMmodule2__________________________________________
void GSMmodule2::setup() { setup(true,-1,-1,GSMMOD2_SMS_MEM); }

//_____setup GSMmodule2__________________________________________
void GSMmodule2::setup(bool swSerial,int pinRx, int pinTx, String mem)
{
 SWSerial_=swSerial;
 status=GSMMOD2_OK;
 //-----set software serial pins and memory---------------------
 if(pinRx>=0) pinRx_=pinRx; else pinRx_=GSMMOD2_PIN_SWS_RX;
 if(pinTx>=0) pinTx_=pinTx; else pinTx_=GSMMOD2_PIN_SWS_TX;
 crlf_=GSMMOD2_CRLF;
 if((mem=="SM")||(mem=="ME")||(mem=="MT")) mem_=mem;
 else mem_=GSMMOD2_SMS_MEM;
 if(mem_="ME") numOfSms1_=21; else numOfSms1_=1;
 sCmd="";
 sResult="";
 sNetwork="";
 //-----begin software serial-----------------------------------
 if(SWSerial_) {
  sResult="SoftSerial";                // start info
  delete(seri);                        // delete an old object
  seri=new SoftSerial2(pinRx_,pinTx_); // RX,TX
  seri->begin(9600);                   // start software serial
  seri->listen();                      // listen to soft SeriSW
  seri->flush();                       // wait clear out buffer
  while(seri->available()) {           // + empty incomming
   seri->read(); delay(1);             // + buffer
  }
 } else {
  sResult="HardSerial";                // start info
  Serial.begin(9600);                  // start hardware Serial
  Serial.flush();                      // wait clear out buffer
  while(Serial.available()) {          // + empty incomming
   Serial.read(); delay(1);            // + buffer
  }
 }
}

//**************************************************************
//      setter & getter methods
//**************************************************************

//_____get status as int________________________________________
int GSMmodule2::getStatus() { return status; }

//_____Status as little result text (e.g. for OLED display)_____
String GSMmodule2::getsStatus() {
 switch(status) {
  case GSMMOD2_OK: return("OK");
  //----------------------------123456789012345678901234567890--
  case GSMMOD2_ERR_ATE0: return("ERROR on  ATE0 echo off");
  case GSMMOD2_NOMODULE: return("ERROR: no module    found");
  case GSMMOD2_ERR_CMGF: return("ERROR on  message   format");
  case GSMMOD2_ERR_CPMS: return("ERROR on  message   storage");
  case GSMMOD2_ERR_CNMI: return("ERROR on  new messg indication");
  case GSMMOD2_ERR_NOGT: return("ERROR:    No input  sign >");
  case GSMMOD2_ERR_SEND: return("ERROR on  sending   SMS");
  case GSMMOD2_ERR_CMGD: return("ERROR on  deleting  SMS");
  case GSMMOD2_ERR_SNUM: return("ERROR:    Wrong SMS number");
  case GSMMOD2_ERR_NMEM: return("ERROR:    Memory not set");
  case GSMMOD2_ERR_READ: return("ERROR:    SMS not   read");
  case GSMMOD2_ERR_NOSM: return("ERROR:    No SMS    found");
  case GSMMOD2_ERR_CMGL: return("ERROR on  list all  SMS");
  case GSMMOD2_ERR_COPS: return("ERROR: no provider (network)");
  default: return("Unknown   state #"+String(status));
 }
}

//_____get last command_________________________________________
String GSMmodule2::getsCommand() { return sCmd; }

//_____result of last modem request_____________________________
// if replace_=true: replace \r by # and \n by ~
String GSMmodule2::getsResult(bool raw_) { 
 if(raw_) return sResult; 
 String s1=sResult;
 s1.replace("\n", "~");
 s1.replace("\r", "#");
 return s1; 
}

//_____return name of registered network________________________
String GSMmodule2::getsNetwork() { return sNetwork; }

//_____get currently used memory ("SM", "ME" or "MT")___________
String GSMmodule2::getsMem() { return mem_; }

//_____set memory for sms_______________________________________
// hardware access: set preferred memory for all actions
// return: status
int GSMmodule2::setMem(String mem) {
 if((mem=="SM")||(mem=="ME")||(mem=="MT")) {
  mem_=mem;
  if(mem_="ME") numOfSms1_=21; else numOfSms1_=1;
  sCmd="AT+CPMS=\""+mem_+"\",\""+mem_+"\",\""+mem_+"\"";
  sResult=sendCmd(sCmd,4);              // send command
  if(sResult.indexOf("OK")<0) status=GSMMOD2_ERR_CPMS;
  else status=GSMMOD2_OK;
  return status;
 }
 status=GSMMOD2_ERR_NMEM; 
 return status;
}

//_____set memory number of command SMS_________________________
int GSMmodule2::setNumOfSms1(int num1) {
 if(num1>0) { 
  numOfSms1_=num1;
  status=status=GSMMOD2_OK;
 }
 else GSMMOD2_ERR_SNUM;
 return status;
}

//_____return memory number of command SMS______________________
// 1 for mem_="SM" or "MT", 21 or 31 if mem_="ME"
int GSMmodule2::getNumOfSms1() { return numOfSms1_; }

//**************************************************************
//      often used working methods
//**************************************************************

//_____Search for GSM module by sending AT command______________
// return: status (0=OK)
bool GSMmodule2::isModule() {
 sCmd="AT";                            // test command
 sResult=sendCmd(sCmd,2,GSMMOD2_CEND,100);   // send command
 if(sResult.indexOf("OK")<0) {         // try again 
  delay(80);                           // wait a little bit
  sResult=sendCmd(sCmd,2,GSMMOD2_CEND,100);  // send command
  if(sResult.indexOf("OK")<0) {        // now OK?
   status=GSMMOD2_NOMODULE;             // set status to error  
   return false;                       // no modem
  }
 }
 return true;
}

//_____begin = start sms module (setup module)__________________
// if a command fails, it will be repeated once
// return: status
int GSMmodule2::begin()
{
 //-----turn echo on (better for debug)-------------------------
 sCmd="ATE0";                          // echo 0=off, 1=on
 sResult=sendCmd(sCmd,2,GSMMOD2_CEND,100);   // send command
 if(sResult.indexOf("OK")<0) {         // try again
  delay(80);                           // wait a little bit
  sResult=sendCmd(sCmd,2,GSMMOD2_CEND,100);  // send command
  if(sResult.indexOf("OK")<0) {        // 2. result
   status=GSMMOD2_ERR_ATE0;             // set status to error
   return status;                      // return
  }
 }
 //-----set sms text format-------------------------------------
 sCmd="AT+CMGF=1";                     // use text format
 sResult=sendCmd(sCmd);                // send command
 if(sResult.indexOf("OK")<0) {         // try again 
  delay(80);                           // wait a little bit
  sResult=sendCmd(sCmd);               // send command
  if(sResult.indexOf("OK")<0) {        // 2. result 
   status=GSMMOD2_ERR_CMGF;             // set status to error
   return status;                      //
  }
 }
 //-----set preferred message storage to modem (ME)-------------
 setMem(GSMMOD2_SMS_MEM);               // default ME
 if(status!=GSMMOD2_OK) {               // if error...
  delay(80);                           // ...wait a little...
  setMem(GSMMOD2_SMS_MEM);              // ...and try again.
 }
 if(status!=GSMMOD2_OK) return status;  // no success
 //-----calculate location of 1st SMS in memory-----------------
 readNumOfSms1();
 //-----configuration message routing---------------------------
 /*
 sCmd="AT+CNMI=2,1,2,1,0";             // info on receiving
 sResult=sendCmd(sCmd);                // send command
 if(sResult.indexOf("OK")<0) {         // try again 
  delay(80);                           // wait a little bit
  sResult=sendCmd(sCmd);               // send command
  if(sResult.indexOf("OK")<0) {        // 2. result 
   status=GSMMOD2_ERR_CNMI;             // set status to error
   return status;                      //
  }
 }
 */
 //-----get registered network----------------------------------
 sNetwork=readNetwork();               //read registered network
 return status;                        //(this sets status ;)
}

//_____send a sms (try 2x)______________________________________
int GSMmodule2::sendSMS(SMS sms1)
{
 String s1;                            // help command string
 String txt2=sms1.text+"\x1A";         // sms text (end 1A)
 //-----contains txt2 char GSMMOD2_CEND?-------------------------
 int numEnd=4+count_1in2(String(GSMMOD2_CEND), txt2);
 //-----send sms------------------------------------------------
 sCmd="AT+CMGS=\""+sms1.phone+"\"";    // cmd send sms
 sResult=sendCmd(sCmd,1,'>');          // "send sms" to modem
 if(sResult.indexOf(">")<0) {          // no input sign 
  sendCmd("\x1A",1,'\n',200);          // just for safety
  if(!sendAT()) {                      // start again
   if(!sendAT()) 
   { status=GSMMOD2_ERR_NOGT; return status; }
  }
  delay(50);                           // wait a little bit
  sResult=sendCmd(sCmd,1,'>');         // "send sms" to modem
  if(sResult.indexOf(">")<0) 
  { status=GSMMOD2_ERR_NOGT; return status; }
 }
 //-----got input sign, send sms text---------------------------
 sResult=sendCmd(txt2,numEnd,GSMMOD2_CEND,10000); // send text
 if(sResult.indexOf("OK")<0)
 { status=GSMMOD2_ERR_SEND; return status; }
 status=GSMMOD2_OK;
 return status;
}

//_____Is sms numOfSms1_ available?_____________________________
// return: true=yes, false=no
bool GSMmodule2::isSMS1() { return isSMS(numOfSms1_); }

//_____read first SMS in used memory____________________________
SMS GSMmodule2::readSMS1() { return(readSMS(numOfSms1_)); }

//_____delete first SMS in used memory__________________________
// return: ERROR code
int GSMmodule2::deleteSMS1() { return(deleteSMS(numOfSms1_)); }

//**************************************************************
//      some more working methods
//**************************************************************

//_____Is sms available?________________________________________
// return: true=yes, false=no
bool GSMmodule2::isSMS(int num)
{
 sCmd="AT+CMGR="+String(num);          // read certain SMS
 sResult=sendCmd(sCmd,4,GSMMOD2_CEND,500);
 if(sResult.length()<40) return false;
 return true;
}

//_____read a certain SMS with given number_____________________
// return: SMS
SMS GSMmodule2::readSMS(int num)
{
 SMS sms1;                             // result SMS
 String s1;                            // help string
 int iPos1, iPos2;                     // index SMS string
 if(num<0) 
  { status=GSMMOD2_ERR_SNUM; return sms1; }
 sCmd="AT+CMGR="+String(num);          // read certain SMS
 sResult=sendCmd(sCmd, 5, GSMMOD2_CEND);
 if(sResult.indexOf("OK")<0) {
  delay(100);
  sResult=sendCmd(sCmd, 5, GSMMOD2_CEND);
  if(sResult.indexOf("OK")<0) 
   { status=GSMMOD2_ERR_READ; return sms1; }
 }
 //-----take string to pieces-----------------------------------
 if(sResult.indexOf("CMGR:")<0)
 { status=GSMMOD2_ERR_NOSM; return sms1; }
 int ok_=true;
 String phone_, date_, text_;
 iPos1=2+sResult.indexOf(",\"");
 iPos2=sResult.indexOf("\",", iPos1);
 if(iPos2>iPos1) phone_=sResult.substring(iPos1,iPos2);
 else ok_=false;
 iPos1=2+sResult.indexOf(",\"",iPos2+2);
 iPos2=sResult.indexOf("\"",iPos1+1);
 if(iPos2>iPos1) date_=sResult.substring(iPos1,iPos2);
 else ok_=false;
 iPos1=iPos2+3;
 iPos2=-1+sResult.indexOf("OK",iPos1+1);
 if(iPos2>iPos1) {
  while(((sResult.charAt(iPos2)=='\r')||
         (sResult.charAt(iPos2)=='\n'))&&iPos2>iPos1) iPos2--;
  text_=sResult.substring(iPos1,iPos2+1);
 }
 else ok_=false;
 if(!ok_) status=GSMMOD2_ERR_READ;
 else {
  sms1.number=num;
  sms1.phone=phone_;
  sms1.date=date_;
  sms1.text=text_;
  status=GSMMOD2_OK;
 }
 return sms1;
}

//_____delete a certain SMS_____________________________________
// return: ERROR code
int GSMmodule2::deleteSMS(int num)
{
 if(num<1) return GSMMOD2_ERR_SNUM;     //
 sCmd="AT+CMGD="+String(num);          // read certain SMS
 sResult=sendCmd(sCmd);                // send command
 if(sResult.indexOf("OK")<0) {         // try again 
  delay(80);                           // wait a little bit
  sResult=sendCmd(sCmd);               // send command
  status=GSMMOD2_ERR_CMGD;              // set status to error
  return status;                       //
 }
 status=GSMMOD2_OK;                     // set status to OK
 return status;                        // 
}

//_____list all SMS in used memory______________________________
String GSMmodule2::listAllSMS()
{
 int iNum=readNumSmsInMem();           // number of stored SMS
 unsigned long ms=GSMMOD2_WAIT_MS+500*iNum;//more time for answer
 sCmd="AT+CMGL=\"ALL\"";               // read all messages
 sResult=sendCmd(sCmd,(3*iNum+2),GSMMOD2_CEND,ms);   // send cmd
 if(sResult.indexOf("OK")<0) status=GSMMOD2_ERR_CMGL;
 else status=GSMMOD2_OK;
 return sResult;
}

//_____register to specified GSM network________________________
//  hardware access: register GSM network 
int GSMmodule2::sendNetwork(String net) {
 sCmd="AT+COPS=1,0,\""+net+"\"";       // set Network
 sResult=sendCmd(sCmd,4,GSMMOD2_CEND,20000); // wait 20sec!
 if(sResult.indexOf("OK")<0) {         // try again
  delay(80);                           // wait a little bit
  sResult=sendCmd(sCmd,4,GSMMOD2_CEND,20000); // wait 20sec!
  if(sResult.indexOf("OK")<0) {        // try again
   status=GSMMOD2_ERR_COPS;             // set status to error
   return status;                      //
  }
 }
 delay(80);                            // wait a little bit
 sNetwork=readNetwork();               // check change
 readNumOfSms1();                      // may be changed!
 return status;                        // 
}

//_____get registered network (provider) or empty string________
// hardware access: get network name from GSM module
String GSMmodule2::readNetwork() {
 String s1="";                         // answer string
 int iPos1, iPos2;                     // helper vars
 sCmd="AT+COPS?";                      // read 
 sResult=sendCmd(sCmd, 4);             // send command
 if(sResult.length()<15)
  { status=GSMMOD2_ERR_COPS; return s1; }
 //-----isolate provider name-----------------------------------
 iPos1=2+sResult.indexOf(",\"");       // start on provider name
 if(iPos1<2) 
  { status=GSMMOD2_ERR_COPS; return s1; }
 iPos2=sResult.indexOf("\"",iPos1);    // end of proviver name
 if((iPos2<0)||(iPos2<=iPos1))
  { status=GSMMOD2_ERR_COPS; return s1; }
 sNetwork=sResult.substring(iPos1, iPos2);   // provider name :)
 status=GSMMOD2_OK;
 return sNetwork;
}

//_____return number of stored SMS in selected mem______________
// hardware access: get number of stored SMS
// return: -1 on error
int GSMmodule2::readNumSmsInMem() {
 int iPos1, iPos2;
 sCmd="AT+CPMS=\""+mem_+"\"";          // command
 sResult=sendCmd(sCmd,4);              // send cmd
 if(sResult.indexOf("OK")<0) {         // error...
  delay(80);                           // ...wait and try again
  sResult=sendCmd(sCmd,4);             // send cmd
  if(sResult.indexOf("OK")<0)return -1;// error 
 }
 iPos1=2+sResult.indexOf(": ");        // start of max SM value
 if(iPos1<2) return -1;                // error
 iPos2=sResult.indexOf(",",iPos1);     // end of max SM value
 if((iPos2<0)||(iPos2<=iPos1)) return -1;  // error
 return(sResult.substring(iPos1,iPos2).toInt());
}

//_____return maximum number of SMS in selected mem_____________
// hardware access: get maximum number of SMS
// return: -1 on error
int GSMmodule2::readMaxSmsInMem() {
 int iPos1, iPos2;
 sCmd="AT+CPMS=\""+mem_+"\"";          // command 
 sResult=sendCmd(sCmd,4);              // send cmd  
 if(sResult.indexOf("OK")<0) {         // error...
  delay(80);                           // ...wait and try again
  sResult=sendCmd(sCmd,4);             // send cmd
  if(sResult.indexOf("OK")<0)return -1;// error 
 }
 iPos1=1+sResult.indexOf(",");         // start of max SM value
 if(iPos1<2) return -1;                // error
 iPos2=sResult.indexOf(",",iPos1);     // end of max SM value
 if((iPos2<0)||(iPos2<=iPos1)) return -1;  // error
 return(sResult.substring(iPos1,iPos2).toInt());
}

//_____read SIM integrated Circuit Card IDentification__________
// return ID or empty string
String GSMmodule2::readSimId() {
 int iPos2;
 sCmd="AT+CCID";                       // command 
 sResult=sendCmd(sCmd,4);              // send cmd  
 if(sResult.indexOf("OK")<0) {         // error...
  delay(80);                           // ...wait and try again
  sResult=sendCmd(sCmd,4);             // send cmd
  if(sResult.indexOf("OK")<0)return "";// error 
 }
 String s1=sResult;
 s1.replace("OK","");
 s1.replace("\r\n","");
 return s1;
}

//_____request SMS with prepaid balance, return status__________
// method deletes first SMS location in actually used memory
// return: status
int GSMmodule2::requestBalanceSMS() {
 SMS sms1;
 unsigned long msEnd;                  // end time [ms]
 bool phonecall=true;
 //-----prepair memory location for SMS------------------------- 
 sendAT();                             // for safety
 status=deleteSMS1();                  // delete 1. sms location
 if(status!=GSMMOD2_OK) return status;  // no success on delete
 //-----do balance request for special provider-----------------
 if(sNetwork==String(GSMMOD2_NET_ID1)) {
  //.....send request (voice call)..............................
  sCmd="ATD"+String(GSMMOD2_NET_TEL1)+";";   // voice call
  sResult=sendCmd(sCmd);                    // dial
  sCmd="AT+CLCC";                           //List Current Calls
  msEnd=GSMMOD2_NET_MS_1+millis();           // end time [ms]
  while(phonecall && (millis()<msEnd)) {    // wait for answer
   delay(2000);
   sResult=sendCmd(sCmd);                   // active call?
   if(sResult.indexOf("CLCC:")<0) phonecall=false;
  }
  delay(50);                           // wait a little
  sCmd="ATH";                          // hang up...
  sResult=sendCmd(sCmd);               // ...anyway
  if(sResult.indexOf("OK")<0) {        // not hang up?
   delay(50);                          // wait a little
   sResult=sendCmd(sCmd);              // try again
  }
 }
 //-----next provider-------------------------------------------
 else if(sNetwork==String(GSMMOD2_NET_ID2)) {
  //.....send request (sms).....................................
  sms1.phone=String(GSMMOD2_NET_TEL2);  // SMS number for balance
  sms1.text=String(GSMMOD2_NET_TXT2);   // SMS request text
  sendSMS(sms1);                       // request balance
 }
 //-----next provider-------------------------------------------
 else if(sNetwork==String(GSMMOD2_NET_ID3)) {
  //.....send request (sms).....................................
  sms1.phone=String(GSMMOD2_NET_TEL3);  // SMS number for balance
  sms1.text=String(GSMMOD2_NET_TXT3);   // SMS request text
  sendSMS(sms1);                       // request balance
 }
 //-----unknown provider----------------------------------------
 else { };
 return status;
}

//_____extract balance value from SMS___________________________
// return: Balance (credit) value as String or empty String
String GSMmodule2::extractBalance(String smstext) {
 String sRet="";
 int pos1, pos2;
 //-----isolate balance value string from SMS textline----------
 if(sNetwork==String(GSMMOD2_NET_ID1)) {
  pos1=4+smstext.indexOf("EUR ");      // start of balance value
  pos2=smstext.indexOf(" Gut",pos1);   // end of balance value
  if(pos2>pos1) sRet=smstext.substring(pos1,pos2);
 } 
 //-----next provider-------------------------------------------
 else if(sNetwork==String(GSMMOD2_NET_ID2)) {
  pos1=3+smstext.indexOf("gt ");       // start of balance value
  pos2=smstext.indexOf(" ",pos1);      // end of balance value
  if(pos2>pos1) sRet=smstext.substring(pos1,pos2);
 }
 //-----next provider-------------------------------------------
 else if(sNetwork==String(GSMMOD2_NET_ID3)) {
  String s1=smstext;                   //
  s1.replace("00","");                 //16 hex bit -> 8 bit hex
  s1=hexStringToString(s1);            // -> 8 bit ASCII
  pos1=3+s1.indexOf("gt ");            // start of balance value
  pos2=s1.indexOf(" ",pos1);           // end of balance value
  if(pos2>pos1) sRet=s1.substring(pos1,pos2);
 }
 //-----unknown provider----------------------------------------
 else { };
 return sRet;
}

//**************************************************************
//    helper functions
//**************************************************************

//_____send a standard command to SIM800C_______________________
String GSMmodule2::sendCmd(String cmd) { 
 return(sendCmd(cmd, 2, GSMMOD2_CEND, GSMMOD2_WAIT_MS));
}

//_____send a standard command to SIM800C_______________________
String GSMmodule2::sendCmd(String cmd, int numEnd) { 
 return(sendCmd(cmd, numEnd, GSMMOD2_CEND, GSMMOD2_WAIT_MS));
}

//_____send a standard command to SIM800C_______________________
String GSMmodule2::sendCmd(String cmd, int numEnd, char cEnd) { 
 return(sendCmd(cmd, numEnd, cEnd, GSMMOD2_WAIT_MS));
}

//_____send a command to SIM800C________________________________
// sEnd  : End-String (or character as String)
// numEnd: how often should end string be found in answer
// minimum duration: about 100ms
// return: modem answer string or "" on error
String GSMmodule2::sendCmd(String cmd, int numEnd, char cEnd, unsigned long ms)
{
 int    i=0;                           // receive byte counter
 char   c;                             // received byte
 String sRet="";                       // answer string
 String cmd2=cmd;                      // command + carriage ret
 int    lenCmd2;                       // length of command
 bool   reading=true;                  // read command
 int    len;                           // number of chars
 
 int    cntEnd=0;                      // number of end chars
 unsigned long msEnd;                  // end time [ms]
 char   aok[]={'O','K','\r',cEnd,0};
 char*  pok=aok; 
 if(cmd.indexOf(String(char(26)))<0) cmd2=cmd+GSMMOD2_CRLF;
 lenCmd2=cmd2.length();
 //-----empty receive buffer, send command----------------------
 if(SWSerial_) {
  while((len=seri->available())>0) { seri->read(); delay(2); }
  delay(20);                            // wait a little bit
  seri->print(cmd2);                    // send command
 } else {
  while((len=Serial.available())>0){ Serial.read();  delay(2); }
  delay(20);                            // wait a little bit
  Serial.print(cmd2);                   // send command
 }
 //-----receive answer------------------------------------------
 delay(20+lenCmd2);                   // wait a little bit
 msEnd=ms+millis();                    // end time [ms]
 while(reading && (millis()<msEnd)) {  // wait for answer
  yield();                             // do watch dog etc.
  if(SWSerial_) len=seri->available(); // chars available?
           else len=Serial.available();// chars available?
  if(len>0) {                          // chars here!
   if(SWSerial_) c=seri->read();       // read char
            else c=Serial.read();      // read char
   sRet+=String((char)c);              // add char to answer
   if(c==*pok) {                       // OK text...
    pok++; if(*pok==0) reading=false;  // ...found
    } else pok=aok;                    // ...not found
   if(c==cEnd) cntEnd++;               // end char detected
   if(cntEnd>=numEnd) reading=false;   // enough end chars
  }                                    // END: chars here!
  else delay(20);                      // no chars here: wait
 }                                     // END: wait for answer
 //-----just for safety: empty receive buffer-------------------
 if(SWSerial_)
 { while((len=seri->available())>0){seri->read(); delay(2);} }
 else
 { while((len=Serial.available())>0){Serial.read(); delay(2);} }
 return sRet;                          // return answer string
}

//_____return memory number of command SMS______________________
// hardware access: get max. number of SMS in SIM memory SM
// 1 for mem_="SM" or "MT", 21 or 31 if mem_="ME"
int GSMmodule2::readNumOfSms1() {
 int iPos1, iPos2;
 if(mem_!="ME") numOfSms1_=1;
 else {
  numOfSms1_=21;                       // default value
  String sC="AT+CPMS=\"SM\"";          // command change mem
  String s1=sendCmd(sC,4);             // set mem to SM
  setMem(mem_);                        // set mem back
  iPos1=1+s1.indexOf(",");             // start of max SM value
  if(iPos1<2) return numOfSms1_;       // error -> default value
  iPos2=s1.indexOf(",",iPos1);         // end of max SM value
  if((iPos2<0)||(iPos2<=iPos1)) return numOfSms1_;  // default
  int i=s1.substring(iPos1,iPos2).toInt();
  if(i>0) numOfSms1_=1+i;
 }
 return numOfSms1_; 
}
//_____Send AT command without effecting sCmd, sResult, status__
// clears serial buffer before sending AT command
// duration: about 100ms ... 200ms
// return: true = OK received, false ERROR
bool GSMmodule2::sendAT() {
 String s1="AT";                       // test command
 s1=sendCmd(s1,2,GSMMOD2_CEND,100);     // send command
 if(s1.indexOf("OK")<0) return false;  // now OK?
 return true;
}

//_____is software serial? (true=software serial)_______________
bool GSMmodule2::isSoftwareSerial() { return SWSerial_; }

//_____set software (true) or hardware (false) serial for module
// return: status after methode begin()
int GSMmodule2::setSoftwareSerial(bool swSerial) {
 SWSerial_=swSerial;
 setup(swSerial, pinRx_, pinTx_, mem_);
 return begin();
}

//_____convert HEX-String to ASCII-String_______________________
// e.g. 48414C4C4F -> HALLO
String GSMmodule2::hexStringToString(String sHex)
{
 String sRet="";
 int len_=sHex.length();
 for(int i=0; i<len_; i=i+2)
 {
  char cc=(hex2int(sHex.charAt(i))<<4)|hex2int(sHex.charAt(i+1));
  sRet+=String(cc);
 }
 return sRet;
}

//_____count occurrence of a specific string in another string__
int GSMmodule2::count_1in2(String sFind, String sBasic)
{
 int iPos=0;
 int iNum=0;
 int iLen=sFind.length();
 while(sBasic.indexOf(sFind,iPos)>=0) {
  iPos=sBasic.indexOf(sFind, iPos)+iLen;
  iNum++;
 }
 return iNum;
}

//_____just for test: write a character to (software) serial____
void GSMmodule2::write(char c) {
 if(SWSerial_) seri->write(c);
 else Serial.write(c);
}

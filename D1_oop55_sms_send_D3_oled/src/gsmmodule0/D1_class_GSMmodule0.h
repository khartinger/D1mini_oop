//_____D1_class_GSMmodule0.h__________________181229-181229_____
// D1 mini class for sending and receiving SMS with a
// SIM800C GSM GPRS module STM32
// Information about last GSM command (feedback methods):
//   getsCommand() ... last (AT-) command (String)
//   getsResult() .... result of last command (String)
//   getStatus() ..... status value (0 = OK)
//   getsStatus() .... status message (String)
// Hardware: (1) WeMos D1 mini
//           (2) Battery Shield for WEMOS D1 mini
//           (3) Selfmade SIM800C 9pol ProtoBoard Shield with
//               SIM800C GSM GPRS module STM32
// Created by Karl Hartinger, December 29, 2018.
// Last modified: -
// Released into the public domain.

#ifndef D1_CLASS_GSMMODULE0_H
#define D1_CLASS_GSMMODULE0_H
#define GSMMOD_SWS            true     //software serial
//#define GSMMOD_SWS           false     //hardware serial
#include "Arduino.h"                   // D5,D6

#if GSMMOD_SWS
 #include <SoftwareSerial.h>
#endif

//_____convert hex char to value (e.g. C -> 12)_________________
#define hex2int(c)( (c>='0' && c<='9') ? (c-'0') : (9+(c&0xF)) )

#define  GSMMOD_PIN_SWS_TX      D5     // D5 SW Serial send
#define  GSMMOD_PIN_SWS_RX      D6     // D6 SW Serial receive
#define  GSMMOD_CRLF          "\r"     // sim command: end chars
#define  GSMMOD_CEND          '\n'     // sim answer: end char
#define  GSMMOD_WAIT_MS       4000     // sim command timeout
#define  GSMMOD_SMS_MEM       "ME"     // ME=modem, SM=sim card
#define  GSMMOD_SMS_NUM         21     // sms# ME: 21, SM:1
//-------provider/network data (Austria)------------------------
#define  GSMMOD_NET_ID1      "A1"           //networkID B.free
#define  GSMMOD_NET_TEL1     "08006642903"  //b.free phone numb
#define  GSMMOD_NET_MS_1     29000          //A1 wait max.29sec
#define  GSMMOD_NET_ID2      "T-Mobile A"   //networkID HOT
#define  GSMMOD_NET_TEL2     "6700"         //Hot phone number
#define  GSMMOD_NET_TXT2     "GUTHABEN"     //Hot balance text
#define  GSMMOD_NET_ID3      "one"          //networkID Drei
#define  GSMMOD_NET_TEL3     "31321"        //Drei phone number
#define  GSMMOD_NET_TXT3     "GUT"          //Drei balance text
//-------status-------------------------------------------------
#define  GSMMOD_OK               0     // success
#define  GSMMOD_NOMODULE         1     // ERROR no module found
#define  GSMMOD_ERR_ATE0         2     // ERROR while ATE0
#define  GSMMOD_ERR_CMGF         3     // ERROR while AT+CMGF
#define  GSMMOD_ERR_CPMS         4     // ERROR while AT+CPMS
#define  GSMMOD_ERR_CNMI         5     // ERROR while AT+CNMI
#define  GSMMOD_ERR_NOGT         6     // ERROR no input sign >
#define  GSMMOD_ERR_SEND         7     // ERROR while sending sms
#define  GSMMOD_ERR_CMGD         8     // ERROR while deleting
#define  GSMMOD_ERR_SNUM         9     // ERROR wrong sms number
#define  GSMMOD_ERR_NMEM        10     // ERROR memory not set
#define  GSMMOD_ERR_READ        11     // ERROR: SMS not read
#define  GSMMOD_ERR_NOSM        12     // ERROR: No SMS
#define  GSMMOD_ERR_CMGL        13     // ERROR: SMS not listed
#define  GSMMOD_ERR_COPS        14     // ERROR: no network

//_____info about a SMS_________________________________________
struct SMS {
 int number=-1;
 String phone="";
 String date="";
 String text="";
};

class GSMmodule0 {
 //-----properties----------------------------------------------
 protected:
  int    status;                       // state of sms module
  int    pinRx_;                       // software serial pin RX
  int    pinTx_;                       // software serial pin TX
  String crlf_;                        // end of sms command
  String mem_;                         // "SM", "ME", "MT"
  int    numOfSms1_;                   // number 1. sms location
  String sCmd;                         // command string
  String sResult;                      // answer of modem 
  String sNetwork;                     // registered network
#if GSMMOD_SWS
  SoftwareSerial *seri;                // software serial
#endif
 //------constructor & co---------------------------------------
 public:
  GSMmodule0();
  GSMmodule0(String mem);              // mem + default pins
  GSMmodule0(int pinRx, int pinTx);    // pins + default mem
  GSMmodule0(int pinRx, int pinTx, String mem); //
  ~GSMmodule0();
  void setup();
  void setup(int pinRx, int pinTx, String mem);
 //------setter & getter methods--------------------------------
 public: 
  int    getStatus();                  // get status as int
  String getsStatus();                 // get status as text
  String getsCommand();                // get last command
  String getsResult(bool raw_=true);   // last modem answer
  String getsNetwork();                // registered network
  String getsMem();                    // "SM", "ME", "MT"
  int    setMem(String mem);           // set memory for sms
  int    setNumOfSms1(int num1);       // memory number of...
  int    getNumOfSms1();               // ...command SMS
 //------often used working methods-----------------------------
  bool   isModule();                   // is a module answering?
  int    begin();                      // start module
  int    sendSMS(SMS sms1);            // send a sms
  bool   isSMS1();                     // true: sms1 received
  SMS    readSMS1();                   // read first SMS
  int    deleteSMS1();                 // delete first SMS
 //------some more working methods------------------------------
  bool   isSMS(int num);               // true: sms received
  SMS    readSMS(int num);             // read SMS num
  int    deleteSMS(int num);           // delete SMS number num
  String listAllSMS();                 // list all sms as string
  int    sendNetwork(String net);      // set gsm network
  String readNetwork();                // registered gsm network
  int    readNumSmsInMem();            // number of stored SMS
  int    readMaxSmsInMem();            // max. number of SMS
  String readSimId();                  // SIM Card ID
  int    requestBalanceSMS();          // provider specific
  String extractBalance(String smstext);// provider specific
 //------helper functions---------------------------------------
  String sendCmd(String cmd);          // send AT command
  String sendCmd(String cmd, int numEnd); //
  String sendCmd(String cmd, int numEnd, char cEnd);
  String sendCmd(String cmd, int numEnd, char cEnd, unsigned long ms);
  int    readNumOfSms1();              // calc num using mem
  bool   sendAT();                     // no effect on sCmd, ...
  bool   isSoftwareSerial();           // true=software serial
  String hexStringToString(String sHex); // HEX- to ASCII-String
  int    count_1in2(String sFind, String sBasic);
  void   write(char c);
};
#endif

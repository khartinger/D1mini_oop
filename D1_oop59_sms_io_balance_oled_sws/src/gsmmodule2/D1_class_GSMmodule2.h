//_____D1_class_GSMmodule2.h___________________181230-181230_____
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
// Created by Karl Hartinger, December 30, 2018.
// Last modified: -
// Released into the public domain.
#ifndef D1_CLASS_GSMMODULE_H
#define D1_CLASS_GSMMODULE_H
#include "Arduino.h"                   // D5,D6
#include "D1_class_SoftSerial2.h"

//_____convert hex char to value (e.g. C -> 12)_________________
#define hex2int(c)( (c>='0' && c<='9') ? (c-'0') : (9+(c&0xF)) )

#define  GSMMOD2_PIN_SWS_TX     D5     // D5 SW Serial send
#define  GSMMOD2_PIN_SWS_RX     D6     // D6 SW Serial receive
#define  GSMMOD2_CRLF         "\r"     // sim command: end chars
#define  GSMMOD2_CEND         '\n'     // sim answer: end char
#define  GSMMOD2_WAIT_MS      4000     // sim command timeout
#define  GSMMOD2_SMS_MEM      "ME"     // ME=modem, SM=sim card
#define  GSMMOD2_SMS_NUM        21     // sms# ME: 21, SM:1
//-------provider/network data (Austria)------------------------
#define  GSMMOD2_NET_ID1     "A1"           //networkID B.free
#define  GSMMOD2_NET_TEL1    "08006642903"  //b.free phone numb
#define  GSMMOD2_NET_MS_1    29000          //A1 wait max.29sec
#define  GSMMOD2_NET_ID2     "T-Mobile A"   //networkID HOT
#define  GSMMOD2_NET_TEL2    "6700"         //Hot phone number
#define  GSMMOD2_NET_TXT2    "GUTHABEN"     //Hot balance text
#define  GSMMOD2_NET_ID3     "one"          //networkID Drei
#define  GSMMOD2_NET_TEL3    "31321"        //Drei phone number
#define  GSMMOD2_NET_TXT3    "GUT"          //Drei balance text
//-------status-------------------------------------------------
#define  GSMMOD2_OK              0     // success
#define  GSMMOD2_NOMODULE        1     // ERROR no module found
#define  GSMMOD2_ERR_ATE0        2     // ERROR while ATE0
#define  GSMMOD2_ERR_CMGF        3     // ERROR while AT+CMGF
#define  GSMMOD2_ERR_CPMS        4     // ERROR while AT+CPMS
#define  GSMMOD2_ERR_CNMI        5     // ERROR while AT+CNMI
#define  GSMMOD2_ERR_NOGT        6     // ERROR no input sign >
#define  GSMMOD2_ERR_SEND        7     // ERROR while sending sms
#define  GSMMOD2_ERR_CMGD        8     // ERROR while deleting
#define  GSMMOD2_ERR_SNUM        9     // ERROR wrong sms number
#define  GSMMOD2_ERR_NMEM       10     // ERROR memory not set
#define  GSMMOD2_ERR_READ       11     // ERROR: SMS not read
#define  GSMMOD2_ERR_NOSM       12     // ERROR: No SMS
#define  GSMMOD2_ERR_CMGL       13     // ERROR: SMS not listed
#define  GSMMOD2_ERR_COPS       14     // ERROR: no network

//_____info about a SMS_________________________________________
struct SMS {
 int number=-1;
 String phone="";
 String date="";
 String text="";
};

class GSMmodule2 {
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
  bool   SWSerial_;                    // true = software Serial
  SoftSerial2 *seri;                   // software serial
 //------constructor & co---------------------------------------
 public:
  GSMmodule2();
  GSMmodule2(bool swSerial);            // hard-/software serial
  GSMmodule2(bool swSerial, String mem);// hard-/soft-serial+mem
  GSMmodule2(String mem);               // sws+mem + default pins
  GSMmodule2(int pinRx, int pinTx);     // sws+pins + default mem
  GSMmodule2(int pinRx, int pinTx, String mem); // sws+pins+mem
  ~GSMmodule2();
  void setup();
  void setup(bool swSerial, int pinRx, int pinTx, String mem);
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
  bool   isModule();                   // module answering AT?
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
  bool   isSoftwareSerial();              //true=software serial
  int    setSoftwareSerial(bool swSerial);//true=software serial
  String hexStringToString(String sHex); // HEX- to ASCII-String
  int    count_1in2(String sFind, String sBasic);
  void   write(char c);
};
#endif

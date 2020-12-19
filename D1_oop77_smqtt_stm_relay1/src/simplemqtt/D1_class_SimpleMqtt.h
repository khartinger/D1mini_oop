//_____D1_class_SimpleMqtt.h__________________200705-201219_____
// The SimpleMqtt class is suitable for D1 mini (ESP8266) and 
// and ESP32 D1mini and extends the PubSubClient class to make
// MQTT easy to use.
// * For this purpose a "base" topic (topicbase, default is
//   simplemqtt/default) is defined, which is extended by the
//   following keywords:
// /get Request of the value specified in the payload
//      e.g. version query:
//      -t simplemqtt/default/get -m version
// /set Set a value e.g. 
//      -t simplemqtt/default/set/topicbase -m new_base
// * In response you get a MQTT message with the keyword /ret,
//    e.g. -t simplemqtt/default/ret/version -m 2020-12-08
// * Furthermore, all (arbitrary) topics defined in the constant
//   TOPIC_SUB are subscribed.
// * Furthermore, all commands of the PubSubClient class can 
//   still be used.
//
// In the user program the following string constants must be
// defined. Type of board:
// *  #define  D1MINI         1            // ESP8266 D1mini +pro
// OR #define  ESP32D1        2            // ESP32 D1mini
// Topic base and key words for get-, set, sub-, pub-action
// (key words comma separated or empty string):
// * TOPIC_BASE, TOPIC_GET, TOPIC_SET, TOPIC_SUB, TOPIC_PUB
// Furthermore, the following functions must be implemented
// in the user program:
// * void callback(char* topic, byte* payload, unsigned int length)
//   { client.callback_(topic, payload, length); }
// * String simpleGet(String sPayload) { return String(""); }
// * String simpleSet(String sTopic, String sPayload) { return String(""); }
// * void simpleSub(String sTopic, String sPayload) { }
// Finally, an object must be created in the user program, e.g.
// * SimpleMqtt client("ssid", "password", "mqttservername|ip");
// and in the loop() function the method 
// * client.doLoop();
// must be called, otherwise no MQTT messages are processed!
//
// Note: If the PubSubClient class is registered (installed)
//       in the IDE, the PubSubClient files in the directory
//       src/simplemqtt should be deleted.
// Created by Karl Hartinger, December 08, 2020.
// Changes:
// 2020-12-16 add setLanguage (language_, SIMPLEMQTT_LANGUAGE)
// 2020-12-19 connectMQTT(): add line 2 if(!isWiFiConnected...
//            EEPROM: add eeprom...myData()
// Hardware: D1 mini OR ESP32 D1mini
// Released into the public domain.

#ifndef D1_CLASS_SIMPLEMQTT_H
#define D1_CLASS_SIMPLEMQTT_H
#include "Arduino.h"                   // D1, ...
#ifdef ESP8266
 #include <ESP8266WiFi.h>              // network connection
#endif
#if defined(ESP32) || defined(ESP32D1)
 #include <WiFi.h>                     // network connection
#endif
#include "PubSubClient.h"              // MQTT basic methods
#include "EEPROM.h"                    // to save mqtt base

//_____functions MUST be defined in main program________________
String simpleGet(String sPayload);
String simpleSet(String sTopic, String sPayload);
void   simpleSub(String sTopic, String sPayload);

//_____defines__________________________________________________
#if !defined(DEBUG_MQTT)
 #define DEBUG_MQTT          true      // true=Serial output
#endif
#define SIMPLEMQTT_LANGUAGE  'e'       // e=english, d=deutsch
#define SIMPLEMQTT_VERSION   "SimpleMqtt_Version_2020-12-17"
#define SIMPLEMQTT_BASE      "simplemqtt/default"
#define STARTINFO_TOPIC      "info/start"
#define STARTINFO_ALLOW      true      // send start info

#define MQTT_SSID    "ssid"            // wlan name
#define MQTT_PASS    ""                // no password
#define MQTT_SERVER  "localhost"       // default server name
#define MQTT_PORT    1883              // default mqtt port
#define NO_IP        "xxx.xxx.xxx.xxx" // 

#define  TOPIC_MAX                  12 // max. topics to sub
#define  NOTHING_TODO                0 // # mqtt do nothing
#define  MQTT_RECONNECT_MS         200 //
#define  TIMEOUT_WIFI_CONNECT_MS  8000 // wait for WLAN
#define  WIFI_CONNECTING_COUNTER     1 // connecting after begin
#define  PAYLOAD_MAXLEN            200 //
#define  USE_EEPROM               true // 
#define  EEPROM_SIZE               256 // 
#define  EEPROM_SIZE_MAX          4096 //
#define  EEPROM_DATA_MAX            99 //
#define  TOPIC_MAXLEN               48 // must be EEPROMSIZE-12

//-----Bits for connection state--------------------------------
#define  CON_RESET              0x0000 //  0
#define  BIT_WIFI_LAST          0x0001 // bit 1
#define  BIT_WIFI_CON           0x0002 // bit 2
#define  BIT_WIFI_DIS           0x0004 // bit 3

#define  BIT_MQTT_LAST          0x0100 // bit 8
#define  BIT_MQTT_CON           0x0200 // bit 9
#define  BIT_MQTT_DIS           0x0400 // bit 10

#define  BIT_CONN_ERROR         0x8000 // bit 16

class SimpleMqtt : public PubSubClient {
 //------properties---------------------------------------------
 protected:
  String ssid_;                        // WiFi newtwork name
  String pass_;                        // WiFi password
  String mqtt_;                        // MQTT server (name or ip)
  int    port_;                        // mqtt port (def 1883)
  char   language_;                    // e=english, d=deutsch
  WiFiClient d1miniClient;             // WiFi client for MQTT
  String sMQTTClientName;              // MQTT client name
  String sMyIP;                        // client IP address
  int    wifiWaitMsMax;                // WiFi waiting time connect
  int    wifiConnectingCounter;        // connectings counter
  int    wifiConnectingCounterMax;     // connectings after begin
  unsigned long conState;              // connection state
  EEPROMClass *eeprom_;                // new EEPROMClass;
  unsigned long eepromSize_;           // 
  unsigned long iGet;                  // mask for get topics
  unsigned long iSet;                  // mask for set topics
  unsigned long iRet;                  // mask for ret topics
  unsigned long iRetSet;               // mask for retset topics
  unsigned long iSub;                  // mask for sub topics
  unsigned long iPub;                  // mask for pub topics
  int    numTopicGet;                  // numer of get topics
  int    numTopicSet;                  // numer of set topics
  int    numTopicSub;                  // numer of sub topics
  int    numTopicPub;                  // numer of pub topics
  String sTopicBase;                   // "path" of topics
  String sTopicBaseDefault;            // default "path" of topics
  String aTopicGet[TOPIC_MAX];         // get topics
  String aTopicSet[TOPIC_MAX];         // set topics
  String aTopicSub[TOPIC_MAX];         // sub topics
  String aTopicPub[TOPIC_MAX];         // pub topics
  String aPayloadSet[TOPIC_MAX];       // payload for set topics
  String aPayloadRet[TOPIC_MAX];       // payload for ret topics
  String aPayloadSub[TOPIC_MAX];       // payload for sub topics
  String aPayloadPub[TOPIC_MAX];       // payload for pub topics

 //------constructor & co---------------------------------------
 public:
  //_____constructor, use default values________________________
  SimpleMqtt();
  SimpleMqtt(char* ssid, char* pwd);
//SimpleMqtt(char* ssid, char* pwd, char* mqtt_server);
  SimpleMqtt(char* ssid, char* pwd, char* mqtt_server, char* topicbase);
  SimpleMqtt(String sssid, String spwd, String smqtt_server);
 
 protected:
  //_____initialize all properties. Called by constructor(s)____
  void   setup();                      // called by constructors
  bool   begin(int iSource);           // called by constructors
 public:
  //_____init objects, read topic base from eeprom (use default on error)
  bool   begin();                      // init objects
 
 //------setter and getter methods------------------------------
 public:
  //_____set language (default e=english; d=german)_____________
  void   setLanguage(char language);
  //_____set (used) EEPROM size_________________________________
  void   setEepromSize(int eepromSize);
  //_____Maximum milliseconds to wait for WiFi to be connected__
  void   setWiFiWaitingTime(int ms);
  //_____number of connectingWiFi() after connectingWiFiBegin()_
  void   setWiFiConnectingCounter(int number);
  //_____set MQTT client name___________________________________
  void   setMQTTClientName(String sName) {sMQTTClientName=sName;};
  //_____get MQTT client name___________________________________
  String getMQTTClientName() { return sMQTTClientName; };
  //_____get MQTT client state as string________________________
  String getsState();
  //_____client name of WiFi network____________________________
  String getsSSID();
  //_____client IP address as string____________________________
  String getsMyIP();
  //_____get D1mini MAC address as string_______________________
  String getsMac();
  //_____get "path" for mqtt messages___________________________
  String getsTopicBase();
  
 //------methods to declare mqtt topics-------------------------
  //_____set "path" for MQTT commands (+eeprom), NO subscribe___
  int    setTopicBase(String topicBasis);
  //_____set default "path" for MQTT commands (+eeprom)_________
  int    setTopicBaseDefault(String topicBasisDefault);
  //_____set all (comma separated) topics_______________________
  void   setTopics(String sAllGet, String sAllSet, String sAllSub, String sAllPub);
  //_____set all get-topics as comma separated strings__________
  int    setTopicGet(String sAllGet);
  //_____set all set-topics as comma separated strings__________
  int    setTopicSet(String sAllSet);
  //_____set all sub(scribe)-topics as comma separated strings__
  int    setTopicSub(String sAllSub);
  //_____set all pub(lish)-topics as comma separated strings____
  int    setTopicPub(String sAllPub);

 //------methods for Wifi (WLAN)--------------------------------
  //_____try to connect to WiFi, wait max. wifiWaitMsMax________
  bool   connectWiFi();                // calls connectingWiFi*
  //_____start a new WiFi connection____________________________
  bool   connectingWiFiBegin();        // start new connecting
  //_____wait for WiFi connection to be established_____________
  bool   connectingWiFi();             // try to connect + wait
  //_____wait n times for WiFi connection to be established_____
  bool   connectingWiFi(int attempts); // try n times connect+wait
  //_____disconnect a WiFi connection___________________________
  bool   disconnectWiFi();             // disconnect
  //_____is client connected to WiFi (WLAN)?____________________
  bool   isWiFiConnected();            // is connected to WiFi
  //_____is WiFi new connected?_________________________________
  bool   isWiFiConnectedNew();         // is new WiFi connection
  //_____Wifi lost?_____________________________________________
  bool   isWiFiDisconnectedNew();      // WiFi lost?
  //_____get WiFi signal strength (or ? on error)_______________
  String getsSignal();                 // WiFi signal strength

 //------methods for mqtt connection----------------------------
  //_____wait for WiFi connection, then connect to MQTT server__
  bool   connectWiFiMQTT();            // connect MQTT, wait
  //_____try to connect to MQTT server__________________________
  bool   connectMQTT();                // connect to MQTT broker
  //_____is mqtt connection ok? (no: reconnect)_________________
  bool   checkMQTT();                  // called by doLoop() !
  //_____is mqtt connection ok?_________________________________
  bool   isMQTTConnected();            // is connected to MQTT
  //_____is MQTT connection new?________________________________
  bool   isMQTTConnectedNew();         // is new connected MQTT
  //_____MQTT connection lost?__________________________________
  bool   isMQTTDisconnectedNew();      // is new connected MQTT

 //------mqtt (main) loop control-------------------------------
 //--> One of these methods must be called  in loop() function!!
  //_____control mqtt in main loop (with reconnect)_____________
  bool   doLoop(void);                 // calls checkMQTT();
  //_____control mqtt in main loop (without reconnect!)_________
  bool   doLoop(bool tryToReconnect);  // calls checkMQTT();
  
 //-----internal callback method--------------------------------
 //_____internal callback method________________________________
 //      ---> !!! MUST be called by global callback routine !!!
  void   callback_(char* topic, byte* payload, unsigned int length);

 //------mqtt working methods----------------------------------- 
  //_____set new topic base, unsubscribe old topics, subscribe new topics
  bool   changeTopicBase(String newBase);
  //_____set new topic base, unsubscribe old topics, subscribe new topics
  bool   changeTopicBase(String oldBase, String newBase);
  //_____prepare to send a message with topic out of aPayloadPub
  void   sendPubIndex(int index, String payload);
  //_____force (simulate) a get-, set-, sub- or pub-message_____
  bool   simpleMqttDo(String type, String topic, String payload);

 //------connection state---------------------------------------
  //_____is error bit set?______________________________________
  bool   isConnectError();
  //_____clear connection error bit_____________________________
  void   resetConnectError();
  //_____convert conState to unsigned long number_______________
  unsigned long getConState();
  //_____convert conState to HEX string_________________________
  String getConStateHex();
 
 //------helper methods-----------------------------------------
  //_____subscribe get-, set-, sub-topic________________________
  bool   subscribeAllTopics();                   // 
  //_____subscribe get-, set-, sub-topic for given topic________
  bool   subscribeAllTopics(String topicBasis);  // 
  //_____unsubscribe get-, set-, sub-topic______________________
  bool   unsubscribeAllTopics();                 // 
  //_____unsubscribe get-, set-, sub-topic______________________
  bool   unsubscribeAllTopics(String topicBasis);// 
  //_____unsubscribe old, subscribe new topic base______________
  int    changeSubscribe(String oldTopic, String newTopic);
  //_____send answers to get-/set-requests______________________
  void   sendRet();                    // send iXXX >0 topics
  //_____convert Array to json format___________________________
  String jsonArray(String what);
  //_____split string to array 1________________________________
  int    splitString(String str, String aStr[]);
  //_____split string to array 2________________________________
  int    splitString(String str, String aStr[], String delimiter);
  //_____split string to array 3_________________________________
  int    splitString(String str, String aStr[], String delimiter, int imax);

 //------internal methods---------------------------------------
  //_____generate get answers in array aPayloadRet[]____________
  void   createGetAnswer();

 //------methods for eeprom read/write--------------------------
  //_____eeprom return value as string__________________________
  String getsEepromStatus(int iResult);
  //_____write topic to eeprom as topicBase_____________________
  int    eepromWriteTopicBase(String topic);
  //_____read topic base from eeprom____________________________
  String eepromReadTopicBase(int& iResult);
  //_____read topic base from eeprom, ignore result_____________
  String eepromReadTopicBase();
  //_____erase identifier of topicBase__________________________
  bool   eepromEraseTopicBase();
  //_____write string data to eeprom (after topicBase)__________
  int    eepromWriteMyData(String sData);
  //_____read string data from eeprom___________________________
  String eepromReadMyData(int& iResult);
  //_____read string data from eeprom, ignore result____________
  String eepromReadMyData();
  //_____erase string data from eeprom__________________________
  bool   eepromEraseMyData();
  //_____read a block from eeprom_______________________________
  size_t eepromReadBlock(char* data, unsigned long address,
         unsigned long len);
  //_____write a block to eeprom________________________________
  size_t eepromWriteBlock(char* data, unsigned long address,
         unsigned long len);
};
#endif
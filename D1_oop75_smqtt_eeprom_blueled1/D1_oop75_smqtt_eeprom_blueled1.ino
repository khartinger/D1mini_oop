//_____D1_oop75_smqtt_eeprom_blueled1_________200705-201016_____
// Demo program to show the use of class SimpleMqtt.
// Function of this program:
// [1] Connect to given WiFi and MQTT broker
// [2] Use "topic base" stored in eeprom or topic led/1
// [3] Answer the following requests with a topic like
//     led/1/ret/...:
//     -t led/1/get -m topicbase
//     -t led/1/get -m eeprom
//     -t led/1/get -m blueled
// [4] Change topic base (e.g. to led/2)
//     -t led/1/set/topicbase -m led/2
// [5] Set D1mini blue led off, on or toggle
//     -t led/1/set/blueled -m 0
//     (plus message -t led/3/set/blueled -m toggle)
// [6] Erase topic base stored in EEPROM by
//     -t led/1/set/eeprom -m erase (or -m 0)
//     On next start topic base is set to default value led/1
// [7] Automatic (build in) answers for messages 
//     -t led/1/get -m help
//     -t led/1/get -m version
//     -t led/1/get -m ip
// Class SimpleMqtt extends class PubSubClient for easy use.
// All commands of the PubSubClient class can still be used.
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Hardware: 
// (1) WeMos D1 mini (OR ESP32 D1 mini)
//
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, October 16, 2020.
// Changes:
// 2020-10-16 New
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG75        true //false
#define  VERSION75      "2020-10-16 D1_oop75_smqtt_eeprom_blueled1"
#define  TOPIC_BASE     "led/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,blueled"
#define  TOPIC_SET      "topicbase,eeprom,blueled"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      "led/3/set/blueled"

//_____sensors, actors, global variables________________________
#if defined(ESP32) || defined(ESP32D1)
 #define BLUELED_PIN    2                   // led pin 
 #define BLUELED_ON     1                   // ESP32 1
 #define BLUELED_OFF    0                   // ESP32 0
#else
 #define BLUELED_PIN    D4                  // led pin D4=IO2
 #define BLUELED_ON     0                   // D1mini 0
 #define BLUELED_OFF    1                   // D1mini 1
#endif
int      ledVal_=BLUELED_OFF;               // pin value

//_____MQTT communication_______________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1", TOPIC_BASE);

//_____MQTT: inspect all subscribed incoming messages___________
void callback(char* topic, byte* payload, unsigned int length)
{
 client.callback_(topic, payload, length);  // must be called!
}

//_____answer get requests______________________________________
// sPayload: payload to message from TOPIC_GET
// auto answer: for help (+), version, ip (can be overwritten)
// return: ret answer payload for get request
String simpleGet(String sPayload)
{
 if(sPayload=="version") return String(VERSION75);
 //-------------------------------------------------------------
 if(sPayload=="topicbase") return client.getsTopicBase();
 //-------------------------------------------------------------
 if(sPayload=="eeprom") {
  int iResult;
  String s1=client.eepromReadTopicBase(iResult);
  if(iResult==-2) return String("Error: No topic base stored");
  if(iResult<0) return String("Error # ")+String(iResult);
  return s1;
 }
 //-------------------------------------------------------------
 if(sPayload=="blueled") {
  if(ledVal_==BLUELED_ON) return String("on"); 
  else return String("off"); 
 }
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_____execute set requests_____________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {                  // new topic base?
  int ret=client.changeTopicBase(sPayload); // change base
  return client.getsTopicBase();            // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom") {                     // erase eeprom?
  if(sPayload=="0" || sPayload=="erase")    // payload OK?
  if(client.eepromEraseTopicBase()) return "erased";
  else return "not erased";                 // return answer
 }
 //-------------------------------------------------------------
 if(sTopic=="blueled") {                    // switch blue led?
  if(sPayload=="on" || sPayload=="1") ledVal_=BLUELED_ON;
  else {                                    // other command
   if(sPayload=="off" || sPayload=="0") ledVal_=BLUELED_OFF;
   else ledVal_=1-ledVal_;                  // toggle led
  }
  digitalWrite(BLUELED_PIN,ledVal_);        // turn led on/off
  client.sendPubIndex(0, "toggle");         // additional message
  if(ledVal_==BLUELED_ON) return String("on"); // return answer
  return String("off");                     // return answer
 }
 //-------------------------------------------------------------
 return String("");                         // wrong set command
}

//_____execute sub requests_____________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
}

//_____SETUP____________________________________________________
void setup() {
  //-----Serial, just for debug----------------------------------
 //if(DEBUG75)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //-----init pins-----------------------------------------------
 pinMode(BLUELED_PIN, OUTPUT);              // set pin to output
 digitalWrite(BLUELED_PIN,ledVal_);         // turn led on/off
 //-----prepare mqtt start--------------------------------------
 // Use setTopicBaseDefault() (or constructor) instead of 
 // setTopicBase(), to use topic base from eeprom on restart ;)
 setTopicBaseDefault(TOPIC_BASE);
 client.begin();                            // setup objects
 client.setCallback(callback);              // mqtt receiver
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 //-----connect to WiFi and MQTT broker-------------------------
 while(!client.connectWiFiMQTT());
 Serial.println("setup(): topicBase="+client.getsTopicBase());
 Serial.println("setup(): --Finished--");
}

//_____LOOP_____________________________________________________
void loop() {
 client.doLoop();                           // mqtt loop
 delay(100);
}

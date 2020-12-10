//_____D1_oop76_smqtt_stm_demo________________200705-201103_____
// Demo program to show the use of class SimpleMqtt.
// Function of this program:
// [1] Use a state machine
// [2] Connect to given WiFi and MQTT broker
// [3] Use topic base stored in eeprom or "simplemqtt"
// [4] Answer the following requests with a topic like
//     simplemqtt/ret/...:
//     -t simplemqtt/get -m topicbase
//     -t simplemqtt/get -m eeprom
//     -t simplemqtt/get -m blueled
//     -t simplemqtt/get -m value1
// [5] Set new topic base (also stored in eeprom):
//     -t simplemqtt/set/topicbase -m new_base
// [6] Erase stored topic base (-> use default on restart)
//     -t simplemqtt/set/eeprom -m erase
// [7] Set a new value to demo variable
//     -t simplemqtt/set/value1 -m new_value
// [8] Set the blue led by an "external" request
//     -t led/3/setled -m 1
//     with answer led/3/led_is on|off
// [9] Send a version message every minute
// [10] Automatic (build in) answers for messages 
//     -t simplemqtt/get -m help
//     -t simplemqtt/get -m version
//     -t simplemqtt/get -m ip
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
// 2020-11-03 New
// Released into the public domain.
//#define D1MINI          1              // ESP8266 D1mini +pro
#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/statemachine/D1_class_Statemachine.h"

#define  DEBUG76        true //false
#define  VERSION76      "2020-11-03 D1_oop76_smqtt_stm_demo1"
#define  TOPIC_BASE     "simplemqtt"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,blueled,value1"
#define  TOPIC_SET      "topicbase,eeprom,value1"
#define  TOPIC_SUB      "led/3/setled"
#define  TOPIC_PUB      "led/3/led_is"

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
String   value1="unknown1";

//_____MQTT communication_______________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1");

//_____state machine____________________________________________
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
Statemachine stm(STATE_MAX, STATE_DELAY); //1..600

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
 //-------------------------------------------------------------
 if(sPayload=="version") return String(VERSION76);
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
 if(sPayload=="value1") return value1;
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_____execute set requests_____________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
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
 if(sTopic=="value1") {                     // set value1
  value1=sPayload;
  return value1;
 }
 //-------------------------------------------------------------
 return String("");                         // wrong set command
}

//_____execute sub requests_____________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
 if(sTopic=="led/3/setled") {               // switch blue led
  if(sPayload=="on" || sPayload=="1") ledVal_=BLUELED_ON;
  else {                                    // other command
   if(sPayload=="off" || sPayload=="0") ledVal_=BLUELED_OFF;
   else ledVal_=1-ledVal_;                  // toggle led
  }
  digitalWrite(BLUELED_PIN,ledVal_);        // turn led on/off
  if(ledVal_==BLUELED_ON) client.sendPubIndex(0,"on");
  else client.sendPubIndex(0,"off");
  return;
 }
 //-------------------------------------------------------------
}

//_____SETUP____________________________________________________
void setup() {
 //-----Serial, just for debug----------------------------------
 //if(DEBUG76)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //-----init pins-----------------------------------------------
 pinMode(BLUELED_PIN, OUTPUT);              // set pin to output
 digitalWrite(BLUELED_PIN,ledVal_);         // turn led on/off
 //-----setup mqtt parameter------------------------------------
 // Use setTopicBaseDefault() (or constructor) instead of 
 // setTopicBase(), to use topic base from eeprom on restart ;)
 client.setTopicBaseDefault(TOPIC_BASE);    // set default base
 //client.setTopicBase(TOPIC_BASE);           // fix topic base
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();
 client.setCallback(callback);
 //-----show connecting procedure via Serial (just for fun)-----
 //-----connect to WiFi-----------------------------------------
 client.setWiFiWaitingTime(1000);           // set to 1 second
 client.setWiFiConnectingCounter(10);       // try 10s connecting
 client.connectingWiFiBegin();              // start connecting
 while(!client.connectingWiFi()) {          // connect to WiFi
 if(DEBUG76) {Serial.println("setup(): No WiFi connection"); delay(1000);}
 };
 Serial.println("setup(): WiFi connected, IP: "+client.getsMyIP());
 //-----connect to MQTT broker----------------------------------
 if(client.connectMQTT())
  Serial.println("setup(): Connected to MQTT broker!");
 else
  Serial.println("setup(): NOT connected to MQTT broker!");
 //-----setup finish--------------------------------------------
 Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 int state=stm.loopBegin();            // state begin 
 if(state==1) client.simpleMqttDo("get","version","");
 client.doLoop();                     // mqtt loop
  //-----just for demo: info, if connection is new---------------
 if(client.isWiFiConnectedNew()) Serial.println("loop(): New WiFi connection");
 if(client.isMQTTConnectedNew()) Serial.println("loop(): New MQTT connection");
 stm.loopEnd();                        // state end
}

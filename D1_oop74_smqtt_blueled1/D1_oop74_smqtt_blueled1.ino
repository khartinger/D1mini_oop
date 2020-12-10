//_____D1_oop74_smqtt_blueled1________________200705-201025_____
// Demo program to show the use of class SimpleMqtt.
// Function of this program:
// [1] Connect to given WiFi and MQTT broker
// [2] Use "topic base" led/1
// [3] Wait for topic led/1/get with payload blueled e.g.
//     -t led/1/get -m blueled
//     answer it with led/1/ret/blueled and payload on or off
// [4] Wait for message (topic) led/1/set/blueled with 
//     payload on (1) or off (0), e.g.
//     -t led/1/set/blueled -m 1
//     switch led and
//     answer it with led/1/ret/blueled and payload on or off
// [5] Automatic (build in) answers for messages 
//     -t led/1/get -m help
//     -t led/1/get -m version
//     -t led/1/get -m ip
// Class SimpleMqtt extends class PubSubClient for easy use.
// All commands of the PubSubClient class can still be used.
//
// Hardware: 
// (1) WeMos D1 mini (OR ESP32 D1 mini)
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, October 25, 2020.
// Changes:
// 2020-10-25 New
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini

#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG74        true //false
#define  VERSION74      "2020-10-25 D1_oop74_smqtt_blueled1"
#define  TOPIC_BASE     "led/1"
#define  TOPIC_GET      "help,version,ip,blueled"
#define  TOPIC_SET      "blueled"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""

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
 if(sPayload=="version") return String(VERSION74);
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
 if(sTopic=="blueled") {                    // switch blue led?
  if(sPayload=="on" || sPayload=="1") ledVal_=BLUELED_ON;
  else {                                    // other command
   if(sPayload=="off" || sPayload=="0") ledVal_=BLUELED_OFF;
  }
  digitalWrite(BLUELED_PIN,ledVal_);        // turn led on/off
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
 //if(DEBUG74)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //-----init pins-----------------------------------------------
 pinMode(BLUELED_PIN, OUTPUT);              // set pin to output
 digitalWrite(BLUELED_PIN,ledVal_);         // turn led on/off
 //-----prepare mqtt start--------------------------------------
 client.setCallback(callback);              // mqtt receiver
 client.setTopicBase(TOPIC_BASE);           // set base, eeprom
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();                            // setup objects
 Serial.println("setup(): topicBase="+client.getsTopicBase());
 //-----connect to WiFi and MQTT broker-------------------------
 while(!client.connectWiFiMQTT());
 Serial.println("setup(): --Finished--");
}

//_____LOOP_____________________________________________________
void loop() {
 //-----process mqtt actions------------------------------------
 int ret=client.doLoop();              // mqtt loop
 //-----just for demo: info, if connection is new---------------
 if(client.isWiFiConnectedNew()) Serial.println("loop(): New WiFi connection");
 if(client.isMQTTConnectedNew()) Serial.println("loop(): New MQTT connection");
 delay(100);
}

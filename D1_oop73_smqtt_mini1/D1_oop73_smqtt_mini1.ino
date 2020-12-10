//_____D1_oop73_smqtt_mini1___________________200705-201208_____
// Simple demo program to show the use of class SimpleMqtt.
// Function of this program:
// [1] Connect to given WiFi and MQTT broker
// [2] Use default "topic base" simplemqtt/default
// [3] Answer the following requests with a topic like
//     simplemqtt/default/ret/...:
//     -t simplemqtt/default/get -m help
//     -t simplemqtt/default/get -m version
//     -t simplemqtt/default/get -m ip
//     -t simplemqtt/default/get -m value1
// [4] Store a value:
//     -t simplemqtt/default/set/value1 -m 123
// Class SimpleMqtt extends class PubSubClient for easy use.
// You can use all commands of class PubSubClient as well.
// When PubSubClient lib is installed,
// delete PubSubClient files in directory src/simplemqtt
// Hardware: 
// (1) WeMos D1 mini OR ESP32 D1 mini
//
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, December 08, 2020.
// Changes:
// 2020-12-08 New
// Released into the public domain.

#include "src/simplemqtt/D1_class_SimpleMqtt.h"

//#define  D1MINI         1              // ESP8266 D1mini +pro
#define  ESP32D1        2              // ESP32 D1mini
#define  DEBUG73        true //false
#define  VERSION73      "2020-12-08 D1_oop73_smqtt_mini1"
#define  TOPIC_GET      "help,version,ip,value1"
#define  TOPIC_SET      "value1"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""

//_____sensors, actors, global variables________________________
String   value1="unknown1";

//_____MQTT communication_______________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1");

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
 if(sPayload=="version") return String(VERSION73);
 if(sPayload=="value1") return value1;
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_____execute set requests_____________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
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
}

//_____SETUP____________________________________________________
void setup() {
 //-----Serial, just for debug----------------------------------
 //if(DEBUG73)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //-----prepare mqtt start--------------------------------------
 client.eepromEraseTopicBase();        // delete saved base
 client.setCallback(callback);              // mqtt receiver
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();                            // setup objects
 Serial.println("setup(): topicBase="+client.getsTopicBase());
 //-----connect to WiFi and MQTT broker-------------------------
 while(!client.connectWiFiMQTT()) delay(500);
 Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 client.doLoop();                      // mqtt loop
 delay(100);
}

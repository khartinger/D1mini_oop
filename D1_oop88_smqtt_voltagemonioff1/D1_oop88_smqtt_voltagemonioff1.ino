//_____D1_oop88_smqtt_voltagemonioff1.ino__________20210220_____
// After switching on the power supply, the D1 mini sends the
// MQTT message 'voltage/1/ret/status' with payload '1'.
// If the supply voltage is switched off, it sends the same
//   message with payload '0'.
// Only if power supply via USB (test phase):
//   If supply voltage seems to be off (D5=0) and the relay is
//   "NC" (D1=0) the same message with payload '-1' is sent.
// In addition, it is possible to switch the consumer voltage 
//   off and on via MQTT. To switch on again with the main 
//   switch, it only has to be briefly switched off once and 
//   then switched on again (reset of the D1 mini).
// Note: Connect consumer to NC contact of relay!
//       (relay off = consumer on to save energy)
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. 5V power supply with voltage divider and backup capacitor
// 3. D1 mini relay shield
//
// Created by Karl Hartinger, February 14, 2021.
// Changes: 
// 2021-02-14 First release
// 2021-02-20 use sTopic_PUB_SUB instead of TOPIC_SUB, TOPIC_PUB
//            split "status" into the parts din5v and consumer
// Released into the public domain.
#include <Arduino.h>
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG88        true //false //true
#define  VERSION88      "2021-02-20 D1_oop88_smqtt_voltagemonioff1"
#define  TOPIC_BASE     "voltage/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,status,relay,consumer"
#define  TOPIC_SET      "topicbase,eeprom,relay,consumer"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""
//-----special topic for return status message (PUB/SUB)--------
String sTopic_PUB_SUB=String(TOPIC_BASE)+"/ret/status";

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          1             // LED on
 #define LED_OFF         0             // LED off
 #define PIN_DIN_5V     18             // D5 -> 18
 #define PIN_RELAY      22             // D1 -> 22
#else
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          0             // LED on
 #define LED_OFF         1             // LED off
 #define PIN_DIN_5V     D5             // D5 -> 18
 #define PIN_RELAY      D1             // D1 -> 22
#endif
#define CONSUMER_ON     0              // lamp on=relay off
#define CONSUMER_OFF    1              // lamp off=relay on
int din5V=1;                           // value of PIN_DIN_5V
int din5VLast=-99;                     // no last value of PIN_DIN_5V
int consumer=CONSUMER_ON;              // 0=relay consumer on
int consumerLast=consumer;             // to detect a change

//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1");

//_______MQTT: inspect all subscribed incoming messages_________
void callback(char* topic, byte* payload, unsigned int length)
{
 client.callback_(topic, payload, length);  // must be called!
}

//_______answer get requests____________________________________
// sPayload: payload to message from TOPIC_GET
// auto answer: for help (+), version, ip (can be overwritten)
// return: ret answer payload for get request
String simpleGet(String sPayload)
{
 //-------------------------------------------------------------
 if(sPayload=="version") return String(VERSION88);
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
 if(sPayload=="status" || sPayload=="consumer") 
  return sCalcStatus();
 //-------------------------------------------------------------
 if(sPayload=="relay") return String(1-consumer);
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {                  // new topic base?
  sTopic_PUB_SUB=sPayload+"/ret/status";    // new status topic
  client.setTopicPub(sTopic_PUB_SUB);       // new status topic
  client.setTopicSub(sTopic_PUB_SUB);       // new status topic
  client.changeTopicBase(sPayload);         // change base
  String s1=client.getsTopicBase();         // read new base
  return s1;                                // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom") {                     // erase eeprom?
  if(sPayload=="0" || sPayload=="erase") {  // payload OK?
   if(client.eepromEraseTopicBase()) return "erased";
  }
  else return String("not erased");         // return answer
 }
 //-------------------------------------------------------------
 if(sTopic=="relay") { //....set relay..........................
  if(sPayload=="1" || sPayload=="on") consumer=CONSUMER_OFF;
  else
  {
   if(sPayload=="0" || sPayload=="off") consumer=CONSUMER_ON;
   else {
    if(sPayload=="-1" || sPayload=="toggle") consumer=1-consumer;
    else return String("error");
   }
  }
  digitalWrite(PIN_RELAY,consumer);         // switch relay
  return String(consumer);
 }
 //-------------------------------------------------------------
 if(sTopic=="consumer") { //....set consumer (relay)............
  if(sPayload=="1" || sPayload=="on") consumer=CONSUMER_ON;
  else
  {
   if(sPayload=="0" || sPayload=="off") consumer=CONSUMER_OFF;
   else {
    if(sPayload=="-1" || sPayload=="toggle") consumer=1-consumer;
    else return String("error");
   }
  }
  digitalWrite(PIN_RELAY,consumer);         // switch relay
  if(consumer==CONSUMER_ON) return String("on");
  else return String("off");
 }
 //-------------------------------------------------------------
 return String("");                         // wrong set command
}

//_______execute sub requests___________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
}

//_______calculate status as string_____________________________
// uses: global variables consumer and din5V
// return    || consumer  |
// value     || off | on* |   * normally is consumer=on
// ==========++=====+=====|
// din5V = 0 || -2  |  0  |
// din5V = 1 || -1  |  1  |
String sCalcStatus()
{
 int status_=0;
 if(consumer==CONSUMER_ON) {
  if(din5V==1) status_=1; else status_=0;
 } else {
  if(din5V==1) status_=-1; else status_=-2;
 }
 return String(status_);
}

//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 Serial.begin(115200);
 if(DEBUG88) Serial.println("\nsetup(): --Start--");
 //------set pin mode-------------------------------------------
 pinMode(PIN_LED, OUTPUT);                  // blue led
 pinMode(PIN_DIN_5V, INPUT);                // 5V sensor
 pinMode(PIN_RELAY, OUTPUT);                // relay output
 int led_=LED_ON;                           // start: set...
 digitalWrite(PIN_LED,led_);                // ...led on
 digitalWrite(PIN_RELAY,consumer);          // relay off -> NC
 //------get topic base from eeprom, build status topic---------
 int iResult;
 String s1=client.eepromReadTopicBase(iResult);
 if(iResult>=0) sTopic_PUB_SUB=s1+"/ret/status";
 //------prepare mqtt start-------------------------------------
 client.setCallback(callback);              // mqtt receiver
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,sTopic_PUB_SUB,sTopic_PUB_SUB);
 client.setRetainedIndex("get",5,true);     // 5=status
 client.setRetainedIndex("pub",0,true);     // 0=ret/status
 client.begin();                            // setup objects
 if(DEBUG88) Serial.println("setup(): topicBase="+client.getsTopicBase());
 //------connect to WiFi and MQTT broker------------------------
 while(!client.connectWiFiMQTT()) {
  if(DEBUG88) Serial.println("setup(): Connecting error");
  for(int i=0; i<10; i++) {
   led_=1-led_;                             // invert led
   digitalWrite(PIN_LED,led_);              // switch led
   delay(100);
  }
 }
 //------finish setup-------------------------------------------
 digitalWrite(PIN_LED,LED_OFF);             // connected=led off
 if(DEBUG88) Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 din5V = digitalRead(PIN_DIN_5V);
 //------detect a 5V change or relay change via MQTT------------
 if(consumer!=consumerLast || din5V!=din5VLast)
 {
  if(DEBUG88) Serial.printf("New status=%s\n",sCalcStatus().c_str());
  consumerLast=consumer;                    // change detected
  din5VLast=din5V;                          // change detected
  digitalWrite(PIN_RELAY,consumer);         // switch relay
  client.sendPubIndex(0,sCalcStatus());     // send status 0
 }
 //------process MQTT message(s)--------------------------------
 client.doLoop();                           // mqtt loop
 delay(50);                                 // wait 0.05s
}

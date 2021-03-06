//_____D1_oop87_smqtt_voltagemonitoring1.ino_______20210220_____
// After switching on the power supply, the D1 mini sends the
//   MQTT message 'voltage/1/ret/status' with payload '1'.
// If the supply voltage is switched off, it sends the same
//   message with payload '0'.
// Blue LED on: connecting | LED off: connected to MQTT broker
// In DEBUG mode, the time until the message is sent and until
//   the same message arrives back at the D1mini is also 
//   measured and displayed via Serial (usually 52ms or 104ms).
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. 5V power supply with voltage divider and backup capacitor
//
// Created by Karl Hartinger, February 13, 2021.
// Changes: 
// 2021-02-13 First release
// 2021-02-20 use sTopic_PUB_SUB instead of TOPIC_SUB, TOPIC_PUB
//            use statusLast instead of counter
// Released into the public domain.
#include <Arduino.h>
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG87        false // true  //false
#define  VERSION87      "2021-02-20 D1_oop87_smqtt_voltagemonitoring1"
#define  TOPIC_BASE     "voltage/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,status"
#define  TOPIC_SET      "topicbase,eeprom"
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
#else
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          0             // LED on
 #define LED_OFF         1             // LED off
 #define PIN_DIN_5V     D5             // D5 -> 18
#endif
int status;                            // state of PIN_DIN_5V
int statusLast=-99;                    // no last state

//-------for DEBUG use------------------------------------------
#if defined(DEBUG87)
 uint32_t ms_start=millis();           // save the start time
 bool ballow_new_ms=true;              // allow a new start time
 bool bsend_ms=false;                  // send time via Serial
#endif

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
 if(sPayload=="version") return String(VERSION87);
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
 if(sPayload=="status") return String(status);
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
  else return "not erased";                 // return answer
 }
 //-------------------------------------------------------------
 return String("");                         // wrong set command
}

//_______execute sub requests___________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
 if(DEBUG87) 
 { 
  if(sTopic==sTopic_PUB_SUB)
  {
   Serial.print("simpleSub(): ");
   Serial.println(millis()-ms_start);
   ballow_new_ms=true;
  }
 }
}

//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 Serial.begin(115200); 
 if(DEBUG87) Serial.println("\nsetup(): --Start--");
 //------set pin mode-------------------------------------------
 pinMode(PIN_LED, OUTPUT);                  // blue led
 pinMode(PIN_DIN_5V, INPUT);                // 5V sensor
 int led_=LED_ON;                           // help value
 digitalWrite(PIN_LED,led_);                // led on
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
 if(DEBUG87) Serial.println("setup(): topicBase="+client.getsTopicBase());
 //------connect to WiFi and MQTT broker------------------------
 while(!client.connectWiFiMQTT()) {
  if(DEBUG87) Serial.println("setup(): Connecting error");
  for(int i=0; i<10; i++) {
   led_=1-led_;                             // invert led
   digitalWrite(PIN_LED,led_);              // switch led
   delay(100);
  }
 }
 //------finish setup-------------------------------------------
 digitalWrite(PIN_LED,LED_OFF);             // connected=led off
 if(DEBUG87) Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 if(DEBUG87) { 
  bsend_ms=false;
  if(ballow_new_ms) ms_start=millis(); 
 }
 status = digitalRead(PIN_DIN_5V);
 //------detect a 5V change-------------------------------------
 if(status!=statusLast)
 {
  statusLast=status;
  client.sendPubIndex(0,String(status));   // send status 0
  if(DEBUG87) {bsend_ms=true; ballow_new_ms=false;} //send Serial
 }
 //------process MQTT message(s)--------------------------------
 client.doLoop();                           // mqtt loop
 delay(50);                                 // wait 0.05s
 if(DEBUG87) {if(bsend_ms){Serial.print("loop(): ");Serial.println(millis()-ms_start);}}
}

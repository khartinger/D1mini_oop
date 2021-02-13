//_____D1_oop87_smqtt_voltagemonitoring1.ino_______20210213_____
// After switching on the power supply, the D1 mini sends the
// MQTT message 'voltage/1/ret/status' with payload '1'.
// If the supply voltage is switched off, it sends the same
// message with payload '0'.
// In DEBUG mode, the time until the message is sent and until
// the same message arrives back at the D1mini is also 
// measured and displayed via Serial (usually 52ms or 104ms).
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. 5V power supply with voltage divider and backup capacitor
//
// Created by Karl Hartinger, February 13, 2021.
// Changes: 
// 2021-02-13 First release
// Released into the public domain.
#include <Arduino.h>
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG87        false //true  //false
#define  VERSION87      "2021-02-13 D1_oop87_smqtt_voltagemonitoring1"
#define  TOPIC_BASE     "voltage/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,status"
#define  TOPIC_SET      "topicbase,eeprom"
#define  TOPIC_SUB      TOPIC_BASE "/ret/status"
#define  TOPIC_PUB      TOPIC_BASE "/ret/status"

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          1             // LED on
 #define LED_OFF         0             // LED off
 #define PIN_DIN        18             // D5 -> 18
#else
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          0             // LED on
 #define LED_OFF         1             // LED off
 #define PIN_DIN        D5             // D5 -> 18
#endif
int status=1;                          // state of PIN_DIN
int counter=0;                         // if PIN_DIN is 0

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
  String s1=String(TOPIC_BASE)+String("/ret/status");
  if(sTopic==s1)
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
 if(DEBUG87) {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------set pin mode-------------------------------------------
 pinMode(PIN_LED, OUTPUT);
 pinMode(PIN_DIN, INPUT);
 int led_=LED_OFF;                          // help value
 digitalWrite(PIN_LED,led_);                // led off
 //------prepare mqtt start-------------------------------------
 client.setCallback(callback);              // mqtt receiver
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.setRetainedIndex("get",5,true);     // 5=status
 client.begin();                            // setup objects
 Serial.println("setup(): topicBase="+client.getsTopicBase());
 //------connect to WiFi and MQTT broker------------------------
 while(!client.connectWiFiMQTT()) {
  delay(500);
 }
 digitalWrite(PIN_LED,LED_ON);              // connected=led on
 //------send status--------------------------------------------
 client.sendPubIndex(0,String(status));     // 1st message
 client.doLoop();                           // mqtt loop
 Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 if(DEBUG87) { 
  bsend_ms=false;
  if(ballow_new_ms) ms_start=millis(); 
 }
 status = digitalRead(PIN_DIN);
 if(status==0)
 {//-----supply voltage down------------------------------------
  if(counter==0)
  {//----send mqtt message--------------------------------------
   client.sendPubIndex(0,String(status));
   counter=300;                             // next send in 15s
   if(DEBUG87) {bsend_ms=true; ballow_new_ms=false;} // allow sending
  }
  else {//----mqtt message sent, wait for next time-------------
   counter--;
  }
 }
 client.doLoop();                           // mqtt loop
 delay(50);                                 // wait 0.05s
 if(DEBUG87) { if(bsend_ms) Serial.println(millis()-ms_start); }
}

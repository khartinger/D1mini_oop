//_____D1_oop88_smqtt_voltagemonioff1.ino__________20210214_____
// After switching on the power supply, the D1 mini sends the
// MQTT message 'voltage/1/ret/status' with payload '1'.
// If the supply voltage is switched off, it sends the same
// message with payload '0'.
// In addition, it is possible to switch the consumer voltage 
// off and on via MQTT. To switch on again with the main switch,
// it only has to be briefly switched off once and then 
// switched on again (reset of the D1 mini).
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. 5V power supply with voltage divider and backup capacitor
// 3. D1 mini relay shield
//
// Created by Karl Hartinger, February 14, 2021.
// Changes: 
// 2021-02-14 First release
// Released into the public domain.
#include <Arduino.h>
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG88        false //true  //false
#define  VERSION88      "2021-02-14 D1_oop88_smqtt_voltagemonioff1"
#define  TOPIC_BASE     "voltage/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,status,relay"
#define  TOPIC_SET      "topicbase,eeprom,relay"
#define  TOPIC_SUB      TOPIC_BASE "/ret/status"
#define  TOPIC_PUB      TOPIC_BASE "/ret/status"

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          1             // LED on
 #define LED_OFF         0             // LED off
 #define PIN_DIN        18             // D5 -> 18
 #define PIN_RELAY      22             // D1 -> 22
#else
 #define PIN_LED         2             // D4=IO2
 #define LED_ON          0             // LED on
 #define LED_OFF         1             // LED off
 #define PIN_DIN        D5             // D5 -> 18
 #define PIN_RELAY      D1             // D1 -> 22
#endif
#define CONTACT_ON      0              // lamp on=relay off
#define CONTACT_OFF     1              // lamp off=relay on
int status=1;                          // state of PIN_DIN
int statusLast=status;                 // to detect a change
int counter=0;                         // if PIN_DIN is 0
int contact=CONTACT_ON;                // 0=relay contact on
int contactLast=contact;               // to detect a change

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
 if(sPayload=="status") {
  if(status==1 && contact==CONTACT_ON) return String("1");
  else return String("0");
 }
 //-------------------------------------------------------------
 if(sPayload=="relay") return String(contact);
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
  else return String("not erased");         // return answer
 }
 //-------------------------------------------------------------
 if(sTopic=="relay") {                      // switch relay
  if(sPayload=="1" || sPayload=="on") contact=CONTACT_ON;
  else {
   if(sPayload=="0" || sPayload=="off") contact=CONTACT_OFF;
   else {
    if(sPayload=="-1" || sPayload=="toggle") {
     if(contact==CONTACT_ON) contact=CONTACT_OFF;
     else contact=CONTACT_ON;
    }
   }
  }
  if(status==1 && contact==CONTACT_ON) return String("consumer on");
  else return String("consumer off");
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

//_______SETUP__________________________________________________
void setup() {
 //------Serial-------------------------------------------------
 Serial.begin(115200); Serial.println("\nsetup(): --Start--");
 //------set pin mode-------------------------------------------
 pinMode(PIN_LED, OUTPUT);
 pinMode(PIN_DIN, INPUT);
 pinMode(PIN_RELAY, OUTPUT);
 int led_=LED_OFF;                          // help value
 digitalWrite(PIN_LED,led_);                // led off
 digitalWrite(PIN_RELAY,CONTACT_ON);        // relay off -> NC
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
 status = digitalRead(PIN_DIN);
 if(status==0)
 {//-----supply voltage down------------------------------------
  if(counter==0)
  {//----send mqtt message--------------------------------------
   client.sendPubIndex(0,String(status));   // send status 0
   contactLast=CONTACT_OFF;                 // make sure to ..
   contact=CONTACT_ON;                      // switch off relay
   digitalWrite(PIN_RELAY,contact);         // switch relay off
   counter=300;                             // next send in 15s
  }
  else {//----mqtt message sent, wait for next time-------------
   counter--;                               // count status=0
  }
 }
 //------detect a pin change or relay change through MQTT-------
 if(contact!=contactLast || status!=statusLast)
 {
  contactLast=contact;                      // change detected
  statusLast=status;                        // change detected
  digitalWrite(PIN_RELAY,contact);          // switch relay
  int statusOverall=0;                      // help value
  if(status==1 && contact==CONTACT_ON) statusOverall=1;
  client.sendPubIndex(0,String(statusOverall));
 }
 //------process MQTT message(s)--------------------------------
 client.doLoop();                           // mqtt loop
 delay(50);                                 // wait 0.05s
}

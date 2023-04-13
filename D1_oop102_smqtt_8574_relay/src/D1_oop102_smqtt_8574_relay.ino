//_____D1_oop102_smqtt_8574_relay.ino____________khartinger_____
// This program for a D1mini (or ESP32) shows the switching of 
// an 8-channel relay board via an I2C I/O board with PCF8574 
// via MQTT.
// If a certain MQTT message (see below) is received, the 
// corresponding relays 1 to 6 are switched on for one second.
// Additionally the relays 7 and 8 are switched on and off 
// automatically.
// The program can be used e.g. to switch points of an 
// electric railroad. 
// In detail:
// 1.  D1 mini works as a MQTT client and waits for a message
//     -t modul1/set/turnout -m t01=0|1 or
//     -t modul1/set/turnout -m 0x0A
//     When the message arrives, the output in0 is set to 0 or 1
//     and the outputs in7 and in6 are set to 1 for 1sec.
// 2.  Function of this program:
// 2.1 Connect to given WiFi and MQTT broker
// 2.2 Use "topic base" modul1
// 2.3 Automatic (build in) answers for messages 
//     -t modul1/get -m help
//     -t modul1/get -m version
//     -t modul1/get -m ip
// 2.4 Turn on relay(s) for a second
// 2.5 Saving a start pattern
// 2.6 Possibility to change the base topic:
//     -t modul1/set/topicbase -m new_topic_base
//
// Class SimpleMqtt extends class PubSubClient for easy use.
// All commands of the PubSubClient class can still be used.
//
// Hardware:
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. PCF8574 board with (default) address 0x20
//    connected via i2c:
//    SCL=D1=GPIO5, SDA=D2=GPIO4, 3V3, GND
// 3. Relay-Board + 5V + GND
//    e.g. Elegoo 8 Kanal DC 5V Relaymodul with
//    Optokoppler for Arduino (Amazon)
//
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Important: Example needs a MQTT-broker!
// Visual Studio Code: Add line 
// Created by Karl Hartinger, April 12, 2023
// Changes:
// 2023-04-12 New
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
// #define ESP32D1         2              // ESP32 D1mini

#include <Arduino.h>
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/pcf8574/D1_class_pcf8574.h"

#define  DEBUG102       false
#define  VERSION102     "2023-04-12 D1_oop102_smqtt_8574_relay"
#define  LANGUAGE102    'e'            // 'd' or 'e'
#define  TOPIC_BASE     "modul1"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom,turnout,startbyte"
#define  TOPIC_SET      "topicbase,eeprom0,turnout,startbyte"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""
#define  RELAYS_START   0x00

//_______sensors, actors, global variables______________________
PCF8574 pcf8574;                       // 8 digital IOs
//_______Global values__________________________________________
byte    relays=RELAYS_START;           // relay state
bool    relaysGo=true;
String  sConn;                         // Connection state

//_______state machine__________________________________________
#define STATE_MAX            36000     // 36000*100ms = 1 hour
#define STATE_DELAY            100     // state delay in ms
#define STATES_TURNOUT_ON       10     // 10*100ms=1s
Statemachine stm(STATE_MAX, STATE_DELAY); //1..36000
int32_t stateTurnoutOff=-1;            // 

//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
//SimpleMqtt client("Raspi11", "12345678","10.1.1.1", TOPIC_BASE);
SimpleMqtt client(String("Raspi11"), String("12345678"),
                  String("10.1.1.1"), String(TOPIC_BASE));


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
 String p1=String("");                      // help string
 sPayload.toLowerCase();                    // for easy compare
 //-------------------------------------------------------------
  if(sPayload=="help" || sPayload=="?") {
  p1="+MQTT: ../set/turnout -m t01=0 or -m 0x03\r\n";
  return p1;
 }
 //-------------------------------------------------------------
  if(sPayload=="version") {
  p1="{\"version\":\""; p1+= String(VERSION102); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="ip") {
  p1="{\"ip\":\""; p1+= client.getsLocalIP(); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="topicbase") {
  p1="{\"topicbase\":\""; p1+=client.getsTopicBase(); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="eeprom") {
  int iResult1, iResult2;
  String s1=client.eepromReadTopicBase(iResult1);
  String s2=client.eepromReadMyData(iResult2);
  p1="{\"topicbase\":\"";
  if(iResult1>=0) p1+=s1;
  else
  {
   if(iResult1==-2) p1+="(no topic base)";
   else p1+=String("Error_#")+String(iResult1);
  }
  p1+="\",\"mydata\":\"";
  if(s2=="") p1+="(no data)";
  else p1+=s2;
  p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="turnout") {
  p1="{\"turnout\":0x"; p1+=String(relays, 16) ; p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="startbyte") {
  String s0=client.eepromReadMyData();
  if(s0.length()>0) s0=s0.substring(5);
  else s0=String(RELAYS_START);
  p1="{\"startbyte\":0x"; p1+=s0; p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 String p1=String("");                      // help string
 sPayload.toLowerCase();                    // for easy compare
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {                  // new topic base?
  client.changeTopicBase(sPayload);         // change base
  p1="{\"topicbase\":\"";                   // start json
  p1+=client.getsTopicBase();               // read new base
  p1+="\"}";                                // end json
  return p1;                                // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom0") {                    // erase eeprom?
  if(sPayload=="?") {
   p1="{\"erase\":\"";                      // start json
   p1+="1=topicBase|2=myData|3=all\"}";     // end json
   return p1;                               // return erase info
  }
  p1="";
  if(sPayload=="2" || sPayload=="3" || sPayload=="all")
  {
   p1+="my data ";
   if(!client.eepromEraseMyData()) p1+="NOT ";
   p1+="deleted";                           // return answer 1
  }
  if(sPayload=="1" || sPayload=="3" || sPayload=="all")
  {
   if(p1!="") p1+=", ";
   p1+="topicbase ";
   if(!client.eepromEraseTopicBase()) p1+="NOT ";
   p1+="deleted";                           // return answer 2
  }
  p1="{\"eeprom\":\""+p1+"\"}";             // make json
  return p1;
 }
 //-------------------------------------------------------------
 if(sTopic=="turnout") {                    // 
  if(sPayload.charAt(0)=='t' && sPayload.charAt(3)=='=') 
  {//----move one turnout---------------------------------------
   String s1=sPayload.substring(1,3);
   int turnoutNum=(int)s1.toInt();
   if(turnoutNum>0 && turnoutNum<7) {
    s1=sPayload.substring(4);
    if(s1=="0") {
     relays&=~(1<<(turnoutNum-1));
     relaysGo=true;
    } else {
     if(s1=="1") {
      relays|=(1<<(turnoutNum-1));
      relaysGo=true;
     }
    }
   }
  } 
  else
  {
   if(sPayload.charAt(0)=='0' && sPayload.charAt(1)=='x') 
   {//---set relay pattern--------------------------------------
    String s1=sPayload.substring(2);
    int relays_=strtol(s1.c_str(),0,16);
    if(relays_>=0 && relays_<=0x3F) relays=relays_;
     relaysGo=true;
   }
  }
  p1="{\"turnout\":0x";
  p1+=String((int)relays,16);
  p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sTopic=="startbyte") {                  // 
  if(sPayload.charAt(0)=='0' && sPayload.charAt(1)=='x') 
  {//---set start relay pattern---------------------------------
   String s1=sPayload.substring(2);
   int relays_=strtol(s1.c_str(),0,16);
   if(relays_>=0 && relays_<=0x3F)
   {
    String s1=String("R0=");
    client.eepromWriteMyData(s1+sPayload);
    p1=String("new_startbyte:");
    return p1+sPayload;
   }
   return String("start byte not set");
  }
 }
 //-------------------------------------------------------------
 return String("");                         // wrong set command
}

//_______execute sub requests___________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
}

//_______move turnout___________________________________________
void turnoutDrive(bool on_)
{
 byte relays_=relays;
 if(on_) relays_|=0xC0;
 else relays_=0; //relays_&=0x3F;
 pcf8574.setByte(relays_);
}

//_____SETUP____________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 if(DEBUG102) {
  Serial.begin(115200); while(!Serial) continue;
  Serial.println("\nsetup(): --Start--");
 }
 //------init remote 8-Bit I/O Expander PCF8574 #1--------------
 pcf8574.setInvertOutput(true);
 while (!pcf8574.begin()) {                 // start I2C
   Serial.println("Error: "+pcf8574.getsStatus());
   Serial.print(" - Could not find PCF8574 at 0x");
   Serial.print(pcf8574.getAddress(), 16);
   Serial.println(": check wiring");
   delay(5000);                             // wait 5s
 }
 if(DEBUG102) Serial.printf("PCF8574 found at 0x%02X\r\n", pcf8574.getAddress());
 //------(try to) read relay start byte from eeprom---------------
 int iResult;
 relays=RELAYS_START;
 String sMyData=client.eepromReadMyData(iResult);
 if(iResult>0)
 {
  int len1=sMyData.length();
  if(len1>3)
  {
   if(sMyData.substring(0,5)=="R0=0x")
   {
    String s1=sMyData.substring(3);
    relays=strtol(s1.c_str(),0,16);
    if(DEBUG102) Serial.println("Start byte from EEPROM: 0x"+s1);
   }
  }
 }
 //------set relays---------------------------------------------
 turnoutDrive(true);                        // turnout voltage on
 delay(1000);                               // wait
 turnoutDrive(false);                       // turnout voltage off
 relaysGo=false;
 //------setup mqtt---------------------------------------------
 client.setLanguage(LANGUAGE102);           //e=english,d=german
 client.setCallback(callback);              // mqtt receiver
 client.setTopicBaseDefault(TOPIC_BASE);    // topic base
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 //client.setRetainedIndex("get",3,true);
 client.begin();                            // setup objects
 //------connect to WiFi and mqtt broker------------------------
 int i=3;
 while((--i)>=0) { 
  if(client.connectWiFiMQTT()) i=-1;
  delay(500);
 }
 //------setup finish-------------------------------------------
 String sTemp="{\"topicbase\":\"";
 sTemp+=client.getsTopicBase()+"\",\"IP\":\""+client.getsLocalIP()+"\"}";
 client.publish_P("info/start/setup",sTemp.c_str(),false);
 //client.bAllowMQTTStartInfo(false);          //NO mqtt (re)start info
 if(DEBUG102) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG102) Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======(1) make at the beginning of the loop ...==============
 int state=stm.loopBegin();                 // state begin 
 String s1;
 String sSerial=String(state);
 //======(2) do, independent on the network, ...================
 //------turn on turnout drive?---------------------------------
 if(relaysGo) {
  turnoutDrive(true);
  stateTurnoutOff=stm.add(STATES_TURNOUT_ON);
  relaysGo=false;
  sSerial+=" | Turnout drive ON ";
 }
 //------turn off turnout drive?--------------------------------
 if(state==stateTurnoutOff)
 {
  turnoutDrive(false);
  stateTurnoutOff=-1;
  sSerial+=" | Turnout drive OFF";
 }
 //======(3) process mqtt actions===============================
 client.doLoop();                           // mqtt loop
 //======(4) do, depending on the network access, ...===========
/*
 if(client.isWiFiConnectedNew())    sConn=" WiFi OK  ";
 if(client.isMQTTConnectedNew())    sConn=" MQTT OK  ";
 if(client.isMQTTDisconnectedNew()) sConn="-No MQTT--";
 if(client.isWiFiDisconnectedNew()) sConn="-No WiFi--";
 if(DEBUG102) sSerial+=" | "+sConn;
*/
 //======(4) do at the end of the loop ...======================
 uint32_t ms=stm.loopEnd();                   // state end
 //------print serial data--------------------------------------
 if(DEBUG102) {
  Serial.print(sSerial+" | "); Serial.print(ms); 
  if(ms>STATE_DELAY) Serial.println("ms-Too long!!");
  else Serial.println("ms");
 }
}

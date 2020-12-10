//_____D1_oop78_smqtt_oled096_demo1.ino_______191222-201208_____
// This program for a D1 mini (or ESP32 D1mini) is used to
// connect to WiFi and a MQTT broker and show the connecting
// process on a 0.96" OLED:
// Line 1: Signal strength of WiFi
// Line 2: Name of connected WiFi (SSID, `Raspi11`)
// Line 3: IP-Address
// Line 4: Topic for sending und receiving counter value
//         (`oled096_demo1`).
// Line 5: MQTT state
// Line 6: Counter value (out and in)
// Line 7: MAC-Address of D1 mini
// Functions:
// [1] Show MAC address (line 7, inverted)
// [2] Try to connect to the given WLAN (ssid, password).
//     Plot dots on OLED (line 3) while waiting
// [3] Show name of WLAN and IP when connection is established
//     (line 2 and line 3)
// [4] Show MQTT topic (TOPIC_BASE = `oled096_demo1`) (line 4)
// [5] Show state of MQTT connection (line 5)
// [6] When connected, a counter value is sent every 10 seconds
//     to the broker (always topic `oled096_demo1`!).
// [7] A received value (always topic `oled096_demo1`!) is also
//      shown on OLED line 6.
// ------------------------------------------------------------- 
// Additionally the following "standard" features are available:
// [8] Automatic (build in) answers for messages 
//     -t oled096_demo1/get -m help
//     -t oled096_demo1/get -m version
//     -t oled096_demo1/get -m ip
//     Type of answer: topic like oled096_demo1/ret/...
// [9] Answer the following requests with a topic like
//     simplemqtt/ret/...:
//     -t oled096_demo1/get -m topicbase
//     -t oled096_demo1/get -m eeprom
//     -t oled096_demo1/get -m counter
//[10] Set new topic base (also stored in eeprom):
//     -t simplemqtt/set/topicbase -m new_base
//[11] Erase stored topic base (-> use default on restart)
//     -t simplemqtt/set/eeprom -m erase
// Class SimpleMqtt extends class PubSubClient for easy use.
// You can use all commands of class PubSubClient as well.
// When PubSubClient lib is installed,
// delete PubSubClient files in directory src/simplemqtt
// Hardware:
// (1) WeMos D1 mini (OR ESP32 D1 mini)
// (2) one color OLED 0.96" mounted on a proto shield 
//     (128x64 pixel, I2C-address 0x3C)       
//
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, December 08, 2020.
// Changes:
// 2020-12-08 First release
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/screen096/D1_class_Screen096.h"

#define  DEBUG78        true  //false
#define  VERSION78      "2020-12-08 D1_oop78_smqtt_oled096_demo1"
#define  TOPIC_BASE     "oled096_demo1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,counter"
#define  TOPIC_SET      "topicbase,eeprom"
#define  TOPIC_SUB      TOPIC_BASE
#define  TOPIC_PUB      TOPIC_BASE
#define  COUNTER_DIV    100
#define  LINE_LEN       21

//_______sensors, actors, global variables______________________
Screen096 display_;
long   counterLoop=0;
String sCounterOut="?";
String sCounterIn="?";

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
 if(sPayload=="version") return String(VERSION78);
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
 if(sPayload=="counter") return String((counterLoop/COUNTER_DIV));
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
  //String s2="topi "+s1;                     // topic info
  //s2=s2.substring(0,LINE_LEN);              // max. 21 chars
  //display_.screen16(4,s2);                  //  topic: line 4
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
 //-------------------------------------------------------------
 if(sTopic==TOPIC_SUB) {                    // counter in
  sCounterIn=sPayload;
  String s1=String("Cnt: out "+sCounterOut+" in "+sCounterIn);
  display_.screen16(6, s1);                 // display received value
  if(DEBUG78) Serial.println("simpleSub(): "+s1);
 }
}

//_______setup Serial, WLAN and MQTT clients____________________
void setup() 
{
 bool bRet;
 //------Serial, just for debug---------------------------------
 //if(DEBUG78)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------init 0.96" OLED display--------------------------------
 display_.begin();
 display_.useCP437();            // after line display_.begin();
 //------init WiFi/MQTT client----------------------------------
 client.setCallback(callback);
 client.setTopicBaseDefault(String(TOPIC_BASE));
 client.setWiFiWaitingTime(1000);      // set a short time (1s)
 client.setWiFiConnectingCounter(19);  // try 10s connecting
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();                       // init client objects
 //------get MAC address, prepare OLED--------------------------
 String s1="MAC ";
 s1+=client.getsMac();
 if(DEBUG78) Serial.println(s1);
 display_.screen16Clear(0,"? dBm",'c');// line 1: 0=with border
 display_.screen16Dot(3,0);            // line 3: set dotCounter=0
 display_.screen16(-7,s1);             // line 7: inverted (-7)
 //------show connecting procedure------------------------------
 s1="WiFi begin connecting";
 s1=s1.substring(0,LINE_LEN);          // max. 21 character
 display_.screen16(2,s1);              // line 2: begin connect
 if(DEBUG78) Serial.print("setup(): "+s1+String("\n"));
 client.connectingWiFiBegin();         // begin connecting
 s1="WiFi connecting...";
 s1=s1.substring(0,LINE_LEN);
 display_.screen16(2,s1);              // line 2: connecting
 do {
  bRet=client.connectingWiFi();        // try to connect
  display_.screen16Dot(3);             // line 3: waiting dot
 } while(!bRet);
 //------WiFi connected-----------------------------------------
 s1="MyIP ";
 s1+=client.getsMyIP();                // get IP address
 display_.screen16Dot(3,0);            // line 3: set dotCounter=0
 display_.screen16(3, s1);             // line 3: display MyIP
 if(DEBUG78) Serial.println("setup(): "+s1);
 //------setup finish-------------------------------------------
 if(DEBUG78) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG78) Serial.println("setup(): --Finished--\n");
}

int connectingWiFiBeginCounter=0;

//_______LOOP___________________________________________________
void loop() {
 String s1; 
 //======(1) do, independent on the network, ...================
 //------time to force sending counter value?-------------------
 if(counterLoop%COUNTER_DIV==0)
 {
  sCounterOut=String((counterLoop/COUNTER_DIV));
  String s1=String("Cnt: out "+sCounterOut+" in "+sCounterIn);
  display_.screen16(6, s1);            // line 6: received value
  if(DEBUG78) Serial.println("loop(): "+s1); 
  client.simpleMqttDo("pub",TOPIC_PUB,sCounterOut); //force pub-message
 }
 //======(2) do mqtt messages (send, receive)===================
 client.doLoop();                      // doLoop MUST be called
 
 //======(3) do, depending on the network access, ...===========
 //......update display: show MQTT state........................
 s1=client.getsState();                // get MQTT state
 s1="stat "+s1;                        // status info
 s1=s1.substring(0,LINE_LEN);          // max. 21 character
 display_.screen16(5, s1);             // line 5: MQTT state

 if(client.isWiFiConnected())
 {//-----yes: connected to WiFi---------------------------------
  //.....always show signal strength on 0.96" OLED..............
  s1="Signal ";
  s1+=client.getsSignal();             // wifi signal strength
  s1+=" dBm";                          // unit
  display_.screen16(0,s1,'c');         // line 1: signal strength
  //-----MQTT client is connected to WiFi (WLAN)----------------
  if(client.isWiFiConnectedNew())
  {
   if(DEBUG78) Serial.println("loop(): ** New WiFi connection! **");
   //....new WiFi connection: show network name (ssid) on OLED..
   s1="WiFi ";
   s1+=client.getsSSID();
   s1=s1.substring(0,LINE_LEN);        // max. 21 character
   display_.screen16(2, s1);           // line 2: WLAN SSID
   if(DEBUG78) Serial.print("loop(): New connected "+s1); 
   //....new WiFi connection: show ip address on OLED...........
   s1="MyIP ";
   s1+=client.getsMyIP();              // get IP address
   display_.screen16Dot(3, 0);         // set dotCounter to 0
   display_.screen16(3, s1);           // line 3: display MyIP
   if(DEBUG78) Serial.println(", "+s1); 
  }
  //-----WiFi connected, also connected to MQTT server?---------
  if(!client.isMQTTConnected()) client.connectMQTT();
  if(client.isMQTTConnected())
  {
   display_.screen16(4,"topi "+client.getsTopicBase());
   if(client.isMQTTConnectedNew())
   {//...new MQTT connection....................................
    if(DEBUG78) Serial.println("loop(): ** New MQTT connection! **");
   }
  }
  else
  {//....no MQTT connection.....................................
   display_.screen16(4,"topi - (no MQTT)");
   if(client.isMQTTDisconnectedNew())
   {
    if(DEBUG78) Serial.println("loop(): ** MQTT lost! (New) **");
   }
  }
 }
 else
 {//-----no: no connection to WiFi------------------------------
  if(DEBUG78) Serial.println("loop(): No WiFi connection!");
  if(client.isWiFiDisconnectedNew())
  {//....new disconnect!........................................
   if(client.isMQTTDisconnectedNew())
   {
    if(DEBUG78) Serial.println("loop(): ** MQTT lost! (New) **");  
   }
   if(DEBUG78) Serial.println("loop(): ** WiFi lost! (New) **"); 
   display_.screen16(0,"? dBm",'c');   // line 1: no signal
   display_.screen16(2,"WiFi connecting");  // line 2:
   display_.screen16(3,"");            // line 3: clear
   display_.screen16Dot(3,0);          // line 3: dot start
   display_.screen16(4,"topi - (no MQTT)"); // line 4:
   display_.screen16(6,"");            // line 6: counter clear
  }
  else
  {//....not new disconnected: waiting dots 0.96" OLED line 3...
   display_.screen16Dot(3);            // show waiting dots
  }
 }
 

/*
 if(!client.isWiFiConnected())
 {
  //-----MQTT client is NOT connected to WiFi (WLAN)------------
  if(client.isWiFiDisconnectedNew())
  {//....new disconnect?........................................
   if(DEBUG78) Serial.println("loop(): ** WiFi lost! (New) **"); 
   display_.screen16(0,"? dBm",'c');   // line 1: no signal
   display_.screen16(2,"WiFi connecting");  // line 2:
   display_.screen16(3,"");            // line 3: clear
   display_.screen16Dot(3,0);          // line 3: dot start
   display_.screen16(4,"topi - (no MQTT)"); // line 4:
   display_.screen16(6,"");            // line 6: counter clear
  }
  else
  {//....not new disconnected: waiting dots 0.96" OLED line 3...
   display_.screen16Dot(3);            // show waiting dots
  }
  //.....(try to) reconnect.....................................
 }
 else
 {//-----WiFi connected-----------------------------------------
  //.....always show signal strength on 0.96" OLED..............
  s1="Signal ";
  s1+=client.getsSignal();             // wifi signal strength
  s1+=" dBm";                          // unit
  display_.screen16(0,s1,'c');         // line 1: signal strength
  //display_.display();
  //-----MQTT client is connected to WiFi (WLAN)----------------
  if(client.isWiFiConnectedNew())
  {
   if(DEBUG78) Serial.println("loop(): ** New WiFi connection! **");
   connectingWiFiBeginCounter=0; 
   //....new WiFi connection: show properties on 0.96" OLED.....
   s1="WiFi ";
   s1+=client.getsSSID();
   s1=s1.substring(0,LINE_LEN);        // max. 21 character
   display_.screen16(2, s1);           // line 2: WLAN SSID
   if(DEBUG78) Serial.print("loop(): New connected "+s1); 
   s1="MyIP ";
   s1+=client.getsMyIP();              // get IP address
   display_.screen16Dot(3, 0);         // set dotCounter to 0
   display_.screen16(3, s1);           // line 3: display MyIP
   if(DEBUG78) Serial.println(", "+s1); 
  }
  //-----WiFi connected, also connected to MQTT server?---------
  if(!client.isMQTTConnected()) client.connectMQTT();
  if(client.isMQTTConnected())
  {
   display_.screen16(4,"topi "+client.getsTopicBase());
   if(client.isMQTTConnectedNew())
   {
    if(DEBUG78) Serial.println("loop(): ** New MQTT connection! **");
   }
  }
  else
  {
   display_.screen16(4,"topi - (no MQTT)");
   if(client.isMQTTDisconnectedNew())
   {
    if(DEBUG78) Serial.println("loop(): ** MQTT lost! (New) **");
   }
  }
 }
 */
 //======(4) do at the end of the loop ...======================
 //------increment counter--------------------------------------
 counterLoop++; if(counterLoop>99999) counterLoop=0;
 delay(100);
}

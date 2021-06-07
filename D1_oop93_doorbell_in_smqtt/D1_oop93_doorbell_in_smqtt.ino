//_____D1_oop93_doorbell_in_smqtt.ino____________khartinger_____
// 1.  D1 mini works as a MQTT client and sends a message 
//     every 60 secs, when the voltage/resistor (LDR) at the 
//      analog input pin is too low:
//     -t {TOPIC_BASE}/ret/ring -m 1
//     Falling below the limit value is also indicated by the 
//     blue LED.
// 2.  Structure of MQTT messages:
// 2.1 Requests: -t {TOPIC_BASE}/get -m {TOPIC_GET}
//               -t {TOPIC_BASE}/set/{TOPIC_SET} -m value
// 2.2 Answers : -t {TOPIC_BASE}/ret/{TOPIC_GET} -m value
// 2.3 Available MQTT topics are listed in #defines
//     TOPIC_GET, TOPIC_SET
// 3.  State machine cycle time: 1 hour (cycle time 0.05 sec)
//
// Application:
// If a doorbell lights up in addition to ringing, this light
// can be converted into a voltage via an LDR (light-dependent
// resistor) and a voltage divider and measured at A0.
// As long as the voltage is below a certain threshold 
//("ainlimit0"), MQTT messages are sent. The blockingtime between
// two messages can be set (default: 1 minute).
//
// Hardware: 1. WeMos D1 Mini
// Sensors:  1. voltage divider at A0:
//              47kOhm pullup resistor to 3V3 and 
//              LDR (Light depending resoistor) to GND
// Created by Karl Hartinger, June 07, 2021.
// Changes: 
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini

#include "src/din/D1_class_Din.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#define  DEBUG93        true // false //true
#define  VERSION93      "2021-06-07 D1_oop93_doorbell_in_smqtt"
#define  FUNCTION93     "mqtt message on LDR low"
#define  LANGUAGE93     'e'            // 'd' or 'e'
#define  TOPIC_BASE     "doorbell/1"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom,all,ring,blockingtime,ainlimit0,ain"
#define  TOPIC_SET      "topicbase,eeprom0,blockingtime,ainlimit0"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""
#define  DOOLBELL0      100                 // limit for ADC 0

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define  DEVICE             ESP32D1
 #define  AIN_MAX            4095
 #define  PIN_AIN_           36             // ESP 36, A0=17
 #define  PIN_DIN_           18             // D5=18
 #define  BLUELED_PIN        2              // led pin 
 #define  BLUELED_ON         1              // ESP32 1
 #define  BLUELED_OFF        0              // ESP32 0
#else
 #define  DEVICE             D1MINI
 #define  AIN_MAX            1023
 #define  PIN_AIN_           A0             // A0=17, ESP 33
 #define  PIN_DIN_           D5             // D5=18
 #define  BLUELED_PIN        D4             // led pin D4=IO2
 #define  BLUELED_ON         0              // D1mini 0
 #define  BLUELED_OFF        1               // D1mini 1
#endif

//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client(String("Raspi11"), String("12345678"),
                  String("10.1.1.1"), String(TOPIC_BASE));

//_______state machine__________________________________________
#define STATE_MAX            36000     // 36000*100ms = 1h
#define STATE_DELAY            100     // state delay in ms
#define STATES_WAIT_MQTT       600     // 60 sec
Statemachine stm(STATE_MAX, STATE_DELAY); //1..600
bool bAllowMQTT=true;                  // allow sending bell msg

//-------global values------------------------------------------
Din      dinBell(PIN_DIN_);
int      ainlimit0=DOOLBELL0;          // limit for ADC 0
bool     bBell=false;                  // state of Bell
int      ain1023=100;                  // limit ain for "0"
int      statesWait=STATES_WAIT_MQTT;  // wait between 2xmsg

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
 String p1;
 sPayload.toLowerCase();
 //-------------------------------------------------------------
 if(sPayload=="version") {
  p1="{\"version\":\""; p1+= String(VERSION93); p1+="\"}";
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
 if(sPayload=="ain") {
  p1="{\"ain\":"; 
  p1+=String(ain1023);
  p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="blockingtime") {
  p1="{\"blockingtime\":"; 
  long lsec=statesWait*STATE_DELAY/1000;
  p1+=String(lsec);
  p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="ainlimit0") {
  p1="{\"ainlimit0\":"; 
  p1+=String(ainlimit0);
  p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="ring") {
  if(bBell) p1="1";
  else p1="0";
  return p1;
 }
 //-------------------------------------------------------------
 //if((sPayload=="alles")||(sPayload=="status")) {
 if(sPayload=="all") {
  p1 ="{\"version\":\""+String(VERSION93)+"\",";
  p1+="\"IP\":\"" +client.getsMyIP()+"\",";
  p1+="\"ring\":";
  if(bBell) p1+="1,";
  else p1+="0,";
  p1+="{\"blockingtime\":"; 
  long lsec=statesWait*STATE_DELAY/1000;
  p1+=String(lsec);
  p1+=",";
  p1+="\"ainlimit0\":"+String(ainlimit0) +",";
  p1+="\"ain\":"+String(ain1023); // +",";
  p1+="}";
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
 int iOn;                                   // help value
 sPayload.toLowerCase();
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
 if(sTopic=="blockingtime") {                   // sec
  p1="{\"blockingtime\":";                      // start json
  int sec=sPayload.toInt();
  if(sec>0 && sec<61) {
   long temp=1000*sec/STATE_DELAY;
   statesWait=temp;
   String s1=String("G0=");                 // name in eeprom
   s1+=String(ainlimit0);                     // ADC limit 0
   s1+=String(",SW=");                      // name statesWait
   s1+=String(statesWait);                  // value statesWait
   client.eepromWriteMyData(s1);            // set eeprom
  }
  long lsec=statesWait*STATE_DELAY/1000;
  p1+=String(lsec);
  p1+="}";                                  // end json
  return p1;
 }
 //-------------------------------------------------------------
 if(sTopic=="ainlimit0") {                    // set ADC 0 limit
  p1="{\"ainlimit0\":";                       // start json
  iOn=sPayload.toInt();
  if((iOn>0)&&(iOn<=AIN_MAX))
  {
   ainlimit0=iOn;                             // working value
   String s1=String("G0=");                 // name in eeprom
   s1+=String(ainlimit0);                     // ADC limit 0
   s1+=String(",SW=");                      // name statesWait
   s1+=String(statesWait);                  // value statesWait
   client.eepromWriteMyData(s1);            // set eeprom
  }
  p1+=String(ainlimit0);                      // get value
  p1+="}";                                  // end json
  return p1;
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

//_____SETUP____________________________________________________
void setup() {
 delay(200);                                // for i2c display
 //----- Serial, just for debug---------------------------------
 //if(DEBUG92)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------init hardware------------------------------------------
 pinMode(PIN_AIN_, OUTPUT);
 pinMode(BLUELED_PIN, OUTPUT);              // set pin to output
 digitalWrite(BLUELED_PIN,BLUELED_OFF);     // turn led off

 //------(try to) read current0 from eeprom---------------------
 int iResult;
 String sMyData=client.eepromReadMyData(iResult);
 if(iResult>0)
 {
  int len1=sMyData.length();
  if(len1>8)
  {
   int ipos1=sMyData.indexOf("G0=");
   int ipos2=sMyData.indexOf(",SW=");
   if(ipos1==0 && ipos2!=-1)
   {
    String s1=sMyData.substring(3,ipos2);
    int temp=s1.toInt();
    if(DEBUG93) Serial.println("\"ainlimit0\" read from EEPROM: "+s1);
    ainlimit0=temp;
    s1=sMyData.substring(4+ipos2);
    temp=s1.toInt();
    if(DEBUG93) Serial.println("\"statesWait\" read from EEPROM: "+s1);
    statesWait=temp;
   }
  }
 }
 //------setup mqtt---------------------------------------------
 client.setLanguage(LANGUAGE93);            //e=english,d=german
 client.setCallback(callback);
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();
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
 if(DEBUG93) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 Serial.println("setup(): --Finished--\n");
}

int stateNextMqtt=-1;

//_______LOOP___________________________________________________
void loop() {
 //======(1) do, independent on the network, ...================
 int state=stm.loopBegin();                 // state begin 
 String s1;                                 // help string
 String sSerial="";                         // serial result
 //------do every state-----------------------------------------
 if(DEBUG93) { 
  s1=String(state);                         // format state #
  sSerial=("     "+s1).substring(s1.length())+": ";
 }
 //------show analog in value-----------------------------------
 ain1023=analogRead(PIN_AIN_);
 if(DEBUG93) sSerial+="ain="+String(ain1023)+" | ";
 if(ain1023<ainlimit0) {
  digitalWrite(BLUELED_PIN,BLUELED_ON);
  if(DEBUG93) sSerial+="< | ";
 }
 else {
  digitalWrite(BLUELED_PIN,BLUELED_OFF);
  if(DEBUG93) sSerial+="> | ";
 }
 //------allow next doorbell ringing----------------------------
 if(state==stateNextMqtt) {
  bAllowMQTT=true; 
  bBell=false; 
  client.simpleMqttDo("get","doorbell","");
 }
 //------Is the doorbell ringing?-------------------------------
 if((ain1023<ainlimit0) && bAllowMQTT)
 {
  bAllowMQTT=false;
  bBell=true;
  stateNextMqtt=stm.add(statesWait);
  sSerial+=" ** Doorbell is ringing! ** | ";
  client.simpleMqttDo("get","ring","");
 }
 //======(2) do mqtt messages (send, receive)===================
 if(!stm.isDelayed())
  client.doLoop();                          // mqtt loop
 //======(3) do, depending on the network access, ...===========
 //======(4) do at the end of the loop ...======================
 uint32_t ms=stm.loopEnd();                 // state end
 //------show debug info: all states----------------------------
 if(DEBUG93) {
  Serial.print(sSerial); Serial.print(ms); 
  if(ms<=STATE_DELAY) Serial.println("ms");
  else Serial.println("ms-Too long!!");
 }
 //------show debug info: only non empty states-----------------
 //if(DEBUG93){if(sSerial.length()>7) Serial.println(sSerial);}
}

//_____D1_oop42_mqtt_relais1.ino______________181101-181101_____
// D1 mini as MQTT client switches a lamp via a relay and 
// measures the current.
// Hardware: (1) WeMos D1 mini
//           (2) Relais shield
//           (3) D1_INA122_V2 180924 (Selfmade) or Analog in
//               and Current transformer ASM-010
// Important: Example needs a broker!
// Created by Karl Hartinger, November 01, 2018
// Last modified: -
// Released into the public domain.
#define  DEBUG42        true  //false
#define  VERSION42      "2018-11-01 D1_oop42_mqtt_relais1"
#define  FUNCTION42     "Relay \"relay1\" waits for \"relay1/set/lamp on|off|toggle\""
#define  GETMAX42       8
#define  SETMAX42       2
const String sGet[GETMAX42]={"help","version","function","ip","all","lamp","current","current0"};
const String sSet[SETMAX42]={"lamp","current0"};

#include "src/mqtt/D1_class_MqttClientKH.h"
#include "src/relais1/D1_class_relais1.h"
#include "src/statemachine/D1_class_Statemachine.h"

//-----MQTT-----------------------------------------------------
//MqttClientKH client("..ssid..", "..password..","mqttservername");
MqttClientKH client("Raspi10", "12345678","10.1.1.1");

//......supported mqtt topics...................................
#define TOPIC_R1_ALL             ("relay1/#")
#define TOPIC_R1_BASIC           ("relay1")
#define TOPIC_R1_GET             ("relay1/get")
#define TOPIC_R1_SET             ("relay1/set/")
#define TOPIC_R1_RET             ("relay1/ret/")
//.....number of send action....................................
#define NR_R1_NOTHING            0     // # mqtt do nothing
#define NR_R1_HELP               0     // # mqtt send help info
#define NR_R1_VERSION            1     // # mqtt send version
#define NR_R1_FUNCTION           2     // # mqtt send function
#define NR_R1_IP                 3     // # mqtt send ip
#define NR_R1_GETALL             4     // # mqtt send all topics
#define NR_R1_LAMP               5     // # mqtt send lamp state
#define NR_R1_CURRENT            6     // # mqtt send current
#define NR_R1_CURRENT0           7     // # mqtt current on limit

//......"container" for all send actions........................
int     mqttSend=NR_R1_NOTHING;        // 0=nothing

//-----state machine--------------------------------------------
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_MQTTSEND          10     // 10x 100ms = 1.0sec
Statemachine statemachine(STATE_MAX, STATE_DELAY); //1..600

//-----sensors, actors------------------------------------------
#define  PIN_RELAIS     D1
Relais1  relais1(PIN_RELAIS);          // relais shield D1

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 //-----convert topic and payload to String---------------------
 String sTopic=String(topic);
 String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 if(DEBUG42)
  Serial.println("Message topic '"+sTopic+"': "+sPayload);
 //-----get request---------------------------------------------
 if(sTopic==TOPIC_R1_GET)
 {
  int i;
  for(i=0; i<GETMAX42; i++) {
   if(sGet[i]==sPayload) break;
  }
  if(i<GETMAX42) {                     // get payload found
   mqttSend|=(1<<i);                   // trigger get request
  }
 }
 //-----commands: set request-----------------------------------
 if(sTopic==(TOPIC_R1_SET+sSet[0]))    // lamp
 {
  if(sPayload=="on"){relais1.on(); mqttSend|=(1<<NR_R1_LAMP);}
  if(sPayload=="off"){relais1.off();mqttSend|=(1<<NR_R1_LAMP);}
  if(sPayload=="toggle") {relais1.toggle();mqttSend|=(1<<NR_R1_LAMP);}
 }
 if(sTopic==(TOPIC_R1_SET+sSet[1]))    // strom0
 {
  int ion=sPayload.toInt();
  if((ion>0)&&(ion<=relais1.getCurrentMax()))
   relais1.setCurrentOn(ion);
  mqttSend|=(1<<NR_R1_CURRENT0);
 }
}

//_____send MQTT message________________________________________
bool sendMQTT(int what_)
{
 int num1;                             // number of action
 unsigned int mask1;                   // mask ...001000
 String t1, p1;                        // topic, payload
 bool r_=true;                         // return value
 //=====plausibility check======================================
 if(DEBUG42) Serial.println("sentMQTT(0x"+String(what_,16)+")");
 if(what_<1) {
  if(DEBUG42) Serial.println("Nothing to do.");
  return false;
 }
  //=====check for WLAN==========================================
 if(!client.connected())
 {
  if(DEBUG42) Serial.println("Not connected!");
  return false;
 }
 if(DEBUG42) Serial.println();
 //=====messages for other devices==============================
 // ...no messages...
 //=====messages for this device================================
 num1=NR_R1_GETALL; //-----send all messages--------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {                                     // set all send bits
  for(int j=0; j<GETMAX42; j++) mqttSend|=(1<<j);
  mqttSend&=(~mask1);                  // done: clear getall bit
 }
 num1=NR_R1_HELP; //-----send help info-------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1="get: ";
  for(int i=0; i<GETMAX42; i++) p1+=sGet[i]+"|";
  p1+="\r\nset: ";
  for(int i=0; i<SETMAX42; i++) p1+=sSet[i]+"|";
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_VERSION; //-----send version------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=String(VERSION42);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_FUNCTION; //-----send function----------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=String(FUNCTION42);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_IP; //-----send ip----------------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=client.getMyIP();
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_LAMP; //-----send relais (lamp) state-------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=String(relais1.getLampstate());
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_CURRENT; //-----send current------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=String(relais1.getCurrent(),0);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_CURRENT0; //-----send current is 0 limit------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=String(relais1.getCurrentOn(),0);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 return r_;
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 {Serial.begin(115200); Serial.println("\n"+String(VERSION42));}
 //-----set actors----------------------------------------------
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(TOPIC_R1_ALL));
 client.setCallback(callback);
 client.reconnect();
 //-----turn off relay/lamp-------------------------------------
 relais1.off();
}

//_____main loop________________________________________________
void loop() 
{
 int state=statemachine.loopBegin();        // state begin 
 //=====do every state==========================================
 //if(DEBUG42) Serial.print(String(state)+": ");
 if(client.isConnected()) //must always be called (for receive!)
 {
  //.....send MQTT answer.......................................
  if((state%STATE_MQTTSEND)==0) {
   if(mqttSend>0) sendMQTT(mqttSend);
  }
 }
 //=====do every second=========================================
 if((state%5)==0)
 {
  //.....check lamp state.......................................
  if(relais1.changed()) {
   mqttSend|=(1<<NR_R1_LAMP); 
  }
 }
 //=====prepare for next state and wait=========================
 statemachine.loopEnd();                    // state end
 //if(DEBUG42) Serial.println();
}

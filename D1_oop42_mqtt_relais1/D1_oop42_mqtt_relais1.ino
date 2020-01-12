//_____D1_oop42_mqtt_relais1.ino______________181101-200109_____
// This example uses a D1 mini as MQTT client to switch a lamp 
// via a relay and measure the current. If the D1 mini receives 
// a topic `relay1/set/relay` with the content (payload) `on`,
// it switches the relay on, with `off` it switches the relay
// off and with payload `toggle` it toggles the relay.   
// The two LEDS show the system state:   
// * DUO-LED red blinking: No connection to WiFi (WLAN)
// * DUO-LED red: MQTT error
// * DUO-LED green: MQTT connection to broker OK.
//   The green LED blinks every 2.5 seconds to show,
//   that the D1 mini is working. 
// Hardware: (1) WeMos D1 mini
//           (2) Relay shield
//           (3) D1_INA122_V2 180924 (Selfmade) or Analog in
//               and Current transformer ASM-010
//           (4) LED at D8, Duo-LED at D6-green/D7-red
// Important: Example needs a broker!
// Created by Karl Hartinger, November 01, 2018
// Modified: 191227 use MqttClientKH2 instead of MqttClientKH
//           200103 use class Relay1 instead of Relais1
//           200109 LED at D8, Duo-LED at D6-green/D7-red
// Released into the public domain.
#define  DEBUG42        true  //false
#define  VERSION42      "2020-01-09 D1_oop42_mqtt_relais1"
#define  FUNCTION42     "Relais \"relay1\" empfaengt \"relay1/set/relay on|off|toggle\""
#define  GETMAX42       8
#define  SETMAX42       2
const String sGet[GETMAX42]={"help","version","function","ip","all","relay","current","current0"};
const String sSet[SETMAX42]={"relay","current0"};

#include "src/mqtt2/D1_class_MqttClientKH2.h"
#include "src/relay1/D1_class_relay1.h"
#include "src/statemachine/D1_class_Statemachine.h"

//-----MQTT-----------------------------------------------------
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi10", "12345678","10.1.1.1");
//......supported mqtt topics...................................
#define TOPIC_R1_ALL             ("relay1/#")
#define TOPIC_R1_BASIC           ("relay1/")
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
#define NR_R1_RELAY              5     // # mqtt send relay state
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
#define  PIN_LED1       D8
#define  PIN_LED_G      D6
#define  PIN_LED_R      D7
#define  PIN_RELAIS     D1
Relay1  relais1(PIN_RELAIS,2000,50);   // relais shield

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 //-----convert topic and payload to String---------------------
 char cPayload[length+1];              // char-array for payload 
 strncpy(cPayload,(char*)payload,length); // copy payload bytes
 cPayload[length]=0;                   // set end char
 if(DEBUG42)
 { Serial.print("Message topic '"); Serial.print(topic);
   Serial.print("': "); Serial.println(cPayload);
 }
 //-----get request---------------------------------------------
 if(strcmp(topic,TOPIC_R1_GET)==0)
 {
  int i;
  for(i=0; i<GETMAX42; i++) {
   if(strcmp(sGet[i].c_str(), cPayload)==0) break;
  }
  if(i<GETMAX42) {                     // get payload found
   mqttSend|=(1<<i);                   // trigger get request
  }
 }
 //-----commands: set request-----------------------------------
 int lenSet=strlen(TOPIC_R1_SET);
 if(strncmp(topic,TOPIC_R1_SET,lenSet)==0)
 {//-----set topic found----------------------------------------
  char* settype=topic;                 // 
  settype+=lenSet;                     // topic command
  if(strcmp(settype,sSet[0].c_str())==0)
  {//.....light on/off/toggle...................................
   if(strcmp(cPayload,"on")==0){relais1.on(); mqttSend|=(1<<NR_R1_RELAY);Serial.println("R ein");}
   if(strcmp(cPayload,"off")==0){relais1.off();mqttSend|=(1<<NR_R1_RELAY);Serial.println("R aus");}
   if(strcmp(cPayload,"toggle")==0) {relais1.toggle();mqttSend|=(1<<NR_R1_RELAY);Serial.println("R um");}
  }
  if(strcmp(settype,sSet[1].c_str())==0)
  {//.....set current xxx.......................................
   int ion=atoi(cPayload);
   if((ion>0)&&(ion<=relais1.getCurrentMax()))
    relais1.setCurrentOn(ion);
   mqttSend|=(1<<NR_R1_CURRENT0);
  } 
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
  client.publishString(t1,p1);         // publish
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
  p1=client.getsMyIP();
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_R1_RELAY; //-----send relais (lamp) state--------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_R1_RET)+sGet[num1];
  p1=String(relais1.getRelaystate());
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

//_____blink blue LED Quickly___________________________________
void blink()
{
 for(int i=0; i<8; i++) {
  digitalWrite(PIN_LED_R, 1);               // LED on
  delay(100);
  digitalWrite(PIN_LED_R, 0);               // LED off
  delay(150);
  }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 {Serial.begin(115200); Serial.println("\n"+String(VERSION42));}
 //-----set actors----------------------------------------------
 pinMode(PIN_LED1, OUTPUT);
 pinMode(PIN_LED_G, OUTPUT);
 pinMode(PIN_LED_R, OUTPUT);
 pinMode(D4, OUTPUT); digitalWrite(D4, 0);
 //-----turn off lamp-------------------------------------------
 relais1.on();
 //-----test leds-----------------------------------------------
 digitalWrite(PIN_LED1, 1);                 // 2. green led on
 digitalWrite(PIN_LED_G, 1);                // green on
 digitalWrite(PIN_LED_R, 0);                // red off
 delay(500);
 digitalWrite(PIN_LED1, 0);                 // 2. green led off
 digitalWrite(PIN_LED_R, 1);                // orange
 delay(500);
 digitalWrite(PIN_LED_G, 0);                // just red
 delay(500);
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(TOPIC_R1_ALL));
 client.addPublish(String(TOPIC_R1_RET));
 client.setCallback(callback);
 while(!client.connectWiFiMQTT()) blink();
 if(DEBUG42) Serial.println("setup() finished -> start loop()\n");
}

int redLED=0;

//_____main loop________________________________________________
void loop() 
{
 int state=statemachine.loopBegin();        // state begin 
 //=====do every state==========================================
 //if(DEBUG42) Serial.print(String(state)+": ");
 if(!client.isWiFiConnected()) 
 {//=====MQTT client is NOT connected to WiFi (WLAN)============
  digitalWrite(PIN_LED_R, redLED);          // no WiFi...
  redLED=1-redLED;                          // blink quickly red
 }
 else
 {//====MQTT client is connected to WiFi (WLAN)=================
  if(client.isConnected()) //must always be called (for receive!)
  {
   digitalWrite(PIN_LED_G, 1);
   digitalWrite(PIN_LED_R, 0);
   //.....send MQTT answer......................................
   if((state%STATE_MQTTSEND)==0) {
    if(mqttSend>0) sendMQTT(mqttSend);
   }
  }
  else
  {
   digitalWrite(PIN_LED_G, 0);
   digitalWrite(PIN_LED_R, 1);
  }
  //=====do every 0.5 seconds===================================
  if((state%5)==0)
  {
   //.....check lamp state......................................
   if(relais1.changed()) {
    mqttSend|=(1<<NR_R1_RELAY); 
   }
  }
  //=====blink LED==============================================
  if((state%25)==0) digitalWrite(PIN_LED1, 1);
  else digitalWrite(PIN_LED1, 0);
 }
 
 //=====prepare for next state and wait=========================
 statemachine.loopEnd();                    // state end
 //if(DEBUG42) Serial.println();
}

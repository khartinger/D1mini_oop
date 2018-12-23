//_____D1_oop43_mqtt_button1_led56.ino________170721-181101_____
// Press button D3 to send a mqtt-request.
// Hardware: (1) WeMos D1 mini
//           (2) Protoboard with button at D3, 2pin duo led
//               between D5 and D6 (with 100 Ohm resistor)
// On start LED lights red and D1 mini tries to connect to a
// mqtt-broker. On success led shines green.
// When button D3 is pressed, D1 mini
//   + publishes topic "relay1/set/lamp" with payload "toggle"
//   + publishes button info under topic "button1/ret/function"
//   + turns blue led on
// Then D1 mini waits for topic relay1/ret/lamp, payload 0 or 1.
// If there is no answer, the blue led shines continuous, 
// if the answer is 0, blue led blinks short pulses (1:9),
// on 1 it blinks long pulses (9:1) for 20 seconds.

// Important: Example needs a broker!
// Created by Karl Hartinger, November 01, 2018.
// Last modified: -
// Released into the public domain.
#define  DEBUG43        true  //false
#define  VERSION43      "2018-11-01 D1_oop43_mqtt_button1_led56"
#define  FUNCTION43     "button \"button1\" sends \"relay1/set/lamp toggle\""
#define  GETMAX43       5
#define  SETMAX43       0
const String sGet[GETMAX43]={"help","version","function","ip","all"};
const String sSet[SETMAX43]={};

#include "src/mqtt/D1_class_MqttClientKH.h"
#include "src/din/D1_class_Din.h"
#include "src/dout/D1_class_Dout.h"
#include "src/statemachine/D1_class_Statemachine.h"

//-----MQTT-----------------------------------------------------
//MqttClientKH client("..ssid..", "..password..","mqttservername");
MqttClientKH client("Raspi10", "12345678","10.1.1.1");

//......supported mqtt receive topics...........................
#define TOPIC_B1_ALL             ("button1/#")
#define TOPIC_B1_BASIC           ("button1")
#define TOPIC_B1_GET             ("button1/get")
#define TOPIC_B1_SET             ("button1/set/")
#define TOPIC_B1_RET             ("button1/ret/")
//.....supported mqtt send topics...............................
#define TOPIC_B1_SWITCH          ("relay1/set/lamp")
#define PAYLOAD_B1_SWITCH        ("toggle")
#define TOPIC_B1_SWITCH_RET      ("relay1/ret/lamp")
//.....number of send action....................................
#define NR_B1_NOTHING            0     // # mqtt do nothing
#define NR_B1_HELP               0     // # mqtt send help
#define NR_B1_VERSION            1     // # mqtt send version
#define NR_B1_FUNCTION           2     // # mqtt send function
#define NR_B1_IP                 3     // # mqtt send ip
#define NR_B1_GETALL             4     // # mqtt send all topics
#define NR_B1_SWITCH             7     // # mqtt send switch cmd

//......"container" for all send actions........................
int     mqttSend=NR_B1_NOTHING;        // 0=nothing

//-----state machine--------------------------------------------
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_MQTTSEND          10     // 10x 100ms = 1.0sec
#define STATE_BLUELEDOFF       200     // 200x 100ms= 20sec
Statemachine statemachine(STATE_MAX, STATE_DELAY); //1..600
int state_blueledoff;

//-----sensors, actors------------------------------------------
#define  PIN_BUTTON             D3
#define  PIN_LED_BLUE           D4
#define  PIN_LED_GREEN          D5
#define  PIN_LED_RED            D6

Din  button_(PIN_BUTTON);              // button at D3
Dout ledBlue(PIN_LED_BLUE, true);      // blue led  @ D1 mini
Dout ledGreen(PIN_LED_GREEN);          // green led @ protoboard
Dout ledRed(PIN_LED_RED);              // red led   @ protoboard
int  blueled;

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 //-----convert topic and payload to String---------------------
 String sTopic=String(topic);
 String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 if(DEBUG43)
  Serial.println("Message topic '"+sTopic+"': "+sPayload);
 //-----get request---------------------------------------------
 if(sTopic==TOPIC_B1_GET)
 {
  int i;
  for(i=0; i<GETMAX43; i++) {
   if(sGet[i]==sPayload) break;
  }
  if(i<GETMAX43) {                     // get payload found
   mqttSend|=(1<<i);                   // trigger get request
  }
 }
 //-----set request--------------------------------------------- 
 // ... no set topics defined ...
 //-----check for confirmation of primary function--------------
 if(sTopic==TOPIC_B1_SWITCH_RET)
 {
  if((sPayload=="0")||(sPayload=="1")) {
   blueled=sPayload.toInt();
   ledBlue.off();                      // blue: command...
   String s1="on";
   if(sPayload=="0") s1="off";
   if(DEBUG43) Serial.println("lamp is "+s1);
  }
 }
}

//_____send MQTT messages_______________________________________
bool sendMQTT(int what_)
{
 int num1;                             // number of action
 unsigned int mask1;                   // mask ...001000
 String t1, p1;                        // topic, payload
 bool r_=true;                         // return value
 //=====plausibility check======================================
 if(DEBUG43) Serial.println("sentMQTT(0x"+String(what_,16)+")");
 if(what_<1) {
  if(DEBUG43) Serial.println("Nothing to do.");
  return false;
 }
  //=====check for WLAN==========================================
 if(!client.connected())
 {
  if(DEBUG43) Serial.println("Not connected!");
  return false;
 }
 if(DEBUG43) Serial.println();
 //=====messages for other devices==============================
 num1=NR_B1_SWITCH; //-----send toogle to switch----------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_SWITCH);
  p1=String(PAYLOAD_B1_SWITCH);
  if(client.publishString(t1,p1)) {    // publish...
   mqttSend&=(~mask1);                 // success: stopp sending
   ledBlue.on();                       // blue: command...
   ledGreen.off();                     // sent, waiting...
   ledRed.off();                       // for confirmation
  } else r_=false;
 }
 //=====messages for this device================================
 num1=NR_B1_GETALL; //-----send all messages--------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {                                     // set all send bits
  for(int j=0; j<GETMAX43; j++) mqttSend|=(1<<j);
  mqttSend&=(~mask1);                  // done: clear getall bit
 }
 num1=NR_B1_HELP; //-----send help info-------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RET)+sGet[num1];
  p1="Possible payloads ";
  for(int i=0; i<GETMAX43; i++) p1+=+"|"+sGet[i];
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B1_VERSION; //-----send version------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RET)+sGet[num1];
  p1=String(VERSION43);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B1_FUNCTION; //-----send function----------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RET)+sGet[num1];
  p1=String(FUNCTION43);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B1_IP; //-----send ip----------------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RET)+sGet[num1];
  p1=client.getMyIP();
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 return r_;
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 {Serial.begin(115200); Serial.println("\n"+String(VERSION43));}
 //-----set actors----------------------------------------------
 ledBlue.off();
 ledGreen.off();
 ledRed.on();
 blueled=-1;                           // no led info on/off
 //-----setup mqtt and trigger toggle message-------------------
 client.addSubscribe(String(TOPIC_B1_ALL));
 client.addSubscribe(String(TOPIC_B1_SWITCH_RET));
 client.setCallback(callback);
 client.reconnect();
 //mqttSend|=(1<<NR_B1_SWITCH);        // trigger toggle switch
}

//_____main loop________________________________________________
void loop() 
{
 int state=statemachine.loopBegin();        // state begin 
 //if(DEBUG43) Serial.print(String(state)+": ");
 //-----do every state------------------------------------------
 if(client.isConnected()) //must always be called (for receive!)
 {
  client.loop();
  ledGreen.on();
  ledRed.off();
 }  
 else
 {
  ledGreen.off();
  ledRed.on();
  ledBlue.off();
 }
 //.....process button..........................................
 if(button_.isFallingEdge())
 {
  delay(10);
  button_.isFallingEdge();
  mqttSend|=(1<<NR_B1_SWITCH);          // trigger toggle switch
  mqttSend|=(1<<NR_B1_FUNCTION);        // for documentation
  ledBlue.on();
  state_blueledoff=statemachine.add(STATE_BLUELEDOFF);
  blueled=-1;
  if(DEBUG43) Serial.println("button pressed"); 
  sendMQTT(mqttSend);
 }

 //-----do not every state--------------------------------------
 //.....send MQTT answer........................................
 if((state%STATE_MQTTSEND)==5)
 {
  if(DEBUG43) {
   if(mqttSend>0) Serial.println(String(state)+": mqttSend="+String(mqttSend));
  }
  if(mqttSend>0) sendMQTT(mqttSend);
 }
 
 //.....turn blue led off after 20 sec..........................
 if(state==state_blueledoff) { blueled=-1; ledBlue.off(); }
 if(state%10==0) { if(blueled!=-1) ledBlue.set(1-blueled); }
 if(state%10==9) { if(blueled!=-1) ledBlue.set(blueled); }
 
 //-----prepare for next state and wait-------------------------
 statemachine.loopEnd();                    // state end
 //if(DEBUG43) Serial.println();
}

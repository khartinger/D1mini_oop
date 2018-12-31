//_____D1_oop46_mqtt_button1_blueledint.ino___181101-181101_____
// Press button D3 sends a mqtt-request.
// Hardware: (1) WeMos D1 mini
//           (2) Button shield D3
// On start blue LED lights and D1 mini tries to connect to a
// mqtt-broker. On success led is turned off, otherwise it
// blinks quickly (on:off=0.1s:0.1s).
// When button D3 is pressed, D1 mini
//   + publishes topic "relay1/set/lamp" with payload "toggle"
//   + publishes button info under topic "button1/ret/function"
//   + blue led blinks slowly (on:off=0.5s:0.5s)
// Then D1 mini waits for topic relay1/ret/lamp, payload 0 or 1.
// If there is no answer, the blue led retains blinking slowly 
// (on:off=0.5s:0.5s). 
// If the answer is 0, blue led blinks short pulses (0.1:0.9).
// If the answer is 1, blue led blinks long pulses (0.9:0.1).
// After 15secs led is turned off (to save energy ;).
//
// Note: Uses class BlueLedInt for blinking (timer interrupt!)
// Important: Example needs a broker!
// Created by Karl Hartinger, November 01, 2018.
// Last modified: -
// Released into the public domain.
#define  DEBUG46        true  //false
#define  VERSION46      "2018-11-01 D1_oop46_mqtt_button1_blueledint"
#define  FUNCTION46     "button \"button4\" sends \"relay1/set/lamp toggle\""
#define  GETMAX46       5
#define  SETMAX46       0
const String sGet[GETMAX46]={"help","version","function","ip","all"};
const String sSet[SETMAX46]={};

#include "src/mqtt/D1_class_MqttClientKH.h"
#include "src/din/D1_class_Din.h"
#include "src/blueledint/D1_class_BlueLedInt.h"
#include "src/statemachine/D1_class_Statemachine.h"

//-----MQTT-----------------------------------------------------
//MqttClientKH client("..ssid..", "..password..","mqttservername");
MqttClientKH client("Raspi10", "12345678","10.1.1.1");

bool connectedOld;

//......supported mqtt receive topics...........................
#define TOPIC_B4_ALL             ("button4/#")
#define TOPIC_B4_BASIC           ("button4")
#define TOPIC_B4_GET             ("button4/get")
#define TOPIC_B4_SET             ("button4/set/")
#define TOPIC_B4_RET             ("button4/ret/")
//.....supported mqtt send topics...............................
#define TOPIC_B4_SWITCH          ("relay1/set/lamp")
#define PAYLOAD_B4_SWITCH        ("toggle")
#define TOPIC_B4_SWITCH_RET      ("relay1/ret/lamp")
//.....number of send action....................................
#define NR_B4_NOTHING            0     // # mqtt do nothing
#define NR_B4_HELP               0     // # mqtt send help
#define NR_B4_VERSION            1     // # mqtt send version
#define NR_B4_FUNCTION           2     // # mqtt send function
#define NR_B4_IP                 3     // # mqtt send ip
#define NR_B4_GETALL             4     // # mqtt send all topics
#define NR_B4_SWITCH             7     // # mqtt send switch cmd

//......"container" for all send actions........................
int     mqttSend=NR_B4_NOTHING;        // 0=nothing

//-----state machine--------------------------------------------
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_MQTTSEND          10     // 10x 100ms = 1.0sec
#define STATE_BLUELEDOFF       150     // 150x 100ms= 15sec
Statemachine statemachine(STATE_MAX, STATE_DELAY); //1..600
int state_blueledoff;

//-----sensors, actors------------------------------------------
#define PIN_BUTTON              D3
Din     button_(PIN_BUTTON);           // button at D3
BlueLedInt blueled;                    // blue led  @ D1 mini

#define LED_B4_LAST              0     // do last state
#define LED_B4_ON                1     // blue led on
#define LED_B4_OFF               2     // blue led off
#define LED_B4_ERROR             3     // error blinking 0.1:0.1
#define LED_B4_WAITING           4     // blinking 0.5:0.5s
#define LED_B4_PULS0             5     // on:off=0.1s:0.9s
#define LED_B4_PULS1             6     // on:off=0.9s:0.1s

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 //-----convert topic and payload to String---------------------
 String sTopic=String(topic);
 String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 if(DEBUG46)
  Serial.println("Message topic '"+sTopic+"': "+sPayload);
 //-----get request---------------------------------------------
 if(sTopic==TOPIC_B4_GET)
 {
  int i;
  for(i=0; i<GETMAX46; i++) {
   if(sGet[i]==sPayload) break;
  }
  if(i<GETMAX46) {                     // get payload found
   mqttSend|=(1<<i);                   // trigger get request
  }
 }
 //-----set request--------------------------------------------- 
 // ... no set topics defined ...
 //-----check for confirmation of primary function--------------
 if(sTopic==TOPIC_B4_SWITCH_RET)
 {
  if((sPayload=="0")||(sPayload=="1")) {
   String s1;
   if(sPayload=="0") { s1="off"; led(LED_B4_PULS0); }
                else { s1="on";  led(LED_B4_PULS1); }
   if(DEBUG46) Serial.println("lamp is "+s1);
  }
 }
}

//_____control led______________________________________________
void led(int shine)
{
 static int ledOldShine=0;
 if(DEBUG46) Serial.println("led("+String(shine)+")");
 if(shine==LED_B4_LAST) shine=ledOldShine;
 switch(shine)
 {
  case LED_B4_ON:       //.....led on...........................
   blueled.on(); break;
  case LED_B4_OFF:      //.....led off..........................
   blueled.off(); break;
  case LED_B4_ERROR:    //.....blinking on:off=0.1s:0.1s........
   blueled.blink(100,100); break;
  case LED_B4_WAITING:  //.....blinking on:off=0.5s:0.5s........
   blueled.blink(500,500); break;
  case LED_B4_PULS0:    //.....blinking on:off=0.1s:0.9s........
   blueled.blink(100,900); break;
  case LED_B4_PULS1:    //.....blinking on:off=0.9s:0.1s........
   blueled.blink(900,100); break;
  default: break;
 }
 if((shine!=LED_B4_ERROR)&&(shine!=LED_B4_LAST))
  ledOldShine=shine;
}

//_____send MQTT messages_______________________________________
bool sendMQTT(int what_)
{
 int num1;                             // number of action
 unsigned int mask1;                   // mask ...001000
 String t1, p1;                        // topic, payload
 bool r_=true;                         // return value
 //=====plausibility check======================================
 if(DEBUG46) Serial.print("sentMQTT(0x"+String(what_,16)+"): ");
 if(what_<1) {
  if(DEBUG46) Serial.println("Nothing to do.");
  return false;
 }
 //=====check for WLAN==========================================
 if(!client.connected())
 {
  led(LED_B4_ERROR);
  if(DEBUG46) Serial.println("Not connected!");
  return false;
 }
 if(DEBUG46) Serial.println();
 //=====messages for other devices==============================
 num1=NR_B4_SWITCH; //-----send toogle to switch----------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B4_SWITCH);
  p1=String(PAYLOAD_B4_SWITCH);
  if(client.publishString(t1,p1)) {    // publish...
   mqttSend&=(~mask1);                 // success: stopp sending
   blueled.blink(500,500);
  }
  else r_=false;
 }
 //=====messages for this device================================
 num1=NR_B4_GETALL; //-----send all messages--------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {                                     // set all send bits
  for(int j=0; j<GETMAX46; j++) mqttSend|=(1<<j);
  mqttSend&=(~mask1);                  // done: clear getall bit
 }
 num1=NR_B4_HELP; //-----send help info-------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B4_RET)+sGet[num1];
  p1="Possible payloads for get: ";
  for(int i=0; i<GETMAX46; i++) p1+=+"|"+sGet[i];
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B4_VERSION; //-----send version------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B4_RET)+sGet[num1];
  p1=String(VERSION46);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B4_FUNCTION; //-----send function----------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B4_RET)+sGet[num1];
  p1=String(FUNCTION46);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B4_IP; //-----send ip----------------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B4_RET)+sGet[num1];
  p1=client.getMyIP();
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 return r_;
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 {Serial.begin(115200); Serial.println("\n"+String(VERSION46));}
 //-----set actors----------------------------------------------
 state_blueledoff=-1;                  //no state 2 turn off led
 led(LED_B4_ON);                       // start: led on
 //-----setup mqtt and trigger toggle message-------------------
 client.addSubscribe(String(TOPIC_B4_ALL));
 client.addSubscribe(String(TOPIC_B4_SWITCH_RET));
 client.setCallback(callback);
 if(client.reconnect()) {
  led(LED_B4_OFF);                     // connected: led off
  connectedOld=true;
 } 
 else {
  led(LED_B4_ERROR);                   // error: blink quickly  
  connectedOld=false;
 }
}

//_____main loop________________________________________________
void loop() 
{
 int state=statemachine.loopBegin();        // state begin 
 //if(DEBUG46) Serial.print(String(state)+": ");
 //=====do every state==========================================
 //-----turn blue led off after 15 sec--------------------------
 if(state==state_blueledoff) {
  led(LED_B4_OFF);
  state_blueledoff=-1;
 }
 //-----check if connected (must always be called-for receive!)-
 if(client.isConnected()) 
 { //.....connected.............................................
  if(!connectedOld) {
   if(DEBUG46) Serial.println("---CONNECT: Last led state---");
   led(LED_B4_LAST);
   //.....send MQTT answer.......................................
   if((state%STATE_MQTTSEND)==5)
   {
    if(DEBUG46) {
     if(mqttSend>0) 
      Serial.println(String(state)+": mqttSend="+String(mqttSend));
    }
    if(mqttSend>0) sendMQTT(mqttSend);
   }
  }
  connectedOld=true;
 }
 else 
 { //.....not connected.........................................
 if(connectedOld) {
   if(DEBUG46) Serial.println("---NOT CONNECTED: Blink on!---");
   led(LED_B4_ERROR);
  }
  connectedOld=false;
 }
 //-----process button------------------------------------------
 if(button_.isFallingEdge())
 {
  delay(10);
  button_.isFallingEdge();
  mqttSend|=(1<<NR_B4_SWITCH);          // trigger toggle switch
  mqttSend|=(1<<NR_B4_FUNCTION);        // for documentation
  led(LED_B4_ON);
  state_blueledoff=statemachine.add(STATE_BLUELEDOFF);
  if(DEBUG46) Serial.println("button pressed"); 
  sendMQTT(mqttSend);
 }
 //=====do not every state======================================
 //=====prepare for next state and wait=========================
 statemachine.loopEnd();                    // state end
 //if(DEBUG46) Serial.println();
}

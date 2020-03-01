//_____D1_oop43_mqtt_button1_led56.ino________170721-200301_____
// Press button D3 to send a mqtt-request.
// Hardware: (1) WeMos D1 mini
//           (2) Protoboard with button at D3, 2pin duo led
//               between D5 and D6 (with 100 Ohm resistor)
// On start LED lights red and D1 mini tries to connect to a
// mqtt-broker. On success led shines green.
// When button D3 is pressed, D1 mini
//   + publishes topic "relay1/set/lamp" with payload "toggle"
//   + turns blue led on
// Then D1 mini waits for topic relay1/ret/lamp, payload 0 or 1.
// * If there is no answer, the blue led shines continuous
//   for 10 seconds
// * if the answer is 0, blue led blinks short pulses (1:9),
// * on answer 1 it blinks long pulses (9:1) for 15 seconds.
// * On all other payloads the blue led blinks quickly.
// Important: Example needs a broker and a relay, that sends
//   the answer message
// Created by Karl Hartinger, November 01, 2018.
// Modified: 200301 Use Mqtt2 instead of Mqtt, use statemachineBlink
// Released into the public domain.
#define  DEBUG43        true  //false
#define  VERSION43      "2020-03-01 D1_oop43_mqtt_button1_led56"
#define  FUNCTION43     "button \"button1\" sends \"relay1/set/relay toggle\""
#define  GETMAX43       6
#define  SETMAX43       1
const String sGet[GETMAX43]={"help","version","function","ip","all","relay"};
const String sSet[SETMAX43]={"blueled"};

#include "src/mqtt2/D1_class_MqttClientKH2.h"
#include "src/din/D1_class_Din.h"
#include "src/dout/D1_class_Dout.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"

//-----MQTT-----------------------------------------------------
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi10", "12345678","10.1.1.1");

//......supported mqtt receive topics...........................
#define TOPIC_B1_BASIC           ("button1")
#define TOPIC_B1_ALL             ("button1/#")
#define TOPIC_B1_GET             ("button1/get")
#define TOPIC_B1_SET             ("button1/set/")
#define TOPIC_B1_RET             ("button1/ret/")
//.....supported mqtt send topics...............................
#define TOPIC_B1_RELAY           ("relay1/set/relay")
#define PAYLOAD_B1_RELAY         ("toggle")
#define TOPIC_B1_RELAY_RET       ("relay1/ret/relay")
// TOPIC_B1_RELAY_RET does the same as sSet[0]="blueled"
//.....number of send action....................................
#define NR_B1_HELP               0     // # mqtt send help
#define NR_B1_VERSION            1     // # mqtt send version
#define NR_B1_FUNCTION           2     // # mqtt send function
#define NR_B1_IP                 3     // # mqtt send ip
#define NR_B1_GETALL             4     // # mqtt send all topics
#define NR_B1_RELAY              5     // # mqtt send relay cmd
//.....number of set action.....................................
// MUST BE the same number as index in sSet[] !!
#define NR_B1_SET_BLUELED        0     // # mqtt set blue led

//......"container" for all send actions........................
#define NR_B1_NOTHING            0     // # mqtt do nothing
int     mqttSend=NR_B1_NOTHING;        // 0=nothing
int     mqttSet=NR_B1_NOTHING;         // 0=nothing
String  mqttSetPayload[SETMAX43];      // 

//-----state machine--------------------------------------------
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_MQTTSEND          10     // 10x 100ms = 1.0sec
#define STATES_BLUELED         100     // 100x 100ms= 10sec
Statemachine statemachine(STATE_MAX, STATE_DELAY); //1..600

//-----sensors, actors------------------------------------------
#define  PIN_BUTTON             D3
#define  PIN_LED_BLUE           D4
#define  PIN_LED_GREEN          D5
#define  PIN_LED_RED            D6

Din  button_(PIN_BUTTON);              // button at D3
Dout ledGreen(PIN_LED_GREEN);          // green led @ protoboard
Dout ledRed(PIN_LED_RED);              // red led   @ protoboard
StatemachineBlink ledBlue(PIN_LED_BLUE,true,1,0,1,1);

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 //=====SECTION 1: convert payload to array (and show message)==
 char cPayload[length+1];                   //char-array payload 
 strncpy(cPayload,(char*)payload,length);   // copy payload bytes
 cPayload[length]=0;                        // set end char
 if(DEBUG43)
 { Serial.print("Message topic '"); Serial.print(topic);
   Serial.print("': "); Serial.println(cPayload);
 }
 //======SECTION 2: GET request -> trigger answer===============
 if(strcmp(topic,TOPIC_B1_GET)==0)
 {//-----check for valid get request (see array sGet[]----------
  int i;
  for(i=0; i<GETMAX43; i++) {
   if(strcmp(sGet[i].c_str(), cPayload)==0) break;
  }
  if(i<GETMAX43) {                     // get payload found
   mqttSend|=(1<<i);                   // trigger get request
  }
 }
 //======SECTION 3: SET request -> trigger action===============
 // do action in main loop to save time in callback function (!)
 int lenSet=strlen(TOPIC_B1_SET);
 if(strncmp(topic,TOPIC_B1_SET,lenSet)==0)
 {//-----set topic found: check for valid set request-----------
  int i;
  char* settype=topic;                // point to topic begin
  settype+=lenSet;                    // point to topic command
  for(i=0; i<SETMAX43; i++) {
   if(strcmp(settype,sSet[i].c_str())==0) break;   // set-command?
  }
  if(i<SETMAX43) {                     // get payload found
  if(DEBUG43) 
   {Serial.print("==> MQTT SET-Cmd: "); Serial.println(sSet[i].c_str());}
   mqttSet|=(1<<i);                    // trigger get request
   mqttSetPayload[i]=String(cPayload); // save payload
  }
 }
 //=====SECTION 4: special MQTT (input) messages================
 //-----receive relay answer------------------------------------
 if(strcmp(topic,TOPIC_B1_RELAY_RET)==0) 
 {
  int i=NR_B1_SET_BLUELED;            // = 0
  mqttSet|=(1<<i);                    // trigger get request
  mqttSetPayload[i]=String(cPayload); // save payload
  if(DEBUG43) {Serial.print("==> Answer from relay");Serial.print(cPayload);}
 }
}

//_____send MQTT messages_______________________________________
// number=bit number in what_ (=mqttSend)
// 0=help, 1=version, 2=function, 3=ip, 4=all, 5=relay
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
 //=====messages for this device================================
 num1=NR_B1_GETALL; //-------send all messages------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {                                     // set all send bits
  for(int j=0; j<GETMAX43; j++) mqttSend|=(1<<j);
  mqttSend&=(~mask1);                  // done: clear getall bit
 }
 num1=NR_B1_HELP; //---------send help info---------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RET)+sGet[num1];
  p1="Possible payloads ";
  for(int i=0; i<GETMAX43; i++) p1+=+"|"+sGet[i];
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 num1=NR_B1_VERSION; //-------send version-----------------------
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
 num1=NR_B1_IP; //-----------send ip----------------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RET)+sGet[num1];
  p1=client.getsMyIP();
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 //=====messages for other devices==============================
 num1=NR_B1_RELAY; //--------send toogle to relay---------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_B1_RELAY);
  p1=String(PAYLOAD_B1_RELAY);
  r_&=client.publishString(t1,p1);     // publish
  mqttSend&=(~mask1);                  // stopp sending anyway
 }
 return r_;
}

//_____do set action____________________________________________
// number=bit number in what_ (=mqttSet)
// 0=blueled
bool doSetAction(int what_, int state)
{
 int num1;                                  // number of action
 unsigned int mask1;                        // mask ...001000
 String p1;                                 // topic, payload
 bool r_=true;                              // return value
 //=====plausibility check======================================
 if(DEBUG43) Serial.println("doSetAction(0x"+String(what_,16)+")");
 if(what_<1) {
  if(DEBUG43) Serial.println("Nothing to do.");
  return false;
 }
 //=====action==================================================
 num1=NR_B1_SET_BLUELED; //--0: set blue led state--------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // do this action?
 {//.....set-command[0]: set blue led...........................
  if(DEBUG43)
  {Serial.print("doSetAction: ");Serial.println(mqttSetPayload[num1]);}
  if(mqttSetPayload[num1]=="1")
  {
   ledBlue.setParams(state,9,1,STATES_BLUELED/10); //long blinks
   if(DEBUG43) Serial.println("Blue LED blinks LONG pulses");
  }
  else {
   if(mqttSetPayload[num1]=="0") 
   {
    ledBlue.setParams(state,1,9,STATES_BLUELED/10); //short blinks
    if(DEBUG43) Serial.println("Blue LED blinks SHORT pulses");
   }
   else
   {
    ledBlue.setParams(state,1,1,STATES_BLUELED/2); //quick blinks
    if(DEBUG43) Serial.println("Blue LED blinks QUICK pulses");
    r_=false;
   }
  }
  mqttSet&=(~mask1);                        // action done
 }
 return r_;
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 {Serial.begin(115200); Serial.println("\n"+String(VERSION43));}
 //-----set actors----------------------------------------------
 ledBlue.off();                               // blue LED off
 ledGreen.off();                              // red
 ledRed.on();                                 // red
 //-----setup mqtt and trigger toggle message-------------------
 client.addSubscribe(String(TOPIC_B1_ALL));
 client.addSubscribe(String(TOPIC_B1_RELAY_RET));
 client.addPublish(String(TOPIC_B1_RET));
 client.addPublish(String(TOPIC_B1_RELAY));
 client.setCallback(callback);
}

//_____main loop________________________________________________
void loop() 
{
 //======SECTION 1: action at the beginning of loop=============
 int state=statemachine.loopBegin();        // state begin
 String sSerial="";
 //-----do every state------------------------------------------
 if(DEBUG43) { sSerial=String(state); sSerial+=+": "; }
  //.....process button..........................................
 if(button_.isFallingEdge())
 {
  delay(10);
  button_.isFallingEdge();
  mqttSend|=(1<<NR_B1_RELAY);           // trigger toggle switch
  if(DEBUG43) Serial.println("button pressed"); 
  ledBlue.setParams(state,STATES_BLUELED,2,1);
  sendMQTT(mqttSend);
 }
 //-----do every state: set action to be done?------------------
 if(DEBUG43) {
  if(mqttSet>0){sSerial+=" mqttSet=0x"; sSerial+=String(mqttSet,16);}
 }
 if(mqttSet>0) doSetAction(mqttSet, state);
 //-----do every state: check WiFi and MQTT---------------------
 if(!client.isWiFiConnected())
 {
  //=====SECTION 2: action for WiFi=NO, MQTT=NO=================
  ledBlue.off();
  ledGreen.off(); ledRed.on();                // red
 }
 else
 {
  //=====SECTION 3: action for WiFi=YES, MQTT=DONT_CARE=========
  //mqttState=NR_WLAN_OK;                     // WLAN yes
  if(client.isConnected()) //MUST always be called (for receive!)
  {
   //====SECTION 4: action for WiFi=YES, MQTT=YES===============
   ledGreen.on(); ledRed.off();               // green
   //.....if it is time: send MQTT answer.......................
   if((state%STATE_MQTTSEND)==0) {
    if(DEBUG43) {
     if(mqttSend>0){sSerial+=" mqttSend=0x"; sSerial+=String(mqttSend,16);}
    }
    if(mqttSend>0) sendMQTT(mqttSend); 
   }
  }
  else
  {//-----WiFi=yes, MQTT=no-------------------------------------
   client.state();                        // call for details
   ledGreen.on(); ledRed.on();            // orange
  }
 }
 //=====SECTION 5: action for WiFi=DONT_CARE, MQTT=DONT_CARE====
 ledBlue.doBlink(statemachine);            // must be section 5!
 //=====SECTION 6: prepare for next state and wait==============
 statemachine.loopEnd();                   // state end
 //if(DEBUG43)Serial.println(sSerial);     // show every state
 //.....show only non empty states..............................
 if(DEBUG43){ if(sSerial.length()>6) Serial.println(sSerial); }
}

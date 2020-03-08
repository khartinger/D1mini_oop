//_____D1_oop71_mqtt2_blueled.ino_____________200308-200308_____
// 1. The blue led of a D1 mini can be controlled by MQTT:
//  * receive a message oop71/set/blueled with topic 0, 1 or 2 
//    turns the blue led off, on or toggles it.
//  * If there is no connection to the mqtt server,
//    the blue led blinks quickly 
// 2. Structure of MQTT messages:
//  * request oop71/get help         : ask for all sGet-, sSet-
//                                     and sAct-topics
//  * request oop71/get version      : ask for version of program
//                    (e.g. 2020-03-08 D1_oop71_mqtt2_blueled)
//  * request oop71/get ip           : ask for ip of the D1mini
//  * request oop71/get blueled      : ask for state of blue led
//  * command oop71/set/blueled 0|1|2: set blue led off|on|toggle
//  * answer  oop71/ret/blueled 0|1  : send answer get-request
// 3. State machine: cycle time 1 minute (600 states each 0.1s)
// Hardware: 1. WeMos D1 Mini
// Created by Karl Hartinger, March 08, 2020.
// Changes: -
// Released into the public domain.

#include "src/mqtt2/D1_class_MqttClientKH2.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"

#define  DEBUG71        false //true
#define  VERSION71      "2020-03-08 D1_oop71_mqtt2_blueled"

//_____global variables_________________________________________
const  int pinBlueLED=2;               // Pin # blue LED = D4
int    iBlueLed=0;                     // 0=off, 1=on, 4=blink
bool   quickBlinking=true;             // ...when not connected

//_____MQTT communication_______________________________________
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi11", "12345678","10.1.1.1");
//......MQTT topics and payloads the D1 mini handles............
#define GETMAX71                 4     // number of get commands
#define SETMAX71                 1     // number of set commands
#define RETMAX71          GETMAX71     // usually same as GETMAX
#define PUBMAX71                 0     // special topics publish
#define SUBMAX71                 0     // special topics subscribe

const   String sGet[GETMAX71]={"help","version","ip","blueled"};
const   String sSet[SETMAX71]={"blueled"};
const   String sSub[SUBMAX71]={};
const   String sPub[PUBMAX71]={};
//......array to save the payloads of set- or ret-messages......
String  sSetPayload[SETMAX71];
String  sRetPayload[RETMAX71];
String  sSubPayload[SUBMAX71];
String  sPubPayload[PUBMAX71];
//......"container" for all get/set/ret/act actions to do.......
#define NR_71_NOTHING            0     // # mqtt do nothing
int     iGet=NR_71_NOTHING;            // 0=nothing
int     iSet=NR_71_NOTHING;            // 0=nothing
int     iRet=NR_71_NOTHING;            // 0=nothing
int     iSub=NR_71_NOTHING;            // 0=nothing
int     iPub=NR_71_NOTHING;            // 0=nothing
//......supported mqtt topics...................................
#define TOPIC_71_BASIC           ("oop71/")
#define TOPIC_71_ALL             ("oop71/#")
#define TOPIC_71_GET             ("oop71/get")
#define TOPIC_71_SET             ("oop71/set/")
#define TOPIC_71_RET             ("oop71/ret/")

//_____state machine____________________________________________
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_MQTTSEND           5     // 0.5sec answer time
Statemachine stm(STATE_MAX, STATE_DELAY);
StatemachineBlink led1(pinBlueLED, true, 1, 1, 1,-1);

//_____MQTT: process all subscribed incoming messages___________
// * If get topics is valid, the appropriate bit is set in iGet
// * If set topics is valid, the appropriate bit is set in iSet
//   and the payload is copied to sSetPayload[]
// * Special topics are specially treated
void callback(char* topic, byte* payload, unsigned int length)
{
 //=====SECTION 1: convert payload to array (and show message)==
 char cPayload[length+1];                   //char-array payload 
 strncpy(cPayload,(char*)payload,length);   // copy payload bytes
 cPayload[length]=0;                        // set end char
 if(DEBUG71)
 { Serial.print("Message topic '"); Serial.print(topic);
   Serial.print("': "); Serial.println(cPayload);
 }
 //======SECTION 2: GET request -> trigger answer===============
 if(strcmp(topic,TOPIC_71_GET)==0)
 {//-----check for valid get request (see array sGet[]----------
  int i;
  for(i=0; i<GETMAX71; i++) {
   if(strcmp(sGet[i].c_str(), cPayload)==0) break;
  }
  if(i<GETMAX71) {                     // get payload found
   iGet|=(1<<i);                       // trigger get request
  }
 }
 //======SECTION 3: SET request -> trigger action===============
 // do action in main loop to save time in callback function (!)
 int lenSet=strlen(TOPIC_71_SET);      // length of "oop71/set/"
 if(strncmp(topic,TOPIC_71_SET,lenSet)==0) // check first part
 {//-----correct first part of set topic found------------------
  // check for valid 2nd part of set request (= sSet[])
  int i;                               // sSet array index
  char* settype=topic;                 // point to topic begin
  settype+=lenSet;                     // point to topic command
  for(i=0; i<SETMAX71; i++) {
   if(strcmp(settype,sSet[i].c_str())==0) break; // set-command?
  }
  if(i<SETMAX71) {                     // get payload found
   iSet|=(1<<i);                       // trigger set request
   sSetPayload[i]=String(cPayload);    // save payload
  }
 }
 //=====SECTION 4: special MQTT (input) messages================
 //.....no special topics to listen for.....
}

//_____MQTT: send a requested message___________________________
// uses   : iRet (number of return message = bit number in iRet)
// index i: 0=help, 1=version, 2=ip, 3=blueled
// return : true=success, false=error
bool sendRet()
{
 unsigned int mask1;                   // mask ...001000
 String t1;                            // topic
 bool r_=true;                         // return value
 //=====plausibility check======================================
 if((iRet<1)&&(iPub<1)) return false;
 //=====check for WLAN==========================================
 if(!client.connected()) return false;
 //=====answers (messages) sent from this device================
 for(int i=0; i<RETMAX71; i++)
 {
  mask1=1<<i;                          // mask
  if((iRet&mask1)>0)                   // send this topic?
  {
   t1=String(TOPIC_71_RET)+sGet[i];    // build topic
   r_&=client.publishString(t1,sRetPayload[i]);  // publish
  }
 }
 //=====special messages sent from this device==================
 for(int i=0; i<PUBMAX71; i++)
 {
  mask1=1<<i;                          // mask
  if((iPub&mask1)>0)                   // send this topic?
  {
   r_&=client.publishString(sPub[i],sPubPayload[i]); // publish
  }
 }
 //=====finish sending answer===================================
 iPub=NR_71_NOTHING;                   // sending done
 iRet=NR_71_NOTHING;                   // sending done
 return r_;                            // send state
}

//_____answer get requests______________________________________
// uses   : iGet, iRet
// index i: 0=help, 1=version, 2=ip, 3=blueled
// return : true=success, false=error
bool doGet()
{
 unsigned int mask1;                   // mask ...001000
 String p1;                            // help string
 bool r_=true;                         // return value
 //=====plausibility check======================================
 if(iGet<1) return false;
 //=====build answer for get-request============================
 for(int i=0; i<GETMAX71; i++)
 {
  mask1=1<<i;                          // mask
  if((iGet&mask1)>0)                   // "get" for this topic?
  {
   switch(i)
   {
    case 0: //-----help-----------------------------------------
     p1="get: ";                                    // list all..
     for(int i=0; i<GETMAX71; i++) p1+=sGet[i]+"|"; // ..sGet[]
     p1+="\r\nset: ";                               // list all..
     for(int i=0; i<SETMAX71; i++) p1+=sSet[i]+"|"; // ..sSet[]
     sRetPayload[i]=p1;                // set payload
     iRet|=(1<<i); break;              // trigger mqtt answer
    case 1: //-----version--------------------------------------
     sRetPayload[i]=String(VERSION71); // string version
     iRet|=(1<<i); break;              // trigger mqtt answer
    case 2: //-----ip-------------------------------------------
     sRetPayload[i]=client.getsMyIP(); // get IP as string
     iRet|=(1<<i); break;              // trigger mqtt answer
    case 3: //-----state of blue led----------------------------
     sRetPayload[i]=String(iBlueLed);  // state of blue led
     iRet|=(1<<i); break;              // trigger mqtt answer
    default: break;                    // wrong index
   }
  }
 }
 iGet=NR_71_NOTHING;                   // get request done
 return r_;                            // get done result
}

//_____answer set requests______________________________________
// uses   : iSet, iRet
// index i: 0=blueled
// return : true=success, false=error
bool doSet()
{
 unsigned int mask1;                   // mask ...001000
 String p1;                            // help string
 bool r_=true;                         // return value
 //=====plausibility check======================================
 if(iSet<1) return false;
 //=====build answer for get-request============================
 for(int i=0; i<SETMAX71; i++)
 {
  mask1=1<<i;                          // mask
  if((iSet&mask1)>0)                   // set command
  {
   switch(i)
   {
    case 0: //-----set blue led---------------------------------
     char cLed;
     cLed=(char)sSetPayload[0][0];
     if(cLed=='2') cLed='0'+(1-iBlueLed);
     if(cLed=='0') {
      led1.setParams(stm.getState(),1,19,-1); 
      iBlueLed=0;
      quickBlinking=false;
     }
     if(cLed=='1') {
      led1.setParams(stm.getState(),19,1,-1);
      iBlueLed=1;
      quickBlinking=false;
     }
     iRet|=(1<<3);                     //trigger blueled answer!!!
     sRetPayload[3]=String(iBlueLed);  // payload for blueled
     break;              
    default: break;                    // wrong index
   }
  }
 }
 iSet=NR_71_NOTHING;                   // get request done
 return r_;                            // get done result
}

//_____SETUP____________________________________________________
void setup() {
 //-----Serial, just for debug----------------------------------
 //if(DEBUG71)
 {Serial.begin(115200); Serial.println("\nStart");}
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(TOPIC_71_ALL));
 for(int i=0; i<SUBMAX71; i++) client.addSubscribe(sSub[i]);
 client.addPublish(String(TOPIC_71_RET));
 for(int i=0; i<PUBMAX71; i++) client.addPublish(sPub[i]);
 client.setCallback(callback);
 led1.setParams(1, 1, 1,-1);
 quickBlinking=true;
 if(DEBUG71) Serial.println("setup() finished -> start loop()\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======SECTION 1: action at the beginning of loop=============
 int state=stm.loopBegin();            // state begin 
 String sSerial="";
 //-----do every state------------------------------------------
 if(DEBUG71) { sSerial=String(state); sSerial+=+": "; }
 //-----do every state: check WiFi and MQTT---------------------
 if(!client.isWiFiConnected())
 {
  //=====SECTION 2: action for WiFi=NO, MQTT=NO=================
  if(!quickBlinking) {                 // no Wifi - no blinking
   led1.setParams(state, 1, 1,-1);     // start led..
   quickBlinking=true;                 // ..blink quickly
  }
 }
 else
 {
  //=====SECTION 3: action for WiFi=YES, MQTT=DONT_CARE=========
  if(client.isConnected()) //MUST always be called (for receive!)
  {
   //====SECTION 4: action for WiFi=YES, MQTT=YES===============
   //.....if it is time: send MQTT answer.......................
   if((state%STATE_MQTTSEND)==0) {
    if(DEBUG71){if(iRet>0){sSerial+=" iRet=0x"+String(iRet,16);}}
    if((iRet>0)||(iPub>0)) sendRet();
   }
   if(quickBlinking)                   // blinking = no connect
   {//.....stopp display "not connected": set old LED state.....
    if(iBlueLed==0) led1.setParams(stm.getState(),1,19,-1);
               else led1.setParams(stm.getState(),19,1,-1);
    quickBlinking=false;
   }
  }
  else
  {//====SECTION 5: action for WiFi=YES, MQTT=NO================
   client.state();                     // call for details
  }
 }
 //======SECTION 6: action for WiFi=DONT_CARE, MQTT=DONT_CARE===
 //-----do every state: get action to be done?------------------
 if(DEBUG71) {if(iGet>0){sSerial+=" sGet=0x"+String(iGet,16);}}
 if(iGet>0) doGet(); 
 //-----do every state: set action to be done?------------------
 if(DEBUG71) {if(iSet>0){sSerial+=" sSet=0x"+String(iSet,16);}}
 if(iSet>0) doSet(); 
 //=====SECTION 6: prepare for next state and wait==============
 led1.doBlink(stm);                    // blue led action
 stm.loopEnd();                        // state end
 //.....show only non empty states..............................
 if(DEBUG71){ if(sSerial.length()>5) Serial.println(sSerial); }
}

//_____D1_oop72_mqtt2_button2blueled.ino______200308-200308_____
// 1. The button of a D1 mini button shield toggles the blue led
//    of another D1 mini by MQTT:
// *  when the button is pressed, a MQTT message 
//    oop71/set/blueled 2 is sent.
//  * receive a message oop71/ret/blueled OR oop72/set/blueled
//    with topic 0, 1 or 2 turns blue led off, on or toggles it.
//  * If there is no connection to the mqtt server,
//    the blue led blinks quickly 
// 2. Structure of MQTT messages:
//  * request oop72/get help         : ask for all sGet-, sSet-
//                                     and sAct-topics
//  * request oop72/get version      : ask for version of program
//               (e.g. 2020-03-08 D1_oop72_mqtt2_button2blueled)
//  * request oop72/get ip           : ask for ip of the D1mini
//  * request oop72/get blueled      : ask for state of blue led
//  * command oop72/set/blueled 0|1|2: set blue led off|on|toggle
//  * command oop72/ret/blueled 0|1|2: set blue led off|on|toggle
//  * answer  oop72/ret/blueled 0|1  : send answer get-request
// 3. State machine: cycle time 1 minute (600 states each 0.1s)
// Hardware: 1. WeMos D1 Mini
//           2. Button shield (D3)
// Created by Karl Hartinger, March 08, 2020.
// Changes: -
// Released into the public domain.

#include "src/mqtt2/D1_class_MqttClientKH2.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"
#include "src/din/D1_class_Din.h"

#define  DEBUG72        false //true
#define  VERSION72      "2020-03-08 D1_oop72_mqtt2_button2blueled"

//_____global variables_________________________________________
const  int pinBlueLED=2;               // Pin # blue LED = D4
int    iBlueLed=0;                     // 0=off, 1=on, 4=blink
bool   quickBlinking=true;             // ...when not connected
Din    button;                         // button D3

//_____MQTT communication_______________________________________
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi11", "12345678","10.1.1.1");
//......MQTT topics and payloads the D1 mini handles............
#define GETMAX72                 4     // number of get commands
#define SETMAX72                 1     // number of set commands
#define RETMAX72          GETMAX72     // usually the same
#define SUBMAX72                 1     // special topics subscribe
#define PUBMAX72                 1     // special topics publish
const   String sGet[GETMAX72]={"help","version","ip","blueled"};
const   String sSet[SETMAX72]={"blueled"};
const   String sSub[SUBMAX72]={"oop71/ret/blueled"};
const   String sPub[PUBMAX72]={"oop71/set/blueled"};
//......array to save the payloads of set- or ret-messages......
String  sSetPayload[SETMAX72];
String  sRetPayload[RETMAX72];
String  sSubPayload[SUBMAX72];
String  sPubPayload[PUBMAX72];
//......"container" for all get/set/ret/act actions to do.......
#define NR_72_NOTHING            0     // # mqtt do nothing
int     iGet=NR_72_NOTHING;            // 0=nothing
int     iSet=NR_72_NOTHING;            // 0=nothing
int     iRet=NR_72_NOTHING;            // 0=nothing
int     iSub=NR_72_NOTHING;            // 0=nothing
int     iPub=NR_72_NOTHING;            // 0=nothing
//......supported mqtt topics...................................
#define TOPIC_72_BASIC           ("oop72/")
#define TOPIC_72_ALL             ("oop72/#")
#define TOPIC_72_GET             ("oop72/get")
#define TOPIC_72_SET             ("oop72/set/")
#define TOPIC_72_RET             ("oop72/ret/")

//_____state machine____________________________________________
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_MQTTSEND           2     // 0.2sec answer time
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
 if(DEBUG72)
 { Serial.print("Message topic '"); Serial.print(topic);
   Serial.print("': "); Serial.println(cPayload);
 }
 //======SECTION 2: GET request -> trigger answer===============
 if(strcmp(topic,TOPIC_72_GET)==0)
 {//-----check for valid get request (see array sGet[]----------
  int i;
  for(i=0; i<GETMAX72; i++) {
   if(strcmp(sGet[i].c_str(), cPayload)==0) break;
  }
  if(i<GETMAX72) {                     // get payload found
   iGet|=(1<<i);                       // trigger get request
  }
 }
 //======SECTION 3: SET request -> trigger action===============
 // do action in main loop to save time in callback function (!)
 int lenSet=strlen(TOPIC_72_SET);      // length of "oop72/set/"
 if(strncmp(topic,TOPIC_72_SET,lenSet)==0) // check first part
 {//-----correct first part of set topic found------------------
  // check for valid 2nd part of set request (= sSet[])
  int i;                               // sSet array index
  char* settype=topic;                 // point to topic begin
  settype+=lenSet;                     // point to topic command
  for(i=0; i<SETMAX72; i++) {
   if(strcmp(settype,sSet[i].c_str())==0) break; // set-command?
  }
  if(i<SETMAX72) {                     // get payload found
   iSet|=(1<<i);                       // trigger set request
   sSetPayload[i]=String(cPayload);    // save payload
  }
 }
 //=====SECTION 4: special MQTT (input) messages================
 if(strcmp(topic,sSub[0].c_str())==0) {
  iSet|=(1<<0);                        // same as set blueled
  sSetPayload[0]=String(cPayload);     // save payload
 }
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
 for(int i=0; i<RETMAX72; i++)
 {
  mask1=1<<i;                          // mask
  if((iRet&mask1)>0)                   // send this topic?
  {
   t1=String(TOPIC_72_RET)+sGet[i];    // build topic
   r_&=client.publishString(t1,sRetPayload[i]);  // publish
  }
 } 
 //=====special messages sent from this device==================
 for(int i=0; i<PUBMAX72; i++)
 {
  mask1=1<<i;                          // mask
  if((iPub&mask1)>0)                   // send this topic?
  {
   r_&=client.publishString(sPub[i],sPubPayload[i]); // publish
  }
 } 
 //=====finish sending answer===================================
 iPub=NR_72_NOTHING;                   // sending done
 iRet=NR_72_NOTHING;                   // sending done
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
 for(int i=0; i<GETMAX72; i++)
 {
  mask1=1<<i;                          // mask
  if((iGet&mask1)>0)                   // "get" for this topic?
  {
   switch(i)
   {
    case 0: //-----help-----------------------------------------
     p1="get: ";                                    // list all..
     for(int i=0; i<GETMAX72; i++) p1+=sGet[i]+"|"; // ..sGet[]
     p1+="\r\nset: ";                               // list all..
     for(int i=0; i<SETMAX72; i++) p1+=sSet[i]+"|"; // ..sSet[]
     p1+="\r\nsub: ";                               // list all..
     for(int i=0; i<SUBMAX72; i++) p1+=sSub[i]+"|"; // ..sSub[]
     p1+="\r\npub: ";                               // list all..
     for(int i=0; i<PUBMAX72; i++) p1+=sPub[i]+"|"; // ..sPub[]
     sRetPayload[i]=p1;                // set payload
     iRet|=(1<<i); break;              // trigger mqtt answer
    case 1: //-----version--------------------------------------
     sRetPayload[i]=String(VERSION72); // string version
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
 iGet=NR_72_NOTHING;                   // get request done
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
 for(int i=0; i<SETMAX72; i++)
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
 iSet=NR_72_NOTHING;                   // get request done
 return r_;                            // get done result
}

//_____SETUP____________________________________________________
void setup() {
 //-----Serial, just for debug----------------------------------
 //if(DEBUG72)
 {Serial.begin(115200); Serial.println("\nStart");}
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(TOPIC_72_ALL));
 for(int i=0; i<SUBMAX72; i++) client.addSubscribe(sSub[i]);
 client.addPublish(String(TOPIC_72_RET));
 for(int i=0; i<PUBMAX72; i++) client.addPublish(sPub[i]);
 client.setCallback(callback);
 led1.setParams(1, 1, 1,-1);
 quickBlinking=true;
 if(DEBUG72) Serial.println("setup() finished -> start loop()\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======SECTION 1: action at the beginning of loop=============
 int state=stm.loopBegin();            // state begin 
 String sSerial="";
 //-----do every state------------------------------------------
 if(DEBUG72) { sSerial=String(state); sSerial+=+": "; }
 //-----do every state: check button----------------------------
 if(button.isFallingEdge())
 {
  sSerial+="Button pressed, ";  
  iPub|=(1<<0);
  sPubPayload[0]=String("2");
 }
 //======SECTION 2: check for WiFi and MQTT connection==========
 if(client.isWiFiConnected())
 {
  if(client.isConnected()) //MUST always be called (for receive!)
  {//====SECTION 3: action for WiFi=YES, MQTT=YES===============
   //.....if it is time: send MQTT answer.......................
   if((state%STATE_MQTTSEND)==0) {
    if(DEBUG72){if(iRet>0){sSerial+=" iRet=0x"+String(iRet,16);}}
    if((iRet>0)||(iPub>0)) sendRet();
   }
   if(quickBlinking)                   // blinking = no connect
   {//.....stopp display "not connected": set old LED state.....
    if(iBlueLed==0) led1.setParams(stm.getState(),1,19,-1);
               else led1.setParams(stm.getState(),19,1,-1);
    quickBlinking=false;
   }
  }//====END OF SECTION 3=======================================
  else
  {//====SECTION 4: action for WiFi=YES, MQTT=NO================
   client.state();                     // call for details
  }//====END OF SECTION 4=======================================
 }//=====END OF SECTION 2: End of if=true=======================
 else
 {//=====SECTION 5: action for WiFi=NO (-> MQTT=NO)=============
  if(!quickBlinking) {                 // no Wifi - no blinking
   led1.setParams(state, 1, 1,-1);     // start led..
   quickBlinking=true;                 // ..blink quickly
  }
  //=====END OF SECTION 5=======================================
 }//=====END OF SECTION 2: End of else==========================
 //======SECTION 6: action for WiFi=DONT_CARE, MQTT=DONT_CARE===
 //-----do every state: get action to be done?------------------
 if(DEBUG72) {if(iGet>0){sSerial+=" sGet=0x"+String(iGet,16);}}
 if(iGet>0) doGet(); 
 //-----do every state: set action to be done?------------------
 if(DEBUG72) {if(iSet>0){sSerial+=" sSet=0x"+String(iSet,16);}}
 if(iSet>0) doSet(); 
 //======SECTION 7: prepare for next state and wait=============
 led1.doBlink(stm);                    // blue led action
 stm.loopEnd();                        // state end
 //.....show only non empty states..............................
 if(DEBUG72){ if(sSerial.length()>5) Serial.println(sSerial); }
}

//_____D1_oop70_MQTT_DIO_oled066_hut_monitor.ino_181101-200215_____
// 1. D1 mini works as a MQTT client in a little hut:
//  * Measure temperature, humidity and pressure indoor and 
//    outdoor with two BME280 every 2 seconds
//  * Measure brightness indoor and outdoor with 
//    two BH1750 every 2 seconds
//  * Measure lamp state and current (mA)
//  * Watch a door state (open, close, lock)
//  * Display status (measurement values, DIO) on an OLED 0.66"
//    every 2s
//  * Send MQTT topic hut/ret/status every 10 minutes
//  * Ask for brokertime every 15 seconds
// 2. Structure of MQTT messages:
//  * Requests: hut/get sGet, hut/set/sSet value,
//  * Answers : hut/ret/sGet value
//  * Special : send getTime ?, receive brokertime yyyymmdd HHMMSS
//  Available MQTT topics are listed in arrays sGet and sSet
// 3. State machine cycle time: 1 minute (600 states with 0.1 sec)

// Hardware: 1. WeMos D1 Mini
//           2. Relais shield (pin changed to D5)
//           3. Button shield (D3)
//           4. Selfmade INA122 shield (+ current transformer)
//           5. selfmade I2C shield (to connect 2x BME280 + 2x BH1750)
//           6. selfmade IO shield
//           7. OLED Shield 0.66" (64x48 pixel)
// Sensors:
//           1. BME280 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//             (I2C-address 0x76 and 0x77)
//           2. BH1750 connected to I2C: SCL=D1, SDA=D2, GND, 3V3
//             (I2C-address 0x23 and 0x5C)
//           3. 1x red LED @ D8 (DOUT)
//           4. 1x switch  @ D6 for door open|close (DIN)
//           5. 1x switch  @ D7 for lock yes|no (DIN)
// Created by Karl Hartinger, November 01, 2018.
// Changes: 20-02-07 change MQTT topics, IO pins
// Released into the public domain.

#define  DEBUG70        false //true
#define  VERSION70      "2020-02-15 D1_oop70_MQTT_DIO_oled066_hut_monitor"
#define  FUNCTION70     "monitor a hut: door, lamp, temperature, ..."
#define  LANGUAGE70     'e'            // 'd' or 'e'
#define  MQTT_NAME      "hut"
#define  NO_BROKERTIME  "no time---"
#define  DEG            String(char(248))+String(char(67))
#define  GETMAX70       14
#define  SETMAX70       5
const String sGet[GETMAX70]={"help","version","function","ip","all","state","door","lamp","current","current0","sensor","debug","time","relay"};
const String sSet[SETMAX70]={"lamp","sensor","current0","debug","relay"};

#include "src/screen1a/D1_class_Screen1a.h"
#include "src/relay1/D1_class_relay1.h"
#include "src/door1/D1_class_Door1.h"
#include "src/sensor2/D1_class_Sensor2.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/mqtt2/D1_class_MqttClientKH2.h"
#include "src/din/D1_class_Din.h"

//-----MQTT-----------------------------------------------------
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi11", "12345678","10.1.1.1");

//......supported mqtt topics...................................
#define TOPIC_H1_BASIC           ("hut/")
#define TOPIC_H1_ALL             ("hut/#")
#define TOPIC_H1_GET             ("hut/get")
#define TOPIC_H1_SET             ("hut/set/")
#define TOPIC_H1_RET             ("hut/ret/")
#define TOPIC_H1_GETTIME         ("getTime")
#define TOPIC_H1_BROKERTIME      ("brokertime")

//.....number of send action....................................
// MUST BE the same number as index in sGet[] !!
#define NR_H1_HELP               0     // # mqtt send help
#define NR_H1_VERSION            1     // # mqtt send version
#define NR_H1_FUNCTION           2     // # mqtt send function
#define NR_H1_IP                 3     // # mqtt send function
#define NR_H1_GETALL             4     // # mqtt send status
#define NR_H1_STATE              5     // # mqtt send status
#define NR_H1_DOOR               6     // # mqtt send door info
#define NR_H1_LAMP               7     // # mqtt send lamp info
#define NR_H1_CURRENT            8     // # mqtt send current
#define NR_H1_CURRENT0           9     // # mqtt send current
#define NR_H1_SENSOR            10     // # mqtt send setup sensor
#define NR_H1_DEBUG             11     // # mqtt send debug info
#define NR_H1_GETTIME           12     // # mqtt request time
#define NR_H1_RELAY             13     // # mqtt send relay state
//.....number of set action.....................................
// MUST BE the same number as index in sSet[] !!
#define NR_H1_SET_LAMP           0     // # mqtt set lamp
#define NR_H1_SET_SENSOR         1     // # mqtt (re)set sensor
#define NR_H1_SET_CURRENT0       2     // # mqtt set current on
#define NR_H1_SET_DEBUG          3     // # mqtt set debug on/off
#define NR_H1_SET_RELAY          4     // # mqtt set relay

//......"container" for all send/receive actions................
#define NR_H1_NOTHING            0     // # mqtt do nothing
int     mqttSend=NR_H1_NOTHING;        // 0=nothing
int     mqttSet=NR_H1_NOTHING;         // 0=nothing
String  mqttSetPayload[SETMAX70];      // 

//.....WIFI state numbers.......................................
#define NR_WLAN_WAITING          0     //
#define NR_WLAN_OK               1     //
#define NR_WLAN_ERROR            2     //
#define NR_MQTT_WAITING          3     //
#define NR_MQTT_OK               4     //
#define NR_MQTT_ERROR            5     //
#define NR_MQTT_TOO_LONG         6     //
#define NR_MQTT_UNKNOWN          7     //
const String sMqttState[]={"WLAN ....", "WLAN OK", "WLAN ERROR", 
 "MQTT ....", "MQTT OK", "MQTT ERROR", "MQTT 2LONG", "MQTT ???"};
int    mqttState=NR_MQTT_UNKNOWN;      // 
int    mqttStateOld=NR_MQTT_UNKNOWN;   // 

//-----state machine--------------------------------------------
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATE_REFRESH_OLED1     20     // 20x 100ms = 2sec
#define STATE_MQTTSEND          15     // 1.5sec answer time
#define MINUTECOUNTER_MAX       10     // 10 min
Statemachine statemachine(STATE_MAX, STATE_DELAY); //1..600

//-----sensors--------------------------------------------------
#define OLED_RESET               0     // OLED_RESET=GPIO 0
Screen1   display_;                    // display object
Door1     door1(D6,D7,D8,LANGUAGE70);  // doorpins open|lock|led
Relay1    lamp1(D5,2000,100);          // lamp pin,max_mA,on_mA
Sensor2   sensor2;                     // 2xbme280, 2xbh1750
Din       button(D3);                  // switch lamp on|off

//-----global values--------------------------------------------
String sBrokertime=NO_BROKERTIME;      // yyyymmdd HHMMSS
bool   bSendDebugPeriodic=false;       //send debug info by MQTT

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 //=====SECTION 1: convert payload to array (and show message)==
 char cPayload[length+1];                   //char-array payload 
 strncpy(cPayload,(char*)payload,length);   // copy payload bytes
 cPayload[length]=0;                        // set end char
 if(DEBUG70)
 { Serial.print("Message topic '"); Serial.print(topic);
   Serial.print("': "); Serial.println(cPayload);
 }
 //======SECTION 2: GET request -> trigger answer===============
 if(strcmp(topic,TOPIC_H1_GET)==0)
 {//-----check for valid get request (see array sGet[]----------
  int i;
  for(i=0; i<GETMAX70; i++) {
   if(strcmp(sGet[i].c_str(), cPayload)==0) break;
  }
  if(i<GETMAX70) {                     // get payload found
   mqttSend|=(1<<i);                   // trigger get request
  }
 }
 //======SECTION 3: SET request -> trigger action===============
 // do action in main loop to save time in callback function (!)
 int lenSet=strlen(TOPIC_H1_SET);
 if(strncmp(topic,TOPIC_H1_SET,lenSet)==0)
 {//-----set topic found: check for valid set request-----------
  int i;
  char* settype=topic;                // point to topic begin
  settype+=lenSet;                    // point to topic command
  for(i=0; i<SETMAX70; i++) {
   if(strcmp(settype,sSet[i].c_str())==0) break;   // set-command?
  }
  if(i<SETMAX70) {                     // get payload found
  if(DEBUG70) 
   {Serial.print("==> MQTT SET-Cmd: "); Serial.println(sSet[i].c_str());}
   mqttSet|=(1<<i);                    // trigger get request
   mqttSetPayload[i]=String(cPayload); // save payload
  }
 }
  
 //=====SECTION 4: special MQTT (input) messages================
 //-----receive brokertime--------------------------------------
 if(strcmp(topic,TOPIC_H1_BROKERTIME)==0) 
 {
  sBrokertime=String(cPayload);
 }
} 

//_____display io status on oled________________________________
//     Z|1234567890
//     -+----------
//     1|Out -12.6°
//     2|In   23.6°
//     3|Door:       (inverted)
//     4|locked      (open, closed, unclear)
//     5|1: xxx mA   (1 = Lamp on, 0 = Lamp off)
//     6|MQTT OK
void showOled066() { for(int i=1; i<7; i++) showOled066(i, ""); }
void showOled066(int line) { showOled066(line, ""); }
void showOled066(int line, String text)
{
 static int iBrokertimePart=1;
 switch(line) {
  case 0: //-----clear screen-----------------------------------
   display_.screen6Clear(1,"");
   break;
  //-----show temperature---------------------------------------
  case 1: if(text=="") text="Out"+sensor2.getsToOled()+DEG;
   display_.screen6(1,text,'l');
   break;
  case 2: if(text=="") text="In "+sensor2.getsTiOled()+DEG;
   display_.screen6(2,text,'l');
   break;
  //-----show door state----------------------------------------
  case 3: 
   if(text=="") {
    if(LANGUAGE70=='d') text=display_.utf8ToOled("Türe:");
    else text=display_.utf8ToOled("Door:");
   }
   display_.screen6i(3,text,'l');
   break;
  case 4: if(text=="") text=door1.getsStateDoor();
   display_.screen6(4,text,'c');
   break;
  //-----current------------------------------------------------
  case 5: if(text=="") text=lamp1.getsStatusOled();
   display_.screen6(5,text,'l');
   break;
  //-----MQTT status--------------------------------------------
  case 6: 
   if(text=="")
   {
    text=sMqttState[mqttState];         // WLAN/MQTT state
    if(sBrokertime!=NO_BROKERTIME)
    {
     if(sBrokertime.length()==15)
     {
      if(iBrokertimePart==1)
      { 
       iBrokertimePart=2;
       if(LANGUAGE70=='d')
       {
        text=sBrokertime.substring(6,8); text+=".";
        text+=sBrokertime.substring(4,6); text+=".";
        text+=sBrokertime.substring(0,4);
       }
       else
       { 
        text=sBrokertime.substring(0,4); text+="-";
        text+=sBrokertime.substring(4,6); text+="-";
        text+=sBrokertime.substring(6,8);
       }
      } else 
      {
       iBrokertimePart=1;
       text=sBrokertime.substring(9,11); text+=":";
       text+=sBrokertime.substring(11,13); text+=":";
       text+=sBrokertime.substring(13,15);
      }
     }
    }
   } 
   display_.screen6i(6,text,'c');
   break;
  default: ;
 }
}

//_____send MQTT message________________________________________
// number=bit number in what_ (=mqttSend)
// 0=help, 1=version, 2=function, 3=ip, 4=all, 5=status,
// 6=door, 7=lamp, 8=current, 9=current0, 10=sensor (setup)
// 11=debug, 12=time (getBrokertime)
bool sendMQTT(int what_)
{
 int num1;                                  // number of action
 unsigned int mask1;                        // mask ...001000
 String t1, p1;                             // topic, payload
 bool r_=true;                              // return value
 //=====plausibility check======================================
 if(DEBUG70) Serial.println("sentMQTT(0x"+String(what_,16)+")");
 if(what_<1) {
  if(DEBUG70) Serial.println("Nothing to do.");
  return false;
 }
  //=====check for WLAN==========================================
 if(!client.connected())
 {
  if(DEBUG70) Serial.println("Not connected!");
  return false;
 }
 if(DEBUG70) Serial.println();
 //=====messages (topics) for other devices=====================
 // ...no messages...
 //=====messages from this device (e.g. answers)================
 num1=NR_H1_HELP; //-----0: send help info----------------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1="get: ";
  for(int i=0; i<GETMAX70; i++) p1+=sGet[i]+"|";
  p1+="\r\nset: ";
  for(int i=0; i<SETMAX70; i++) p1+=sSet[i]+"|";
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_VERSION; //-----1: send version---------------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1=String(VERSION70);
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_FUNCTION; //-----2: send function-------------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1=String(FUNCTION70);
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_IP; //-----3: send ip-------------------------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1=client.getsMyIP();
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_GETALL; //-----4: send all--------------------------
 mask1=1<<num1;                             // mask
 num1=NR_H1_STATE; //-----5: status----------------------------
 mask1=mask1 | (1<<num1);
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1="{\"Lamp\":"+String(lamp1.getLampstate())+",";
  p1+="\"Door\":\""+door1.getsStateDoor()+"\",";
  p1+=(sensor2.getsJson()).substring(1);
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_DOOR; //-----6: send door state---------------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1=door1.getsStateAll();
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_LAMP; //-----7: send lamp state---------------------
 mask1=1<<num1;                            // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  if(LANGUAGE70=='d')
   { if(lamp1.getLampstate()==1) p1="ein"; else p1="aus"; }
  else
   { if(lamp1.getLampstate()==1) p1="on"; else p1="off"; }
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_CURRENT; //-----8: send current answer--------------
 mask1=1<<num1;                        // mask
 if((what_&mask1)>0)                   // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1=String(lamp1.getCurrent());
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_CURRENT0; //-----9: send current limit answer-------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  p1=String(lamp1.getCurrentOn());
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_SENSOR; //-----10: send setup sensor answer---------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  int sensorstatus=sensor2.getStatus();
  if(sensorstatus==0) p1="Sensor OK";
  else p1="Sensorfehler "+sensor2.getsStatus();
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_DEBUG; //-----11: debug information-----------------
 mask1=1<<num1;                            // mask
 if((what_&mask1)>0)                       // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  char ca[48];
  uint16_t mil=statemachine.getBeginMillis();
  uint16_t block=ESP.getMaxFreeBlockSize();
  uint8_t  frag=ESP.getHeapFragmentation();
  int debug_=0; 
  if(bSendDebugPeriodic) debug_=1;
  sprintf(ca, "debug=%d: millis=%d maxBlock=%d frag=%d %%\0",debug_,mil,block,frag);
  p1=String(ca);
  r_&=client.publishString(t1,p1);         // publish
  mqttSend&=(~mask1);                      // stopp sending
 }
 num1=NR_H1_GETTIME; //---12: get brokertime--------------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_GETTIME);              // topic
  p1="?";                                   // payload
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
 num1=NR_H1_RELAY; //----13: send relay state---------------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // send this topic?
 {
  t1=String(TOPIC_H1_RET)+sGet[num1];
  if(LANGUAGE70=='d')
   { if(lamp1.getRelaystate()==1) p1="ein"; else p1="aus"; }
  else
   { if(lamp1.getRelaystate()==1) p1="on"; else p1="off"; }
  r_&=client.publishString(t1,p1);          // publish
  mqttSend&=(~mask1);                       // stopp sending
 }
}

//_____do set action____________________________________________
// number=bit number in what_ (=mqttSet)
// 0=lamp, 1=sensor, 2=current0, 3=debug
bool doSetAction(int what_)
{
 int num1;                                  // number of action
 unsigned int mask1;                        // mask ...001000
 String p1;                                 // topic, payload
 bool r_=true;                              // return value
 //=====plausibility check======================================
 if(DEBUG70) Serial.println("doSetAction(0x"+String(what_,16)+")");
 if(what_<1) {
  if(DEBUG70) Serial.println("Nothing to do.");
  return false;
 }
 //=====action==================================================
 num1=NR_H1_SET_LAMP; //----------0: set lamp state-------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // do this action?
 {//.....set-command[0]: set lamp on/off/toggle.................
  //if(DEBUG70)
  {Serial.print("doSetAction: ");Serial.println(mqttSetPayload[num1]);}
  if((mqttSetPayload[num1]=="on")||(mqttSetPayload[num1]=="1"))
  {
   lamp1.lampOn(); 
   mqttSend|=(1<<NR_H1_LAMP);
   if(DEBUG70) Serial.println("Lamp on");
  }
  if((mqttSetPayload[num1]=="off")||(mqttSetPayload[num1]=="0"))
  {
   lamp1.lampOff();
   mqttSend|=(1<<NR_H1_LAMP);
   if(DEBUG70) Serial.println("Lamp off");
  }
  if((mqttSetPayload[num1]=="toggle")||(mqttSetPayload[num1]=="-1"))
  {
   lamp1.lampToggle();
   mqttSend|=(1<<NR_H1_LAMP);
   if(DEBUG70) Serial.println("Lamp toggled");
  }
  mqttSet&=(~mask1);                        // action done
 }
 num1=NR_H1_SET_SENSOR; //--------1: setup sensor---------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0) {                      // do this action?
  sensor2.setup();                          // action
  mqttSend|=(1<<NR_H1_SENSOR);              // send mqtt answer
  mqttSet&=(~mask1);                        // action done
 }
 num1=NR_H1_SET_CURRENT0; //------2: set current on value-------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0) {                      // do this action?
  int ion=mqttSetPayload[num1].toInt();     // get mA value
  if((ion>0)&&(ion<=lamp1.getCurrentMax())) // value allowed?
   lamp1.setCurrentOn(ion);                 // -> set new value
  mqttSend|=(1<<NR_H1_CURRENT0);            // send mqtt answer
  mqttSet&=(~mask1);                        // action done
 }
 num1=NR_H1_SET_DEBUG; //---------3: switch mqtt debug on|off---
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0) {                      // do this action?
  if((mqttSetPayload[num1]=="off")||(mqttSetPayload[num1]=="0")
   ||(mqttSetPayload[num1]=="false")) bSendDebugPeriodic=false;
  if((mqttSetPayload[num1]=="on")||(mqttSetPayload[num1]=="1")
   ||(mqttSetPayload[num1]=="true")) bSendDebugPeriodic=true;
  mqttSend|=(1<<NR_H1_DEBUG);               // send debug info
  mqttSet&=(~mask1);                        // action done
 }
  num1=NR_H1_SET_RELAY; //---------4: set relay state------------
 mask1=1<<num1;                             // mask
 if((what_&mask1)>0)                        // do this action?
 {//.....set-command[0]: set relay on/off/toggle................
  //if(DEBUG70)
  {Serial.print("doSetAction: ");Serial.println(mqttSetPayload[num1]);}
  if((mqttSetPayload[num1]=="on")||(mqttSetPayload[num1]=="1"))
  {
   lamp1.on(); 
   mqttSend|=(1<<NR_H1_RELAY);
   if(DEBUG70) Serial.println("Relay on");
  }
  if((mqttSetPayload[num1]=="off")||(mqttSetPayload[num1]=="0"))
  {
   lamp1.off();
   mqttSend|=(1<<NR_H1_RELAY);
   if(DEBUG70) Serial.println("Relay off");
  }
  if((mqttSetPayload[num1]=="toggle")||(mqttSetPayload[num1]=="-1"))
  {
   lamp1.toggle();
   mqttSend|=(1<<NR_H1_RELAY);
   if(DEBUG70) Serial.println("Relay toggled");
  }
  mqttSet&=(~mask1);                        // action done
 }
}

//_____build output string lamp state___________________________
String lampstate(int nr)
{
 String s1;
 (nr==1)?s1=" LAMP **Changed**":s1=" LAMP  No change ";
 s1=s1+" |"+lamp1.getsStatusOled()+"|";
 return s1;
}

//_____build output string door state___________________________
String doorstate(int nr)
{
 String s1;
 (nr==1)?s1=" DOOR **Changed**":s1=" DOOR Not changed";
 s1=s1+" |"+door1.getsStateAll()+"|";        // 1|All|Door
 return s1;
}

//_____SETUP____________________________________________________
void setup() {
 bool btemp;
 int itemp;
 int i2cSensorStatus;
 //-----Serial, just for debug----------------------------------
 //if(DEBUG70)
 {Serial.begin(115200); Serial.println("\nStart");}
 //-----i2c: clear oled, write line 6---------------------------
 display_.begin();
 mqttState=NR_WLAN_WAITING;
 showOled066(0);                          //line0=clear screen
 showOled066(6,sMqttState[mqttState]);
 //-----i2c: setup sensor---------------------------------------
 sensor2.setup();
 i2cSensorStatus=sensor2.getStatus();
 if(i2cSensorStatus!=0)
 {Serial.print("I2C Sensorfehler "); Serial.println(i2cSensorStatus);}
 showOled066();
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(TOPIC_H1_ALL));
 client.addSubscribe(String(TOPIC_H1_BROKERTIME));
 client.addPublish(String(TOPIC_H1_RET));
 client.addPublish(String(TOPIC_H1_GETTIME));
 client.setMQTTClientName(MQTT_NAME);
 client.setCallback(callback);
 if(DEBUG70) Serial.println("setup() finished -> start loop()\n");
}

int minutecounter=0;
int state_mqttsend=0;
int temp=0;
String s1;

//_____LOOP_____________________________________________________
void loop() {
 //======SECTION 1: action at the beginning of loop=============
 int state=statemachine.loopBegin();        // state begin 
 String sSerial="";
 //-----do every state------------------------------------------
 if(DEBUG70) { sSerial=String(state); sSerial+=+": "; }
 //-----do every state: set action to be done?------------------
 if(DEBUG70) {
  if(mqttSet>0){sSerial+=" mqttSet=0x"; sSerial+=String(mqttSet,16);}
  }
 if(mqttSet>0) doSetAction(mqttSet);
 //-----do every state: check WiFi and MQTT---------------------
 if(!client.isWiFiConnected())
 {
  //=====SECTION 2: action for WiFi=NO, MQTT=NO=================
  mqttState=NR_WLAN_ERROR;                  // No WLAN
 }
 else
 {
  //=====SECTION 3: action for WiFi=YES, MQTT=DONT_CARE=========
  mqttState=NR_WLAN_OK;                     // WLAN yes
  if(client.isConnected()) //MUST always be called (for receive!)
  {
   //====SECTION 4: action for WiFi=YES, MQTT=YES===============
   mqttState=NR_MQTT_OK;                    // MQTT yes
   //.....if it is time: send MQTT answer.......................
   if((state%STATE_MQTTSEND)==0) {
    if(DEBUG70) {
     if(mqttSend>0){sSerial+=" mqttSend=0x"; sSerial+=String(mqttSend,16);}
    }
    if(mqttSend>0) sendMQTT(mqttSend);
   }
  }
  else
  {//-----WiFi=yes, MQTT=no-------------------------------------
   client.state();                        // call for details
   mqttState=NR_MQTT_ERROR;               // MQTT error
  }
 }
 
 //=====SECTION 5: action for WiFi=DONT_CARE, MQTT=DONT_CARE====
 //-----check button D3-----------------------------------------
 if(button.isFallingEdge()) lamp1.toggle();
 //-----check door state----------------------------------------
 temp=0;
 if(door1.changed()) { mqttSend|=(1<<NR_H1_DOOR); temp=1; }
 if(DEBUG70) sSerial+=String(doorstate(temp));
 //-----check lamp state (every second)-------------------------
  if((state%5)==0)
 {//.....check lamp state.......................................
  temp=0;
  if(lamp1.changed()) { mqttSend|=(1<<NR_H1_LAMP); temp=1; }
  if(DEBUG70) sSerial+=String(lampstate(temp));
 }
 //-----request broker time (every 15 sec)----------------------
 if((state%150)==0) {                       // every 15 sec
  sBrokertime=NO_BROKERTIME;
  mqttSend|=(1<<NR_H1_GETTIME);
 }
 //-----do every minute-----------------------------------------
 if(state==STATE_MAX)
 {
  //-----send system state every 10 minutes---------------------
  if((++minutecounter)>=MINUTECOUNTER_MAX)
  {
   minutecounter=0;
   mqttSend|=(1<<NR_H1_STATE);
   showOled066(0);                          //line0=clear screen
   showOled066();
   if(DEBUG70) sSerial+=" send MQTT Status";
  }
 }
 //-----update oled if WLAN state changed-----------------------
 if(mqttState!=mqttStateOld)
 {
  mqttStateOld=mqttState;                   //remember old state
  showOled066(6);
  if(DEBUG70){sSerial+="\n     mqttState=";sSerial+=String(mqttState);}
 }
 //-----cyclic update oled--------------------------------------
 if((state%STATE_REFRESH_OLED1)==0) 
 {
  showOled066();
  if(DEBUG70) sSerial+="\n     "+sensor2.getsJson();
 }
 //-----just for test: send millis() value----------------------
 if(bSendDebugPeriodic)
 {
  if((state%300)==1) mqttSend|=(1<<NR_H1_DEBUG); // every 30 secs
 }
 //=====SECTION 6: prepare for next state and wait==============
 statemachine.loopEnd();                   // state end
 //if(DEBUG70)Serial.println(sSerial);     // show every state
 //.....show only non empty states..............................
 if(DEBUG70){ if(sSerial.length()>5) Serial.println(sSerial); }
}

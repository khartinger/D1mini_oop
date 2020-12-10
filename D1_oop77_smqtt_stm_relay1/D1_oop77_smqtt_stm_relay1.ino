//_____D1_oop77_smqtt_stm_relay1.ino__________200714-201108_____
// This program for a D1 mini (or ESP32 D1mini) is used to
// switch a relay or (AC) light bulb on and off via MQTT.
// Further functions are:
// (1) Hardware test at the beginning of the program:
//    * Start message via serial interface (115200Bd)
//    * Switch on green LED
//    * Switch relay 2x, check the current and output the result
//      via the serial interface or display it via the DUO LED 
//      (green = OK, red = error)
//    * 4x red-green flashes
//    * Green LED flashes endlessly (short pulses)
// (2) Display of WiFi and MQTT connection status via a DUO LED 
//     (green on D6, red on D7):
//   * red: no network connection
//   * yellow: Connection to the network, but not to the MQTT-Broker
//   * green: Connected to the MQTT broker
// (3) Reply to the following MQTT requests as topic like
//     relay/1/ret/...
//     -t relay/1/get -m help
//     -t relay/1/get -m version
//     -t relay/1/get -m ip
//     -t relay/1/get -m topicbase
//     -t relay/1/get -m all
//     ... (see TOPIC_GET)
// (4) Executing the following set commands
//     -t relay/1/set/topicbase -m new_base
//     -t relay/1/set/... <eeprom|relay|current0|debug>
// (5) Time control by a state machine
// (6) Control of the relay switching by an ASM010 current
//     transformer (on an amplifier board D1_INA122)
//
// Hardware:
// (1) WeMos D1 mini
// (2) Relais shield (D1)
// (3) Selfconstruction shield D1_INA122 (amplifier INA122P)
//     with Current transformer ASM-010
// (4) LED at D8, Duo-LED at D6-green/D7-red
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, December 08, 2020.
// Changes:
// 2020-11-08 New
// Released into the public domain.

//#define ESP32D1         2              // ESP32 D1mini
#define D1MINI          1              // ESP8266 D1mini +pro
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/relay2AC/D1_class_Relay2AC.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"

#define  DEBUG77        true //false
#define  VERSION77      "2020-12-08 D1_oop77_smqtt_stm_relay1"
#define  FUNCTION77     "Switch a relay by \"relay/1/set/licht ein|aus|um\""
#define  TOPIC_BASE     "relay/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom,all,lamp,relay,current,current0,debug"
#define  TOPIC_SET      "topicbase,eeprom,relay,current0,debug"
#define  TOPIC_SUB      ""
#define  TOPIC_PUB      ""

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define  PIN_LED1       5             // D5 or D8...
 #define  PIN_LED_G     19
 #define  PIN_LED_R     23
 #define  PIN_RELAY     22
 Relay2AC relay1(PIN_RELAY,2,2.0,0.05); // relais Ioff=50mA
#else
 #define  PIN_LED1      D8             // D5 or D8...
 #define  PIN_LED_G     D6
 #define  PIN_LED_R     D7
 #define  PIN_RELAY     D1
 Relay2AC relay1(PIN_RELAY,2.0,0.05);  // relais Ioff=50mA 
#endif
bool     bSendDebugPeriodic=false;     //send debug info by MQTT


//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1");

//_______state machine__________________________________________
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
Statemachine stm(STATE_MAX, STATE_DELAY); //1..600
//StatemachineBlink(pin,invert,stateStart,statesOn,statesOff,blinkNum)
// PIN_LED1,false,1,1,0,1 means led is turned on (continuously)
StatemachineBlink led1(PIN_LED1,false,1,1,0,1);

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
 String p1;                            // help string
 //-------------------------------------------------------------
 if(sPayload=="version") return String(VERSION77);
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
 if(sPayload=="all") {
  p1 ="{\"Version\":\""+String(VERSION77)+"\",";
  p1+="\"IP\":\""+client.getsMyIP()+"\",";
  p1+="\"Lamp\":"+String(relay1.getCurrentStatus())+",";
  p1+="\"Relais\":"+String(relay1.getRelayStatus())+",";
  p1+="\"Current\":"+String(relay1.getCurrent(),3)+",";
  p1+="\"Current0\":"+String(relay1.getCurrentOn(),3)+",";
  p1+="\"NominalCurrent\":"+String(relay1.getNominalCurrent(),3)+",";
  p1+="\"Debug\":";
  if(bSendDebugPeriodic) p1+="\"on\""; else p1+="\"off\"";
  p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="lamp")  return String(relay1.getCurrentStatus());
 //-------------------------------------------------------------
 if(sPayload=="relay")  return String(relay1.getRelayStatus());
 //-------------------------------------------------------------
 if(sPayload=="current")  return String(relay1.getCurrent(),3);
 //-------------------------------------------------------------
 if(sPayload=="current0") return String(relay1.getCurrentOn(),3);
 //-------------------------------------------------------------
 if(sPayload=="debug") {
  if(bSendDebugPeriodic)
  {
   char ca[48];
   uint16_t mil=stm.getBeginMillis();
   //uint16_t block=ESP.getMaxFreeBlockSize();
   //uint8_t  frag=ESP.getHeapFragmentation();
   //sprintf(ca, "millis=%d maxBlock=%d frag=%d %%\0",mil,block,frag);
   sprintf(ca, "millis=%d",mil);
   return String(ca);
  }
 }
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 String p1;                            // help string
 float  iOn;                           // help value current
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {                  // new topic base?
  client.changeTopicBase(sPayload);         // change base
  return client.getsTopicBase();            // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom") {                     // erase eeprom?
  if(sPayload=="0" || sPayload=="erase")    // payload OK?
  {
   if(client.eepromEraseTopicBase()) return "erased";
   else return "not erased";                // return answer
  }
 }
 //-------------------------------------------------------------
 if(sTopic=="relay") {                      // set relay
  if(sPayload=="on" || sPayload=="1"){
   relay1.on(); p1=String("Relay on");
  }
  if(sPayload=="off" || sPayload=="0"){
   relay1.off(); p1=String("Relay off");
  }
  if(sPayload=="-1" || sPayload=="toggle"){
   relay1.toggle(); p1=String("Relay toggle");
  }
  if(DEBUG77) Serial.println(p1);
  return p1;
 }
 //-------------------------------------------------------------
 if(sTopic=="current0") {                  // set current limit
  
  iOn=atof(sPayload.c_str());
  if((iOn>0)&&(iOn<=relay1.getNominalCurrent()))
  relay1.setCurrentOn(iOn);
  p1=String("new limit ");
  return p1+sPayload;
 }
 //-------------------------------------------------------------
 // debug: turn sending debug info on or off
 if(sTopic=="debug") {                      // set debug mode
  if(sPayload=="on" || sPayload=="1"){
   bSendDebugPeriodic=true; p1=String("on");
  }
  else { bSendDebugPeriodic=false; p1=String("off"); }
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
}

//_______SETUP__________________________________________________
void setup() {
 //----- Serial, just for debug---------------------------------
 //if(DEBUG77)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
//-------set actors---------------------------------------------
 led1.doBlink(stm);                         //1.state: led D8 on
 pinMode(PIN_LED_G, OUTPUT);                // pin is output
 pinMode(PIN_LED_R, OUTPUT);                // pin is output
 //------set nominal current, test relay------------------------
 relay1.setNominalCurrent(2.00);            // adjust to 2.00 A
 if(relay1.isOK())                          // relay off-on-off
 {//.....current measuring ok...................................
  digitalWrite(PIN_LED_G, 1);               // green on
  digitalWrite(PIN_LED_R, 0);               // red off
  if(DEBUG77) Serial.println("Relay OK");   // serial message
 }
 else 
 {//.....relay did not switch or current measuring not ok.......
  digitalWrite(PIN_LED_G, 0);               // green on
  digitalWrite(PIN_LED_R, 1);               // red off
  if(DEBUG77) Serial.println("Relay Error");// serial message
 }
 relay1.off();                              // turn relay off
 delay(2500);                               // show result @ led
 //------duoled: change 4x green - red--------------------------
 for(int i=0; i<4; i++)
 {
  digitalWrite(PIN_LED_G, 1);               // green on
  digitalWrite(PIN_LED_R, 0);               // red off
  delay(400);
  digitalWrite(PIN_LED_G, 0);               // green off
  digitalWrite(PIN_LED_R, 1);               // red on
  delay(400);
 }
 digitalWrite(PIN_LED_G, 1);                // orange
 delay(1000);
 digitalWrite(PIN_LED_G, 0);                // just red
 // 1,1,24,-1 start with state 1, 100ms on, 2400ms off, endless
 led1.setParams(1,1,24,-1);                 //flash led1 endless
 //------show connecting procedure via Serial (just for fun)----
 digitalWrite(PIN_LED_G, 0);                // green off
 digitalWrite(PIN_LED_R, 1);                // red on
 client.connectingWiFiBegin();              // start connecting
 while(!client.connectingWiFi()) {          // connect to WiFi
  if(DEBUG77) Serial.println("setup(): No WiFi connection"); 
  delay(1000);
 };
 if(DEBUG77) Serial.println("setup(): WiFi connected, IP: "+client.getsMyIP());
 digitalWrite(PIN_LED_G, 1);                // green on +
 digitalWrite(PIN_LED_R, 1);                // red on = orange
 if(DEBUG77) Serial.println("setup(): ip: "+client.getsMyIP());
 //------setup mqtt---------------------------------------------
 client.setCallback(callback);
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();
 //------connect to mqtt broker---------------------------------
 if(client.connectMQTT())
 {
  digitalWrite(PIN_LED_G, 1);               // green on
  digitalWrite(PIN_LED_R, 0);               // red off
  if(DEBUG77) Serial.println("setup(): Connected to MQTT broker!");
 }
 else
 {
  if(DEBUG77) Serial.println("setup(): NOT connected to MQTT broker!");
 }
 //------setup finish-------------------------------------------
 if(DEBUG77) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG77) Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 int state=stm.loopBegin();                 // state begin 
 String sSerial="";
 //======(1) do, independent on the network, ...================
 //------do every state-----------------------------------------
 if(DEBUG77) { sSerial=String(state); sSerial+=+": "; }
 //------do every 0.5 seconds-----------------------------------
 if((state%10)==0)
 {//.....check lamp state.......................................
  sSerial+=String(relay1.getCurrent(),3);
  sSerial+="A ";
  if(relay1.isOn()) sSerial+="on  ";
              else  sSerial+="off ";
  if(relay1.isChange()) {
   client.simpleMqttDo("get","lamp","");
   sSerial+=" CHANGED";
  }
 }
 
 //======(2) do mqtt messages (send, receive)===================
 client.doLoop();                           // mqtt loop
 
 //======(3) do, depending on the network access, ...===========
 //------display WiFi/MQTT state on red/green-LED---------------
 if(client.isWiFiConnected())
 {
  if(client.isMQTTConnected())
  {//....WiFi yes, MQTT yes -> LED green........................
   digitalWrite(PIN_LED_G, 1);              // green on
   digitalWrite(PIN_LED_R, 0);              // red off
   //sSerial+=" WiFi yes+MQTT yes ";
  }
  else
  {//....WiFi yes, MQTT no -> LED orange........................
   digitalWrite(PIN_LED_G, 1);              // green on +
   digitalWrite(PIN_LED_R, 1);              // red on = orange
   //sSerial+=" WiFi yes+MQTT no ";
  }
 }
 else
 {//.....WiFi no -> LED red.....................................
  digitalWrite(PIN_LED_G, 0);               // green off
  digitalWrite(PIN_LED_R, 1);               // red on
  //sSerial+=" WiFi no ";
 }
 //======(4) do at the end of the loop ...======================
 led1.doBlink(stm);                    // led D8 action
 stm.loopEnd();                        // state end
 //------show only non empty states-----------------------------
 if(DEBUG77){ if(sSerial.length()>5) Serial.println(sSerial); }
}

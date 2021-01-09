//_____D1_oop43_smqtt_button3_led56.ino_______170431-210109_____
// Press button D3 to send a mqtt-request.
// Hardware: (1) WeMos D1 mini
//           (2) Protoboard with button at D3, 2pin duo led
//               between D5 and D6 (with 100 Ohm resistor)
// On start LED lights up red and D1 mini tries to connect to a
// mqtt-broker. On success led is green.
// When button D3 is pressed, D1 mini
//   + publishes topic "relay/1/set/relay" with payload "toggle"
//   + blue led to blinks quickly
// Then D1 mini waits for topic relay/1/ret/lamp, payload 0 or 1
// and shows the result for 6 seconds using the blue led:
// (1) If there is no answer, the blue led shines continuous.
// (2) On answer 1 blue led blinks long pulses (9:1).
// (3) On answer 0 blue led blinks short pulses (1:9).
// (4) On all other payloads the blue led blinks quickly.
// (5) After 10 seconds the blue led is turned off
// Important: Example needs a broker and a relay, that sends
//   the answer message
// Created by Karl Hartinger, November 01, 2018.
// Modified
//  200301 Use Mqtt2 instead of Mqtt, use stmBlink
//  201219 Use SimpleMqtt instead of Mqtt2
//  210109 New TOPIC_SUB, TOPIC_PUB
// Released into the public domain.

#define D1MINI          1            // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"

#include "src/din/D1_class_Din.h"
#include "src/dout/D1_class_Dout.h"
#include "src/statemachine/D1_class_Statemachine.h"
#include "src/statemachine/D1_class_StatemachineBlink.h"

#define  DEBUG43        false //true  //false
#define  VERSION43      "2021-01-09 D1_oop43_smqtt_button3_led56"
#define  FUNCTION43     "button \"button/0\" sends \"relay/1/set/relay on|off|toggle\" to switch a relay by MQTT\n"
#define  TOPIC_BASE     "button/0"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom,all,blueled"
#define  TOPIC_SET      "topicbase,eeprom,blueled"
#define  TOPIC_SUB      "relay/1/ret/lamp"
#define  TOPIC_PUB      "relay/1/set/relay"
#define  STATE_NEVER    -9999

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define  PIN_BUTTON    17             // D3 -> 17
 #define  PIN_LED_BLUE   2             // D4 -> 2 (not 16!)
 #define  PIN_LED_GREEN 18             // D5 -> 18
 #define  PIN_LED_RED   19             // D6 -> 19
 //StatemachineBlink(pin,invert,stateStart,statesOn,statesOff,blinkNum)
 StatemachineBlink ledBlue(PIN_LED_BLUE,false,1,0,1,1);
#else
 #define  PIN_BUTTON    D3
 #define  PIN_LED_BLUE  D4
 #define  PIN_LED_GREEN D5
 #define  PIN_LED_RED   D6
 //StatemachineBlink(pin,invert,stateStart,statesOn,statesOff,blinkNum)
 StatemachineBlink ledBlue(PIN_LED_BLUE,true,1,0,1,1);
#endif
Din  button_(PIN_BUTTON);              // button at D3
Dout ledGreen(PIN_LED_GREEN);          // green led @ protoboard
Dout ledRed(PIN_LED_RED);              // red led   @ protoboard

//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1");

//_______state machine__________________________________________
#define STATE_MAX              600     // 600*100ms = 1min
#define STATE_DELAY            100     // state delay in ms
#define STATES_BLUELED         100     // 100x 100ms= 10sec
Statemachine stm(STATE_MAX, STATE_DELAY); //1..600

int iBlueLed=0;              //0=short,1=long,2=on,3=off,4=quick
int stateToTurnBlueLedOff=2; //state, when to turn off blue led

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
 if((sPayload=="?")||(sPayload=="help"))
  return String("+")+String(FUNCTION43);
 //-------------------------------------------------------------
 if(sPayload=="version") return String(VERSION43);
 //-------------------------------------------------------------
 if(sPayload=="topicbase") return client.getsTopicBase();
 //-------------------------------------------------------------
 if(sPayload=="eeprom") {
  int iResult;
  String s1=client.eepromReadTopicBase(iResult);
  if(iResult==-2) return String("Error: No topic base stored");
  if(iResult<0) return String("Error #")+String(iResult);
  return s1;
 }
 //-------------------------------------------------------------
 if(sPayload=="all") {
  p1 ="{\"Version\":\""+String(VERSION43)+"\",";
  p1+="\"IP\":\""+client.getsMyIP()+"\",";
  p1+="\"Topicbase\":\""+client.getsTopicBase()+"\",";
  int iResult;
  String s1=client.eepromReadTopicBase(iResult);
  if(iResult==-2) s1=String("(no topic base)");
  else {
   if(iResult<0) s1=String("Error #")+String(iResult);
  }
  p1+="\"EEPROM\":\""+s1;
  p1+="}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="blueled") return String(iBlueLed);
 //-------------------------------------------------------------
 return String("");                    // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 String p1;                            // help string
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {             // new topic base?
  client.changeTopicBase(sPayload);    // change base
  return client.getsTopicBase();       // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom") {                // erase eeprom?
  if(sPayload=="0" || sPayload=="erase")    // payload OK?
  {
   if(client.eepromEraseTopicBase()) return "erased";
   else return "not erased";           // return answer
  }
  else
  {
   return "(Supported payload: 0 or erase)";
  }
 }
 if(sTopic=="blueled") {    //0=short,1=long,2=on,3=off,4=quick
  if(sPayload=="?") return String("blink...0=short|1=long|2=on|3=off|4=quick");
  int iLed=sPayload.toInt();
  if(iLed>=0 && iLed<=4) {
   startBlueLed(iLed);
   return String("blueled set to ")+String(iLed);
  }
  return String("blueled NOT set");
 }
 //-------------------------------------------------------------
 return String("");                    // wrong set command
}

//_______execute sub requests___________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
 if(sTopic==TOPIC_SUB)                   // answer from relay
 { //....blue led: 0=short, 1=long, 2=on, 3=off, 4=quick blink..
  String sTop=client.getsTopicBase()+String("/ret/blueled");
  if(sPayload=="0") {
   startBlueLed(0); 
   client.publish((const char*)sTop.c_str(),"0");
   return; 
  }
  if(sPayload=="1") {
   startBlueLed(1);
   client.publish((const char*)sTop.c_str(),"1");
   return; 
  }
  startBlueLed(4);
  return;
 }
 //-------------------------------------------------------------
}

//______start the blue led blinking or turn it on or off________
// what_ : 0=short, 1=long, 2=on, 3=off, 4=quick blink
// uses  : statemachine stm, ledBlue
// sets  : iBlueLed, stateToTurnBlueLedOff
// return: what led is doing now (same as global iBlueLed)
int startBlueLed(int what_)
{
 if(what_>=0 && what_<5) iBlueLed=what_;
 else iBlueLed=4;
 int state=stm.getState();
 switch(iBlueLed)
 {
  case 0: //-------led blinks short pulses----------------------
   ledBlue.setParams(state,1,12,STATES_BLUELED/10); //short blinks
   if(DEBUG43) Serial.println("Blue LED blinks SHORT pulses");
   break; 
  case 1: //-------led blinks long pulses-----------------------
   ledBlue.setParams(state,12,1,STATES_BLUELED/10); //long blinks
   if(DEBUG43) Serial.println("Blue LED blinks LONG pulses");
   break;
  case 2: //-------turn blue led on-----------------------------
   ledBlue.on(stm); 
   if(DEBUG43) Serial.println("Blue LED is on");
   break;
  case 3: //-------turn blue led off----------------------------
   ledBlue.off(stm); 
   if(DEBUG43) Serial.println("Blue LED is off");
   break;
  case 4:
  default: //------led blinks quickly---------------------------
   ledBlue.setParams(state,1,1,STATES_BLUELED/2);
   if(DEBUG43) Serial.println("Blue LED blinks quickly");
   break;
 }
 stateToTurnBlueLedOff=stm.add(STATES_BLUELED);
 return iBlueLed;
}

//_______SETUP__________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 //if(DEBUG43)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------set actors---------------------------------------------
 ledGreen.off();                      // green off
 ledRed.on();                         // red on
 ledBlue.on(stm);                     // blue led on (for test)
 //------setup mqtt---------------------------------------------
 client.setLanguage('e');
 client.setCallback(callback);
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();
 //------connect to mqtt broker---------------------------------
 //if(true)
 if(client.connectWiFiMQTT())
 {
  ledGreen.on();                       // green on
  ledRed.off();                        // red off
  if(DEBUG43) Serial.println("setup(): Connected to MQTT broker!");
 }
 else
 {
  ledGreen.off();                      // green off
  ledRed.on();                         // red on
  if(DEBUG43) Serial.println("setup(): NOT connected to MQTT broker!");
 }
 //------setup finish-------------------------------------------
 if(DEBUG43) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG43) Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //======(1) make at the beginning of the loop ...==============
 int state=stm.loopBegin();            // state begin 
 String sSerial="";
 //======(2) do, independent on the network, ...================
 //------do every state-----------------------------------------
 if(DEBUG43) { sSerial=String(state); sSerial+=+": "; }
 if(state==stateToTurnBlueLedOff) {    // state to turn off led
  ledBlue.off(stm);                    // turn led off
  iBlueLed=0;                          // led is off
  stateToTurnBlueLedOff=STATE_NEVER;   // skip this state
 }
 //------do every state: check button---------------------------
 if(button_.isFallingEdge())
 {
  sSerial+="Button pressed, ";  
  client.sendPubIndex(0, "toggle");
  startBlueLed(2);
 }
 //======(3) do mqtt messages (send, receive)===================
 client.doLoop();                      // mqtt loop
 
 //======(4) do, depending on the network access, ...===========
 //------display WiFi/MQTT state on red/green-LED---------------
 if(client.isWiFiConnected())
 {
  if(client.isWiFiConnectedNew()) sSerial+=" ** New WiFi connection ** ";
  if(client.isMQTTConnected())
  {//....WiFi yes, MQTT yes -> LED green........................
   ledGreen.on();                      // green on
   ledRed.off();                       // red off
   if(client.isMQTTConnectedNew()) sSerial+=" ** New MQTT connection ** ";
  }
  else
  {//....WiFi yes, MQTT no -> LED red...........................
   ledGreen.off();                     // green off
   ledRed.on();                        // red on
   if(client.isMQTTDisconnectedNew()) sSerial+=" ** MQTT connection LOST ** ";
  }
 }
 else
 {//.....WiFi no -> LED red.....................................
   ledGreen.off();                     // green off
   ledRed.on();                        // red on
   if(client.isMQTTDisconnectedNew()) sSerial+=" ** MQTT connection LOST ** ";
   if(client.isWiFiDisconnectedNew()) sSerial+=" ** WiFi connection LOST ** ";
 }
 //======(5) do at the end of the loop ...======================
 ledBlue.doBlink(stm);                 // blue led action
 stm.loopEnd();                        // state end
 //------show only non empty states-----------------------------
 if(DEBUG43){ if(sSerial.length()>5) Serial.println(sSerial); }
}

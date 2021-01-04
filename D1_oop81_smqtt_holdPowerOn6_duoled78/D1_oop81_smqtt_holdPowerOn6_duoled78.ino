//___D1_oop81_smqtt_holdPowerOn6_duoled78.ino___210103-210103____
// After pressing a button, the D1 mini sends an MQTT message, 
// waits for the answer and displays it with the help of the 
// built-in blue LED.   
// Hardware: (1) WeMos D1 mini
//           (2) Protoboard "Powerhold"
// In detail the following happens:
// Immediately after startup, the D1 mini sets pin D6 to 3.3V to
// enable latching. Then it sets the Duo LED to red and connects
// to the network and the MQTT broker.   
// If this succeeds, the Duo LED lights up orange and the D1mini
// * publishes topic `info/start` with payload `button/1`
// * publishes topic `led/1/set/blueled` with payload `-1`
// * publishes topic `button/1/ret/voltage` with payload
//    0 to 100 [%].
// After that the D1 mini waits maximum 6 seconds for the 
// arrival of the topic `led/1/ret/blueled` with payload "off"
// (0) or "on" (1). The result is indicated by the blue LED:   
// * If there is no response, the blue LED flashes rapidly 
//   for three seconds (1:1).
// * If the answer is 0, the blue LED flashes twice with short 
//   pulses (1:12).
// * If the response is 1, the blue LED flashes twice with
//   long pulses (12:1).
// Then the D1 mini switches off the relay (D6 to 0) and goes
// into a deep sleep for one hour to be on the safe side 
//(in case the button hangs or is pressed too long on purpose).
//
// Important: Example needs a broker and a second D1mini, that
// sends the answer message (D1_oop75_smqtt_eeprom_blueled1)
// Created by Karl Hartinger, January 03, 2021
// Changes:
// 2021-01-03 New
// Released into the public domain.

#define D1MINI          1            // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/dout/D1_class_Dout.h"
#include "src/ain/D1_class_Ain.h"

#define  DEBUG81        false // true  //false
#define  VERSION81      "2021-01-03 D1_oop81_smqtt_holdPowerOn6_duoled78"
#define  FUNCTION81     "Switch (another) blue led by MQTT \"led/1/set/blueled 1|0|-1\""
#define  TOPIC_BASE     "button/1"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom,all,voltage"
#define  TOPIC_GET_R    "0,   0,      0, 0,        0,     0,  0,      1"
#define  TOPIC_SET      "topicbase,eeprom"
#define  TOPIC_SUB      "led/1/ret/blueled"
#define  TOPIC_PUB      "led/1/set/blueled"

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define  PIN_LED_BLUE   2             // D4 -> 2 (not 16!)
 #define  PIN_RELAY     19             // D6 -> 19
 #define  PIN_LED_GREEN 23             // D7 -> 23
 #define  PIN_LED_RED    5             // D8 -> 5
 Dout holdPower(PIN_RELAY, false, 1);  // relay @ protoboard
 Dout ledBlue(PIN_LED_BLUE, false);    // blue led
 Ain  ain(2, 36);                      // CPU-type=2, Pin 36
#else
 #define  PIN_LED_BLUE  D4
 #define  PIN_RELAY     D6
 #define  PIN_LED_GREEN D7
 #define  PIN_LED_RED   D8
 Dout holdPower(PIN_RELAY, false, 1);  // relay @ protoboard
 Dout ledBlue(PIN_LED_BLUE, true);     // blue led
 Ain  ain;                             // Default: D1 mini A0
#endif

Dout ledGreen(PIN_LED_GREEN);          // green led @ protoboard
Dout ledRed(PIN_LED_RED);              // red led   @ protoboard
int  zsec=60;                          // wait for answer 6.0 sec
int iLedAnswer=-999;                   // LED answer 0 or 1

//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client("Raspi11", "12345678","10.1.1.1");

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
 if(sPayload=="version") return String(VERSION81);
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
  p1 ="{\"Version\":\""+String(VERSION81)+"\",";
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
 if(sPayload=="voltage") return String(calcVoltagePercent(),0);
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
 if(sTopic=="led/1/ret/blueled") {
  if(DEBUG81) Serial.println("simpleSub(): Answer from "+sTopic+": "+sPayload);
  iLedAnswer=-1;
  if((sPayload=="1") || (sPayload=="on") ) iLedAnswer=1;
  if((sPayload=="0") || (sPayload=="off")) iLedAnswer=0;
 }
}

//_______measure voltage and calculate percent__________________
// assumption: max. measurable voltage is 5.4V
//             U >= 5V is 100%, U <= 3.3V is 0%
float calcVoltagePercent()
{
 float fAin=ain.getAinValue();
 float fAinMax=5.0*ain.getAinValueMax()/5.4;    // u2=max=5.0V
 float fAinMin=3.3*ain.getAinValueMax()/5.4;    // u1=min=3.3V
 if(fAin>fAinMax) fAin=fAinMax;
 if(fAin<fAinMin) fAin=fAinMin;
 float proz=100*(fAin-fAinMin)/(fAinMax-fAinMin);
 if(DEBUG81) Serial.println("voltage: "+String(proz,0)+"%");
 return proz;
}

//_______show end of action send MQTT command via LED___________
// uses iLedAnswer, client.doLoop()
// iLedAnswer=0: 2x 0.1s on, 1.2s off
// iLedAnswer=1: 2x 1.2s on, 0.1s off
// other blink values: 15x 0.1 on, 0.1 off
// after blink: switch off relay, goto deep sleep
void endBlink()
{
 //------set blink parameter------------------------------------
 int ion=1, ioff=1, jmax=15;           // quick blink
 int jmaxOnOff=3;                      // if answer ok (0 or 1)
 if(iLedAnswer==0) { ion=1,  ioff=12; jmax=jmaxOnOff; }
 if(iLedAnswer==1) { ion=12, ioff=1;  jmax=jmaxOnOff; }
 //------blinking blue led--------------------------------------
 for(int j=0; j<jmax; j++)
 {
  client.doLoop();                     // iLedAnswer changed?
  if(DEBUG81) Serial.println("endBlink(): iLedAnswer="+String(iLedAnswer));
  ledBlue.on();
  for(int i=0; i<ion; i++)
  {
   delay(100);
   if(iLedAnswer==0) { ion=1,  ioff=12; jmax=jmaxOnOff; }
   if(iLedAnswer==1) { ion=12, ioff=1;  jmax=jmaxOnOff; }
  }
  ledBlue.off();
  for(int i=0; i<ioff; i++)
  {
   delay(100);
   if(iLedAnswer==0) { ion=1,  ioff=12; jmax=jmaxOnOff; }
   if(iLedAnswer==1) { ion=12, ioff=1;  jmax=jmaxOnOff; }
  }
 }
 //------goto sleep (endless loop)------------------------------
 if(DEBUG81) Serial.println("Go to deep sleep.");
 delay(100);
 while(1)
 {
  holdPower.off();                     // turn power off
  delay(100);                          // wait a little bit
  ESP.deepSleep(60L*60e6);             // usec -> 1 Stunde
  delay(200);                          // just to be sure
 }
}

//_______SETUP__________________________________________________
void setup() {
 holdPower.on();                       // hold power relay
 //------Serial, just for debug---------------------------------
 //if(DEBUG81)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------set actors---------------------------------------------
 ledGreen.off();                      // green off
 ledRed.on();                         // red on
 ledBlue.off();                       // blue led off
 //------setup mqtt---------------------------------------------
 client.setLanguage('e');
 client.setCallback(callback);
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.setRetainedIndex("get",7,true);// voltage
 client.begin();
 //------connect to mqtt broker---------------------------------
 int i=2;
 while(!client.connectWiFiMQTT())      // max. 8sec
 {
  if(DEBUG81) Serial.println("setup(): No WiFi/MQTT connection");
  delay(100);                          // wait 100ms (0.1s)
  i--;                                 // counter
  if(i<1) endBlink();                  // time out
 }
 //------connected!---------------------------------------------
 ledGreen.on();                        // LED is orange
 client.sendPubIndex(0, "-1");         // toggle /led/1
 client.forceXXXAnswer("get","voltage",""); // send voltage ret
 //------setup finish-------------------------------------------
 if(DEBUG81) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG81) Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 //------check MQTT for incomming message-----------------------
 client.doLoop();                      // mqtt loop
 //------answer received: show answer via blue led--------------
 if(iLedAnswer!=-999) {                // answer received?
  ledRed.off();                        // led is ...
  ledGreen.on();                       // ... green
  endBlink();                          // show result
 }
 //-----timeout?------------------------------------------------
 delay(100);                           // wait 100ms (0.1s)
 zsec--;                               // dec 1/10s counter
 if(zsec<1) {                          // timeout
  ledRed.on();                         // led is ...
  ledGreen.off();                      // ... red
  endBlink();                          // blink error
 }
}
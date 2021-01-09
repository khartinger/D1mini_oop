//_____D1_oop82_smqtt_door5_holdPowerOn6duoled78.ino_210109_____
// With the help of the do-it-yourself shield "D1_holdPowerOn",
// a battery-powered D1 mini connects to the supply voltage
// after a start pulse and sends the state of pin D5 to a broker
// via MQTT. It then checks whether the message has been 
// forwarded by the broker and displays the result via a Duo LED.
// In detail the following happens:
// 1. after the program start the D1 mini sets pin D6 to "1" 
//    (3V3) to ensure the self-holding.
// 2. the D1 mini sets the Duo LED to red and connects to the
//    network and the MQTT broker.
// 3. if this succeeds, the Duo LED lights up orange and D1 mini
//    * publishes topic `info/start` with payload `door/1`.
//    * publishes topic `door/1/ret/status` with payload
//      `0` or `1` depending on the state of pin D5.
//    * publishes topic `door/1/ret/voltage` with 
//      payload 0 to 100 [%].
// 4. D1 mini indicates the state of D5 by flashing the LED:
//    * D5=1 LED flashes long pulses (9:1).
//    * D5=0 LED flashes short pulses (1:9)
// 5. if the D1 mini receives topic 'door/1/ret/status'
//    sent by itself, the LED flashes at a higher rate
// 6. if status of D5 changes, another status message is sent.
// 7. after 12 seconds D1 mini switches off the relay (D6 to 0).
// If this fails, the D1 mini will go into deep sleep for one
// hour and then restart.
// (Alternatively: If this does not succeed, the red LED flashes
// briefly every 10 seconds. Furthermore, a message with the
// battery status is sent every 8 hours).
//
// Hardware: 
// (1) WeMos D1 mini
// (2) selfmade prototype shield "holdPowerOn"
//
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, January 09, 2021
// Changes:
// 2021-01-09 New
// Released into the public domain.

#define D1MINI          1            // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/dout/D1_class_Dout.h"
#include "src/din/D1_class_Din.h"
#include "src/ain/D1_class_Ain.h"

#define  DEBUG82        true  //false
#define  VERSION82      "2021-01-09 D1_oop82_smqtt_door5_holdPowerOn6_duoled78"
#define  TOPIC_BASE     "door/1"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom,all,status,voltage"
//#define  TOPIC_GET_R    "0,0,0,0,0,0,1,1"
#define  TOPIC_SET      "topicbase,eeprom"
#define  TOPIC_SUB0     "door/1/ret/status"
#define  TOPIC_SUB      TOPIC_SUB0
#define  TOPIC_PUB      ""
#define  NOTHING_REC    -999
#define  LED_RED        1
#define  LED_GREEN      2
#define  LED_ORANGE     3

//_______sensors, actors, global variables______________________
#if defined(ESP32) || defined(ESP32D1)
 #define  PIN_WAKEUP    26               // D0 -> 26
 #define  WAKEUP_PULLUP  5               // 
 //#define  PIN_LED_BLUE   2             // D4 -> 2 (not 16!)
 #define  PIN_STATE     18             // D5 -> 18
 #define  PIN_RELAY     19             // D6 -> 19
 #define  PIN_LED_GREEN 23             // D7 -> 23
 #define  PIN_LED_RED    5             // D8 -> 5
 
 Dout holdPower(PIN_RELAY, false, 1);  // relay @ protoboard
 Din  pinState(PIN_STATE);             // door open/close
 Ain  ain(2, 36);                      // CPU-type=2, Pin 36
#else
 #define  PIN_WAKEUP    D0
 //#define  PIN_LED_BLUE  D4
 #define  PIN_STATE     D5
 #define  PIN_RELAY     D6
 #define  PIN_LED_GREEN D7
 #define  PIN_LED_RED   D8
 Dout holdPower(PIN_RELAY, false, 1);  // relay @ protoboard
 Din  pinState(PIN_STATE);             // door open/close
 Ain  ain;                             // Default: D1 mini A0
#endif

Dout ledGreen(PIN_LED_GREEN);          // green led @ protoboard
Dout ledRed(PIN_LED_RED);              // red led   @ protoboard
int  pinValueSent=-1;                  // value sent to broker
int  pinValueRec=NOTHING_REC;          // value received from broker
int  pinValueSum=0;                    // sum pin values
int  zsec=120;                         // wait for answer 12.0 sec
String sAddToGet="";                   // add to get-Answer

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
 if(sPayload=="version") return String(VERSION82);
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
  p1 ="{\"Version\":\""+String(VERSION82)+"\",";
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
 if(sPayload=="status") return String(pinValueSent);
 //-------------------------------------------------------------
 if(sPayload=="voltage") return String(calcVoltagePercent(),0)+sAddToGet;
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
 if(sTopic==TOPIC_SUB0) {
  if(DEBUG82) Serial.println("simpleSub(): Answer from "+sTopic+": "+sPayload);
  pinValueRec=-1;
  if((sPayload=="1") || (sPayload=="on") ) pinValueRec=1;
  if((sPayload=="0") || (sPayload=="off")) pinValueRec=0;
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
 if(DEBUG82) Serial.println("voltage: "+String(proz,0)+"%");
 return proz;
}

//_______end: switch off relay__________________________________
// on error: led blinks short red pulses
void end_()
{
 if(DEBUG82) Serial.println("End.");
 delay(100);
 digitalWrite(PIN_WAKEUP, HIGH);
 while(1)
 {
  ledRed.off();                        // turn led off
  ledGreen.off();                      // turn led off
  holdPower.off();                     // turn power off
  delay(2000);                         // wait 2sec
  if(DEBUG82) Serial.println("VOR ESP.deepSleep.");
  //ESP.deepSleep(30000000L);            // 30 sec just for test
  ESP.deepSleep(3600000000L);          // 1 hour
  delay(1000);
 }
}

/*
//_______end: switch off relay__________________________________
// on error: led blinks short red pulses
void end_()
{
 unsigned long counter10s=0;
 if(DEBUG82) Serial.println("End.");
 delay(100);
 sAddToGet=" - error";
 while(1)
 {
  ledRed.off();                        // turn led off
  ledGreen.off();                      // turn led off
  holdPower.off();                     // turn power off
  delay(2000);                         // wait 2sec
  client.doLoop();                     // mqtt loop
  delay(2000);                         // wait 2sec
  client.doLoop();                     // mqtt loop
  //.....this code should never be reached ;)...................
  ledRed.on(); delay(100);             // short red...
  ledRed.off(); delay(1900);           // ...led blink
  client.doLoop();                     // mqtt loop
  delay(2000);                         // wait 2sec
  client.doLoop();                     // mqtt loop
  delay(2000);                         // wait 2sec
  counter10s++;                        // increment counter
  if(counter10s % 2880==0)             // 6*60*8 = 8 hours
  {
   client.forceXXXAnswer("get","voltage",""); //send voltage ret
  }
 }
}
*/

//_____set led__________________________________________________
// uses zsec
void setLed(int pinValueSent, int ledColor)
{
 static int  nextZsecChange=zsec;
 static bool ledIsOn=false;
 static int  ledColorOld=0;
 if(ledColor!=ledColorOld)
 {
  ledColorOld=ledColor;
  if(ledIsOn)
  {
   if((ledColor&LED_RED)>0) ledRed.on(); else ledRed.off();
   if((ledColor&LED_GREEN)>0) ledGreen.on(); else ledGreen.off();
  }
 }
 if(zsec<=nextZsecChange)
 {//-----change led on|off or color-----------------------------
  if(ledIsOn)
  {//....turn led off...........................................
   ledRed.off();
   ledGreen.off();
   //....calculate duration of off state........................
   if(pinValueSent==0) nextZsecChange=zsec-9;    // long
                  else nextZsecChange=zsec-1;    // short
  }
  else
  {//....turn led on............................................
   if((ledColor&LED_RED)>0) ledRed.on(); else ledRed.off();
   if((ledColor&LED_GREEN)>0) ledGreen.on(); else ledGreen.off();
   //....calculate duration of on state.........................
   if(pinValueSent==1) nextZsecChange=zsec-9;    // long
                  else nextZsecChange=zsec-1;    // short
  }
  ledIsOn=!ledIsOn; 
 }
}

//_______SETUP__________________________________________________
void setup() {
 holdPower.on();                       // hold power relay
 //------Reset info---------------------------------------------
 int iStartReason=0;
 #if defined(ESP8266) || defined(D1MINI)
  rst_info *pResetInfo;
  pResetInfo = ESP.getResetInfoPtr();  // pointer to resetinfo struct
  iStartReason=pResetInfo->reason;     // get reason number (uint32)
  if(iStartReason==5) sAddToGet=" - wakeup!";
  else sAddToGet="";
 #endif
 //------Serial, just for debug---------------------------------
 //if(DEBUG82)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 Serial.println("iStartReason="+String(iStartReason));
 //------set actors---------------------------------------------
 ledGreen.off();                      // green off
 ledRed.on();                         // red on
 pinMode(PIN_WAKEUP, WAKEUP_PULLUP);  // connect D0->RST
 
 //------setup mqtt---------------------------------------------
 client.setLanguage('e');
 client.setCallback(callback);
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.setRetainedIndex("get",7,true);// status
 client.setRetainedIndex("get",8,true);// voltage
 client.begin();
 //------connect to mqtt broker---------------------------------
 int i=2;
 while(!client.connectWiFiMQTT())      // max. 8sec
 {
  if(DEBUG82) Serial.println("setup(): No WiFi/MQTT connection");
  delay(100);                          // wait 100ms (0.1s)
  i--;                                 // counter
  if(i<1) end_();                      // time out
 }
 //------connected!---------------------------------------------
 pinValueSent=pinState.read();         // pin state...
 setLed(pinValueSent, LED_ORANGE);     // set leds
 client.forceXXXAnswer("get","status","");    //send status ret
 client.forceXXXAnswer("get","voltage","");   //send voltage ret
 //------setup finish-------------------------------------------
 if(DEBUG82) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG82) Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 //------check, if D5 has changed-------------------------------
 int pinValueTemp=pinState.read();
 if(pinValueSent!=pinValueTemp)
 {
  pinValueSent=pinValueTemp;                   // new value...
  client.forceXXXAnswer("get","status","");    //send status ret
 }
 //------check MQTT for incomming message-----------------------
 client.doLoop();                      // mqtt loop
 //------calculate led color------------------------------------
 int ledColor=LED_RED;
 if(client.isMQTTConnected()) {
  if(pinValueSent==pinValueRec) ledColor=LED_GREEN;
  else ledColor=LED_ORANGE;
 }
 //------set Led------------------------------------------------
 setLed(pinValueSent, ledColor);
 //-----timeout?------------------------------------------------
 delay(100);                           // wait 100ms (0.1s)
 zsec--;                               // dec 1/10s counter
 if(zsec<1) end_();                    // timeout
}
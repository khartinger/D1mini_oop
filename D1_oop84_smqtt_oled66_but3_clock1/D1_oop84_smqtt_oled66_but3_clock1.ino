//_____D1_oop84_smqtt_oled66_but3_clock1.ino__210130_210130_____
// This programme for a D1 mini (or ESP32 D1mini) counts seconds
// and displays the value as date and time on a 0.66" LED.
// * The programme connects to the specified MQTT broker via
//   the WLAN.
// * If the D3 key is pressed, the D1mini sends an MQTT 
//   message (Topic `getTime`, Payload `?`) to the broker.
// * If a message with the topic `brokertime` is received,
//   the payload (format `YYYYmmdd HHMMSS`) is used to update
//   the date and time.
// * If no connection with the network or broker can be 
//   established, the time is only updated approximately 
//   every 8 seconds (= default waiting time in the class
//   `SimpleMqtt` for the network connection).
//
// In addition, the following functions are available:
// * Automatic (built-in) answering of requests   
//   `-t oop84/get -m help`   
//   `-t oop84/get -m version`   
//   `-t oop84/get -m ip`   
//   The response is a MQTT message of the form 
//   `oop84/ret/...`.   
// * Reply to the following requests   
//   `-t oop84/get -m topicbase`   
//   `-t oop84/get -m eeprom`   
//   The response will also be a MQTT message of the form
//   `oop84/ret/...`.   
// * Set a new topic base, which is also stored in the EEPROM:
//   `-t oop84/set/topicbase -m new_base`
// * Delete the topic base stored in the EEPROM. 
//   When restarting, the default topicbase will be used.   
//   `-t oop84/set/eeprom -m erase`   
//
// * Important: The example requires an MQTT broker!!!
//   So don't forget: Adapt the WLAN data to your own network in line:
//   `SimpleMqtt client("..ssid..", "..password..", "mqtt server name");`
// * Class SimpleMqtt extends class PubSubClient for easy use.
//   All methods of the class `PubSubClient` can still be used.
//   If PubSubClient library is installed, the two PubSubClient
//   files in the src/simplemqtt directory can be deleted.
//
// Hardware:
// (1) WeMos D1 mini (OR ESP32 D1 mini)
// (2) (Self-made) Button shield D3 (or Button at D3)
// (3) OLED Shield: 0.66", SSD1306, 64x48 pixel, I2C
//
// Created by Karl Hartinger, January 30, 2021.
// Changes:
// 2021-01-30 First release
// Released into the public domain.

#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini
#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/screen1a/D1_class_Screen1a.h"
#include "src/din/D1_class_Din.h"
#include "src/timeconverter1/D1_class_Timeconverter1.h"

#define  DEBUG84        true  //false
#define  VERSION84      "2021-01-30 D1_oop84_smqtt_oled66_but3_clock1"
#define  TOPIC_BASE     "oop84"
#define  TOPIC_GET      "?,help,version,ip,topicbase,eeprom"
#define  TOPIC_SET      "topicbase,eeprom"
#define  TOPIC_SUB      "brokertime"
#define  TOPIC_PUB      "getTime"

//_______pin definitions, depending on uC board_________________
#if defined(ESP32D1)
 const int pinButton=17;               // D3=17
#else
 const int pinButton=D3;               // D3
#endif

//_______sensors, actors, global variables______________________
Screen1 display_;                      // OLED 0.66"
Din     button3(pinButton);            // Pushbutton
String  sBrokertime;                   // from broker
bool    bBrokertimeNew=false;          // is new time?
Timeconverter1  myTC;                  // time + format
time_t  sec1970update=0;               // last time of OLED update
time_t  sec1970D1=0;                   // broker time - D1 time

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
 //-------------------------------------------------------------
 if(sPayload=="version") return String(VERSION84);
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
 return String("");                    // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {             // new topic base?
  client.changeTopicBase(sPayload);    // change base
  String s1=client.getsTopicBase();    // read new base
  return s1;                           // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom") {                // erase eeprom?
  if(sPayload=="0" || sPayload=="erase") {  // payload OK?
   if(client.eepromEraseTopicBase()) return "erased";
  }
  else return "not erased";            // return answer
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
 if(sTopic==TOPIC_SUB) {               // counter in
  sBrokertime=sPayload;
  bBrokertimeNew=true;
  if(DEBUG84) Serial.println("simpleSub(): "+sBrokertime);
 }
}

//_______setup Serial, WLAN and MQTT clients____________________
void setup() 
{
 //------Serial, just for debug---------------------------------
 if(DEBUG84)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------init 0.96" OLED display--------------------------------
 display_.begin();
 display_.screen6iClear(1,"Datum&Zeit");
 display_.screen6i(4,"brokertime");
 display_.screen6(5,"-No WiFi--");
 display_.screen6(6,"-No MQTT--");
 //------init WiFi/MQTT client----------------------------------
 client.setCallback(callback);
 client.setTopicBaseDefault(String(TOPIC_BASE));
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin();                       //init client objects
 //------setup finish-------------------------------------------
 if(DEBUG84) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 if(DEBUG84) Serial.println("setup(): --Finished--\n");
}

//_______LOOP___________________________________________________
void loop() {
 //======(1) do, independent on the network, ...================
 //------calculate unix-time, base: last broker time answer-----
 time_t sec1970=sec1970D1+(millis()/1000);
 //------time to update OLED?-----------------------------------
 if(sec1970>sec1970update)
 {//.....update OLED, because unix-second-value has changed.....
  sec1970update=sec1970;               // remember update time
  myTC.set(sec1970);                   // use converter to...
  display_.screen6(2,myTC.getTmJ_());  // ...convert seconds...
  display_.screen6(3,myTC.getHMS_());  // ...to date and time
 }
 //------new broker time received?------------------------------
 if(bBrokertimeNew)
 {//.....a new broker time has been received....................
  bBrokertimeNew=false;                // change has been edited
  myTC.set(sBrokertime);               // convert broker time...
  sec1970D1=myTC.getSec1970()-(millis()/1000);   //...to secs D1
  //.....display new brokertime.................................
  display_.screen6(5,sBrokertime.substring(0,8));     // date
  display_.screen6(6,sBrokertime.substring(9));       // time
  //.....display (calculated) new brokertime on OLED............
  display_.screen6(2,myTC.getTmJ_());  // should be the same...
  display_.screen6(3,myTC.getHMS_());  // ...values as above ;)
 }
 //------send message, if button is pressed---------------------
 if(button3.isFallingEdge()) client.sendPubIndex(0,"?");

 //======(2) do mqtt messages (send, receive)===================
 client.doLoop();                      // doLoop MUST be called
 
 //======(3) do, depending on the network access, ...===========
 if(client.isWiFiConnectedNew()) display_.screen6(5," WiFi OK  ");
 if(client.isMQTTConnectedNew()) display_.screen6(6," MQTT OK  ");
 if(client.isWiFiDisconnectedNew()) display_.screen6(5,"-No WiFi--");
 if(client.isMQTTDisconnectedNew()) display_.screen6(6,"-No MQTT--");

 //======(4) do at the end of the loop ...======================
 delay(100);
}

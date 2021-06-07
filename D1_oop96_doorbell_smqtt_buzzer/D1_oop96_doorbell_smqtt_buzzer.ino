//_____P1_oop96_doorbell_smqtt_buzzer.ino________khartinger_____
// 1.  D1 mini works as a MQTT client and waits for a message
//     t doorbell/1/ret/ring -m 1
//     When the message arrives, the buzzer plays a melody.
// 2.  Function of this program:
// 2.1 Connect to given WiFi and MQTT broker
// 2.2 Use "topic base" buzzer/1
// 2.3 Wait for topic doorbell/1/ret/ring with payload 1.
//     When the message is received, a melody is played.
// 2.4 Automatic (build in) answers for messages 
//     -t buzzer/1/get -m help
//     -t buzzer/1/get -m version
//     -t buzzer/1/get -m ip
// 2.5 Possibility to change the base topic:
//     -t buzzer/1/set/topicbase -m new_topic_base
//
// Class SimpleMqtt extends class PubSubClient for easy use.
// All commands of the PubSubClient class can still be used.
//
// Hardware: 
// 1. WeMos D1 mini (OR ESP32 D1 mini)
// 2. Buzzer-Shield
// Note: When PubSubClient lib is installed,
//       delete PubSubClient files in directory src/simplemqtt
// Important: Example needs a MQTT-broker!
// Created by Karl Hartinger, June 07, 2021.
// Changes:
// 2021-06-07 New
// Released into the public domain.
#define D1MINI          1              // ESP8266 D1mini +pro
//#define ESP32D1         2              // ESP32 D1mini

#include "src/simplemqtt/D1_class_SimpleMqtt.h"
#include "src/buzzer1/D1_class_Buzzer1.h"

#define  DEBUG96        true //false
#define  VERSION96      "2021-06-07 D1_oop96_doorbell_smqtt_buzzer"
#define  LANGUAGE96     'e'            // 'd' or 'e'
#define  TOPIC_BASE     "buzzer/1"
#define  TOPIC_GET      "help,version,ip,topicbase,eeprom"
#define  TOPIC_SET      "topicbase,eeprom0"
#define  TOPIC_SUB      "doorbell/1/ret/ring"
#define  TOPIC_PUB      ""

//_______sensors, actors, global variables______________________
//.......buzzer object and interrupt service routine............
Buzzer1 buzzer;                             // buzzer object
ICACHE_RAM_ATTR void buzzer1_ISR(void *pArg) { buzzer.timer_ISR(NULL); }
String melody1="$T120L8MS CDEF GAG4 FDB4 GE>C4 <CDEF GAG4 FDB>D C4P4"
               "<AF>C<MLB16MSA16 GEG4 FDB4 GE>C4 <AF>C<MLB16MSA16"
               "GEG4 FDB>D C4P4";
//String melody1="$T120L8MN CDEF GAG4 FDB4 GE>C4 <CDEF GAG4 FDB>D C4P4"
//               "<AF>C<MLB16MNA16 GEG4 FDB4 GE>C4 <AF>C<MLB16MNA16"
//               "GEG4 FDB>D C4P4";

//_______MQTT communication_____________________________________
//SimpleMqtt client("..ssid..", "..password..","mqttservername");
SimpleMqtt client(String("Raspi11"), String("12345678"),
                  String("10.1.1.1"), String(TOPIC_BASE));

//_______Global values__________________________________________
bool startMelody=false;                     // start buzzer

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
 String p1=String("");                      // help string
 sPayload.toLowerCase();                    // for easy compare
 //-------------------------------------------------------------
  if(sPayload=="version") {
  p1="{\"version\":\""; p1+= String(VERSION96); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="ip") {
  p1="{\"ip\":\""; p1+= client.getsLocalIP(); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="topicbase") {
  p1="{\"topicbase\":\""; p1+=client.getsTopicBase(); p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 if(sPayload=="eeprom") {
  int iResult1, iResult2;
  String s1=client.eepromReadTopicBase(iResult1);
  String s2=client.eepromReadMyData(iResult2);
  p1="{\"topicbase\":\"";
  if(iResult1>=0) p1+=s1;
  else
  {
   if(iResult1==-2) p1+="(no topic base)";
   else p1+=String("Error_#")+String(iResult1);
  }
  p1+="\",\"mydata\":\"";
  if(s2=="") p1+="(no data)";
  else p1+=s2;
  p1+="\"}";
  return p1;
 }
 //-------------------------------------------------------------
 return String("");                         // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 String p1=String("");                      // help string
 sPayload.toLowerCase();                    // for easy compare
 //-------------------------------------------------------------
 if(sTopic=="topicbase") {                  // new topic base?
  client.changeTopicBase(sPayload);         // change base
  p1="{\"topicbase\":\"";                   // start json
  p1+=client.getsTopicBase();               // read new base
  p1+="\"}";                                // end json
  return p1;                                // return new base
 }
 //-------------------------------------------------------------
 if(sTopic=="eeprom0") {                    // erase eeprom?
  if(sPayload=="?") {
   p1="{\"erase\":\"";                 // start json
   p1+="1=topicBase|2=myData|3=alles\"}";// end json
   return p1;                          // return erase info
  }
  p1="";
  if(sPayload=="2" || sPayload=="3" || sPayload=="alles")
  {
   p1+="my data ";
   if(!client.eepromEraseMyData()) p1+="NICHT ";
   p1+="geloescht";                    // return answer 1
  }
  if(sPayload=="1" || sPayload=="3" || sPayload=="alles")
  {
   if(p1!="") p1+=", ";
   p1+="topicbase ";
   if(!client.eepromEraseTopicBase()) p1+="NICHT ";
   p1+="geloescht";                         // return answer 2
  }
  p1="{\"eeprom\":\""+p1+"\"}";             // make json
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
 //-------------------------------------------------------------
 if(sTopic==TOPIC_SUB)                      // doorbell?
 {
  if(sPayload=="1") startMelody=true;       // start melody
 }
}

//_____SETUP____________________________________________________
void setup() {
 //------Serial, just for debug---------------------------------
 //if(DEBUG96)
 {Serial.begin(115200); Serial.println("\nsetup(): --Start--");}
 //------setup buzzer-------------------------------------------
 buzzer.setISR(buzzer1_ISR);                // set name of ISR
 buzzer.setMelody(melody1);                 // set new melody
 //------setup mqtt---------------------------------------------
 client.setLanguage(LANGUAGE96);            //e=english,d=german
 client.setCallback(callback);              // mqtt receiver
 client.setTopicBaseDefault(TOPIC_BASE);    // topic base
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 //client.setRetainedIndex("get",3,true);
 client.begin();                            // setup objects
 //------connect to WiFi and mqtt broker------------------------
 int i=3;
 while((--i)>=0) { 
  if(client.connectWiFiMQTT()) i=-1;
  delay(500);
 }
 //------setup finish-------------------------------------------
 String sTemp="{\"topicbase\":\"";
 sTemp+=client.getsTopicBase()+"\",\"IP\":\""+client.getsLocalIP()+"\"}";
 client.publish_P("info/start/setup",sTemp.c_str(),false);
 //client.bAllowMQTTStartInfo(false);          //NO mqtt (re)start info
 if(DEBUG96) Serial.println("setup(): MQTT topicBase="+client.getsTopicBase());
 Serial.println("setup(): --Finished--\n");
}

//_____LOOP_____________________________________________________
void loop() {
 //-----process mqtt actions------------------------------------
 client.doLoop();                           // mqtt loop
 if(startMelody)
 {
  buzzer.start();
  startMelody=false;
 }
 delay(100);
}

//_____D1_class_MqttClientKH.h________________170721-171029_____
// The class MqttClient extends the class PubSubClient,
//  so you can use all commands from this class as well.
// When PubSubClient lib is installed, delete directory /libs !
//
// Hardware: D1 mini
//           Button Shield D3
#ifndef D1_CLASS_MQTTCLIENTKH_H
#define D1_CLASS_MQTTCLIENTKH_H
#include <ESP8266WiFi.h>
#include "libs/PubSubClient.h"         // use with /libs

#define SSID_SIZE     20               // max.len ssid
#define PASS_SIZE     20               // max.len password
#define MQTT_SIZE     20               // max.len mqttservername
#define TOPIC_MAX      8               // max. topics to sub
#define MQTT_RECONNECT_MS         4000
#define TIMEOUT_WIFI_CONNECT_MS   8000 // wait for WLAN
#define MESSAGE_MAXLEN             127
#ifndef DEBUG1
 #define DEBUG1                   true // true=Serial output
#endif

class MqttClientKH : public PubSubClient {
 //-----properties----------------------------------------------
 protected:
  char ssid_[SSID_SIZE+1];             //
  char pass_[PASS_SIZE+1];             // 
  char mqtt_[MQTT_SIZE+1];             // 
  int  port_;                          // mqtt port (def 1883)
  String aTopicSub_[TOPIC_MAX];        // subscribed topics
  String aTopicPub_[TOPIC_MAX];        // topics to publish
  String aPayloadPub_[TOPIC_MAX];      // value on (re)start
  bool   aRetainPub_[TOPIC_MAX];       // retain true|false
  int    numSub_;                      // number subscribed topics
  int    numPub_;                      // number topics to publish
  WiFiClient d1miniClient;             // WLAN client for MQTT
  String sClientName;                  // MQTT client name
  long millis_lastConnected;           // last connection time [ms]
 public:
  MqttClientKH(char* ssid, char* pwd, char* mqtt_server, int port);
  int  getNumSub() { return numSub_; };
  int  getNumPub() { return numPub_; };
  void clrSubscribe() { numSub_=0; };
  void clrPublish() { numPub_=0; };
  void setClientName(String sName) {sClientName=sName;};
  String getClientName() { return sClientName; };
  //-----methods to setup WLAN and mqtt connection--------------
  bool setup_wifi();
  bool reconnect();
  bool isConnected();
  bool sendPubSubTopics();
  void printPubSubTopics2Serial(String clientId);
  //-----methods to define mqtt topics--------------------------
  bool addSubscribe(String topic);
  bool delSubscribe(String topic);
  bool addPublish(String topic, String payload, bool retain);
  bool delPublish(String topic);
  void publishString(String topic, String payload);
  void publishString(String topic, String payload, bool retain);
  //-----seldom used-------------------------------------------
  int  setSubscribe(String aTopicSub[], int num);
  int  setPublish(String aTopicPub[], String aPayload[], int num);
  void subscribeString(String topic);
};

//_____constructor______________________________________________
MqttClientKH::MqttClientKH(char* ssid, char* pwd, 
  char* mqtt_server="localhost", int port=1883):PubSubClient(d1miniClient)
{
 strcpy(ssid_, ssid);
 strcpy(pass_, pwd);
 strcpy(mqtt_, mqtt_server);
 port_=port;
 millis_lastConnected = 0;
 numSub_=0;
 numPub_=0;
 setup_wifi();
 setServer(mqtt_, port_);
}

//**************************************************************
// methods to setup WLAN and mqtt connection
//**************************************************************

//_____connect to the WiFi network______________________________
bool MqttClientKH::setup_wifi()
{
 if(WiFi.status()==WL_CONNECTED) return true;
 delay(10);
 if(DEBUG1) Serial.println("\nConnecting to "+String(ssid_));
 WiFi.begin(ssid_, pass_);
 //-----try to connect to WLAN (access point)-------------------
 int i=TIMEOUT_WIFI_CONNECT_MS/200;
 while((WiFi.status()!=WL_CONNECTED) && (i>0))
 {
  delay(200);
  i--;
  if(DEBUG1){Serial.print("."); if(i%50==0) Serial.println("");}
 }
 //-----connected to WLAN (access point)?-----------------------
 if(i<1)
 { //-----not connected to WLAN---------------------------------
  if(DEBUG1) Serial.println("No connection - time-out!");
  return false;
 }
 //-----success WiFi new connection/reconnect-------------------
 if(DEBUG1)Serial.println("\nConnected! IP address is "+WiFi.localIP().toString());
 return true;
}

//_____check for connect, if not: try to reconnect______________
bool MqttClientKH::reconnect()
{
 //-----when connected, return----------------------------------
 if(connected()) { return true; }
 //-----WiFi connected?-----------------------------------------
 if(!setup_wifi()) return false;
 //-----WiFi yes, mqtt no---------------------------------------
 if(DEBUG1)Serial.println("MQTT: Not connected - reconnect...");
 //-----MQTT: try to send all PubSub topics---------------------
 if(!sendPubSubTopics())
 {
  if(DEBUG1)Serial.println("failed, client state rc="+String(state()));
  return false;
 }
 return true;
}

//_____is mqtt connection ok? (no: reconnect)___________________
bool MqttClientKH::isConnected()
{
 long now = millis();
 //-----check for mqtt connection-------------------------------
 if (!connected())
 {
  if (now - millis_lastConnected > MQTT_RECONNECT_MS) 
  {
   millis_lastConnected=now;
   if(reconnect()) millis_lastConnected=0;
  }
 }
 //-----if connected to broker, do loop function----------------
 if (connected())
 {
  loop();
  return true;
 }
 return false;
}

//-----MQTT: send all PubSub topics (e.g. on reconnect)---------
bool MqttClientKH::sendPubSubTopics()
{
 //-----If no wifi ClientId: create a random client ID----------
 String clientId=sClientName;
 if(clientId=="")
 {
  randomSeed(micros());           // start random numbers
  clientId = "D1mini_Client_";
  clientId += String(random(0xffff), HEX);
 }
 //-----Try to connect------------------------------------------
 if(connect(clientId.c_str()))
 {
  //-----Once connected, publish an announcement----------------
  for(int i=0; i<numPub_; i++)
  {
   publishString(aTopicPub_[i], aPayloadPub_[i], aRetainPub_[i]);
  }
  //.....and resubscribe.......................................
  for(int i=0; i<numSub_; i++)
  {
   subscribeString(aTopicSub_[i]);
  }
  if(DEBUG1) printPubSubTopics2Serial(clientId);
  return true;
 }
 return false;
}

//-----send all PubSub topics to Serial-------------------------
// just for test purpose
void MqttClientKH::printPubSubTopics2Serial(String clientId="")
{
 if(clientId=="") clientId=sClientName;
 Serial.println("====="+clientId+" connected======");
 Serial.println("-----publish topic list ("+String(numPub_)+")-------");
 for(int i=0; i<numPub_; i++)
 {
  Serial.print(aTopicPub_[i]+"="+aPayloadPub_[i]+", retain=");
  if(aRetainPub_[i]) Serial.println("true");
  else Serial.println("false");
 }
 Serial.println("-----subscribe topic list ("+String(numSub_)+")-----");
 for(int i=0; i<numSub_; i++)
 {
  Serial.println(aTopicSub_[i]);
 }
 Serial.println("==============================");
}

//**************************************************************
// methods to define mqtt topics
//**************************************************************

//_____add a (String) topic to subscribe array__________________
bool MqttClientKH::addSubscribe(String topic)
{
 //-----is topic already in subscribe array?--------------------
 for(int i=0; i<numSub_; i++)
 {
  if(topic.equals(aTopicSub_[i])) return true;
 }
 //-----add topic (if enough space)-----------------------------
 if(numSub_<TOPIC_MAX)
 {
  aTopicSub_[numSub_++]=topic;
  return true;
 }
 return false;
}

//_____convert String to array and unsubscribe__________________
// return: true=unscribed, false=not
bool MqttClientKH::delSubscribe(String topic)
{
 //-----is topic in subscribe array?----------------------------
 int i=0;
 for(i=0; i<numSub_; i++)
 {
  if(topic.equals(aTopicSub_[i])) break;
 }
 if(i>=numSub_) return false;
 //i--;
 //-----topic in array at index i, delete from array------------
 numSub_--;
 for(int j=i; j<numSub_; j++)
  aTopicSub_[j]=aTopicSub_[j+1];
 //-----subscribe from broker-----------------------------------
 char cTopic[1+MESSAGE_MAXLEN];       // helper array
 topic.toCharArray(cTopic,MESSAGE_MAXLEN);
 unsubscribe(cTopic);
}

//_____add a (String) topic to publish array____________________
// if topic exists --> replace payload
bool MqttClientKH::addPublish(String topic, String payload="", 
     bool retain=true)
{
 //-----is topic already in subscribe array?--------------------
 for(int i=0; i<numPub_; i++)
 {
  if(topic.equals(aTopicPub_[i]))
  {
   aPayloadPub_[i]=payload;
   aRetainPub_[i]=retain;
   return true;
  }
 }
 //-----add topic (if enough space)-----------------------------
 if(numPub_<TOPIC_MAX)
 {
  aTopicPub_[numPub_]=topic;
  aPayloadPub_[numPub_]=payload;
  aRetainPub_[numPub_]=retain;
  numPub_++;
  return true;
 }
 return false;
}

//_____delete publish topic from list___________________________
// return: true=deleted, false=not
bool MqttClientKH::delPublish(String topic)
{
 //-----is topic in publish array?------------------------------
 int i=0;
 for(i=0; i<numPub_; i++)
 {
  if(topic.equals(aTopicPub_[i])) break;
 }
 if(i>=numPub_) return false;
 numPub_--;
 //-----topic in array at index i, delete from array------------
 for(int j=i; j<numPub_; j++)
 {
  aTopicPub_[j]=aTopicPub_[j+1];
  aPayloadPub_[j]=aPayloadPub_[j+1];
 }
}

//_____convert String to array and publish (without register)___
void MqttClientKH::publishString(String topic, String payload)
{
 publishString(topic, payload, false);
}

//_____convert String to array and publish______________________
void MqttClientKH::publishString(
  String topic, String payload, bool retain=false)
{
 //Serial.println("publishString: topic="+topic+", payload="+payload+", retain="+retain);
 if(payload.length()<1) return;
 char top[1+MESSAGE_MAXLEN];          // helper array
 char msg[1+MESSAGE_MAXLEN];          // helper array
 topic.toCharArray(top,MESSAGE_MAXLEN);
 payload.toCharArray(msg,MESSAGE_MAXLEN);
 publish(top,msg,retain);
 //Serial.println("publishString: published"); 
}

//_____set array of registered subscribe topics_________________
// return: number of registered subcribe topics
int MqttClientKH::setSubscribe(String aTopicSub[], int num)
{
 for(int i=0; i<num; i++)
 {
  //-----check, if topic is already registered------------------
  bool isreg=false;
  for(int j=0; j<numSub_; j++)
  {
   if(aTopicSub[i].equals(aTopicSub_[j])) {isreg=true; j=numSub_;}
  }
  //-----if not registered: add topic---------------------------
  if(!isreg)
  {
   if(numSub_<TOPIC_MAX) aTopicSub_[numSub_++]= aTopicSub[i]; 
  }
  if(numSub_>=TOPIC_MAX) return TOPIC_MAX; 
 }
 return numSub_;
}

//_____set array of registered subscribe topics_________________
// return: number of registered subcribe topics
int MqttClientKH::setPublish(
  String aTopicPub[], String aPayload[], int num)
{
 for(int i=0; i<num; i++)
 {
  //-----check, if topic is already registered------------------
  bool isreg=false;
  for(int j=0; j<numPub_; j++)
  {
   if(aTopicPub[i].equals(aTopicPub_[j])) {isreg=true; j=numPub_;}
  }
  //-----if not registered: add topic---------------------------
  if(!isreg)
  {
   if(numPub_<TOPIC_MAX)
   {
    aTopicPub_[numPub_]= aTopicPub[i];
    aPayloadPub_[numPub_++]= aPayload[i];
   }
  }
  if(numPub_>=TOPIC_MAX) return TOPIC_MAX; 
 }
 return numPub_;
}

//_____convert String to array and subscribe____________________
void MqttClientKH::subscribeString(String topic)
{
 char top[1+MESSAGE_MAXLEN];          // helper array
 topic.toCharArray(top,MESSAGE_MAXLEN);
 subscribe(top);
}

#endif

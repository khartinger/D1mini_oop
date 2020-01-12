//_____D1_class_MqttClientKH2.cpp______________170721-191225_____
// The class MqttClientKH2 extends class PubSubClient for
//  easy use of mqtt.
// You can use all commands of class PubSubClient as well.
// When PubSubClient lib is installed,
// delete PubSubClient files in directory src/mqtt
// Created by Karl Hartinger, July 21, 2017.
// Last changes: 2018-11-01 add getMyIP()
//               2019-12-22 add getsMac()
//               2019-12-23 separate WiFi and MQTT methods
//               2019-12-25 update connectWiFiMQTT()
// Hardware: D1 mini
// Released into the public domain.

#include "D1_class_MqttClientKH2.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor______________________________________________
MqttClientKH2::MqttClientKH2():PubSubClient(d1miniClient)
{
 strcpy(ssid_, MQTT_SSID);
 strcpy(pass_, MQTT_PASS);
 strcpy(mqtt_, MQTT_SERVER);
 port_=MQTT_PORT;
 setup();
}

//_____constructor 2____________________________________________
MqttClientKH2::MqttClientKH2(char* ssid, char* pwd, 
  char* mqtt_server="localhost"):PubSubClient(d1miniClient)
{
 strcpy(ssid_, ssid);
 strcpy(pass_, pwd);
 strcpy(mqtt_, mqtt_server);
 port_=MQTT_PORT;
 setup();
}

//_____constructor 3____________________________________________
MqttClientKH2::MqttClientKH2(char* ssid, char* pwd, 
  char* mqtt_server="localhost", 
  int port=1883):PubSubClient(d1miniClient)
{
 strcpy(ssid_, ssid);
 strcpy(pass_, pwd);
 strcpy(mqtt_, mqtt_server);
 port_=port;
 setup();
}

//_____setup (called by constructor)____________________________
void MqttClientKH2::setup()
{
 millisLastConnected = 0;
 numSub_=0;
 numPub_=0;
 sMyIP="xxx.xxx.xxx.xxx";
 randomSeed(micros());                 // start random numbers
 sMQTTClientName="D1mini_";
 sMQTTClientName+=String(random(0xffff), HEX);
 wifiConnectedNew=true;
 wifiConnectedNewExtern=true;
 WiFiSetup();
 setServer(mqtt_, port_);
}

//**************************************************************
// setter and getter methods
//**************************************************************

//_____get MQTT client state as string__________________________
String MqttClientKH2::getsState()
{
 String s1;
 int client_state=state();
 s1="#"+String(client_state)+" ";
 switch(client_state)
 {
  case MQTT_CONNECTION_TIMEOUT: // -4
   s1+="MQTT timeout connection"; break;
  case MQTT_CONNECTION_LOST:            // -3
   s1+="MQTT lost connection"; break;
  case MQTT_CONNECT_FAILED:             // -2
   s1+="MQTT failed to connect"; break;
  case MQTT_DISCONNECTED: // -1
   s1+="MQTT disconnected"; break;
  case MQTT_CONNECTED: // 0
   s1+="MQTT connected"; break;
  case MQTT_CONNECT_BAD_PROTOCOL: //    1
   s1+="MQTT bad protocol"; break;
  case MQTT_CONNECT_BAD_CLIENT_ID: //   2
   s1+="MQTT bad client ID"; break;
  case MQTT_CONNECT_UNAVAILABLE: //     3
   s1+="MQTT unavailable"; break;
  case MQTT_CONNECT_BAD_CREDENTIALS: // 4
   s1+="MQTT bad credentials"; break;
  case MQTT_CONNECT_UNAUTHORIZED: //    5
   s1+="MQTT unauthorized"; break;
 default: s1+="Unknown state"; break;
 }
 return s1;
}

//_____client IP address________________________________________
String MqttClientKH2::getsSSID() { return String(ssid_); }

//_____client IP address________________________________________
String MqttClientKH2::getsMyIP() { return sMyIP; }

//_____get string with D1mini MAC address_______________________
String MqttClientKH2::getsMac()
{
 String s1((char *)0);                 // help string
 byte mac[6];                          // mac bytes
 WiFi.macAddress(mac);
 s1.reserve(18);
 for(int i=0; i<6; i++)
 {
  s1+=String(mac[i],16);
  s1+=":";
 }
 s1[17]=0;
 return s1;
}

//**************************************************************
// methods for Wifi (WLAN)
//**************************************************************

//_____setup wifi mode__________________________________________
//bool MqttClientKH2::WiFiSetup(String sWifiHostname)
bool MqttClientKH2::WiFiSetup()
{
 if(WiFi.status()==WL_CONNECTED) return false; // no setup done
 delay(10);
 WiFi.mode(WIFI_STA);
 // WiFi.config(ip, gateway, subnet);  // skipp for dhcp
 WiFi.begin(ssid_, pass_);             // start connecting
 wifiConnectedNew=true;
 wifiConnectedNewExtern=true;
 //wifi_station_set_auto_connect(true);
 //wifi_station_set_hostname(sWifiHostname.c_str());
 String s1=String(ssid_);
 if(DEBUG_MQTT)Serial.println("\nWiFi setup, connecting to "+s1);
 return true;
}

//_____is client connected to WiFi (WLAN)?______________________
bool MqttClientKH2::isWiFiConnected()
{
 //-----is client connected to WiFi?----------------------------
 if(WiFi.status()==WL_CONNECTED)
 {
  if(wifiConnectedNew)
  {
   sMyIP=WiFi.localIP().toString();
   wifiConnectedNew=false;
   wifiConnectedNewExtern=true;
   if(DEBUG_MQTT)Serial.println("\nNew connection! IP address is "+sMyIP);
  }
  return true;
 }
 //-----client NOT connected to WiFi----------------------------
 wifiConnectedNew=true;
 wifiConnectedNewExtern=true;
 //newConnected=true;
 return false;
}

//_____is WiFi new connectioned?________________________________
bool MqttClientKH2::isWiFiConnectedNew()
{
 if(wifiConnectedNew || wifiConnectedNewExtern) {
  wifiConnectedNewExtern=false;
  return true;
 }
 wifiConnectedNewExtern=false;
 return false;
}

//_____WiFi signal strength_____________________________________
String MqttClientKH2::getsSignal() 
{
 if(WiFi.status()==WL_CONNECTED) return String(WiFi.RSSI());
 return String("?");
}

//**************************************************************
// methods for mqtt connection
//**************************************************************

//_____is mqtt connection ok? (no: reconnect)___________________
// MUST always be called in main loop (for receive!)
bool MqttClientKH2::isConnected()
{
 long now = millis();
 //-----WiFi connected?-----------------------------------------
 if(!isWiFiConnected()) return false;
 //-----check for mqtt connection-------------------------------
 if (!connected())
 {
  if(DEBUG_MQTT) Serial.println("WiFi ok, MQTT not connected");
  if (now - millisLastConnected > MQTT_RECONNECT_MS) 
  {
   millisLastConnected=now;
   if(reconnect()) millisLastConnected=0;
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


//_____wait for WiFi connection, then connect to MQTT server____
// return true=success, false on error
bool MqttClientKH2::connectWiFiMQTT()
{
 if(isConnected()) return true;
 if(!isWiFiConnected())
 {
  WiFiSetup();
  //-----try to connect to WLAN (access point)------------------
  int i=TIMEOUT_WIFI_CONNECT_MS/200;
  while((WiFi.status()!=WL_CONNECTED) && (i>0))
  {
   delay(200);
   i--;
   if(DEBUG_MQTT){Serial.print("."); if(i%50==0) Serial.println("");}
  }
  //-----connected to WLAN (access point)?----------------------
  if(i<1)
  { //-----not connected to WLAN--------------------------------
   if(DEBUG_MQTT) Serial.println("No connection - time-out!");
   return false;
  }
  //-----success WiFi new connection/reconnect------------------
  wifiConnectedNew=true;
  wifiConnectedNewExtern=true;
  sMyIP=WiFi.localIP().toString();
 }
 if(DEBUG_MQTT)Serial.println("\nConnected! IP address is "+sMyIP);
 //-----connect to MQTT server----------------------------------
 if(connect(sMQTTClientName.c_str()))
 {
  if(sendPubSubTopics()) return true;  // send topic info
 }
 if(DEBUG_MQTT) {Serial.println("Topics failture, state: "+getsState());};
 return reconnect();
}

//_____check for connect, if not: try to reconnect______________
bool MqttClientKH2::reconnect()
{
 //-----when connected, return----------------------------------
 if(PubSubClient::connected()) { return true; }
 if(DEBUG_MQTT) Serial.println("reconnect...");
 //-----WiFi connected?-----------------------------------------
 if(!isWiFiConnected()) return false;  // no WiFi
 //-----WiFi yes, mqtt no---------------------------------------
 if(DEBUG_MQTT)Serial.println("MQTT: Not connected - reconnect...");
 if(!PubSubClient::connect(sMQTTClientName.c_str())) return false;
 //-----MQTT: try to send all PubSub topics---------------------
 if(!sendPubSubTopics())
 {
  if(DEBUG_MQTT) {Serial.println("failed, state: "+getsState());};
  return false;
 }
 return true;
}

//-----MQTT: send all PubSub topics (e.g. on reconnect)---------
bool MqttClientKH2::sendPubSubTopics()
{
 //-----If no wifi ClientId: create a random client ID----------
 String clientId=sMQTTClientName;
 if(clientId=="")
 {
  randomSeed(micros());           // start random numbers
  clientId = "D1mini_Client_";
  clientId += String(random(0xffff), HEX);
 }
 //-----Try to connect------------------------------------------
 if(DEBUG_MQTT) Serial.println("sendPubSubTopics: clientId="+ clientId);
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
  if(DEBUG_MQTT) printPubSubTopics2Serial(clientId);
  return true;
 }
 return false;
}

//-----send all PubSub topics to Serial-------------------------
// just for test purpose
void MqttClientKH2::printPubSubTopics2Serial(String clientId="")
{
 if(clientId=="") clientId=sMQTTClientName;
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
bool MqttClientKH2::addSubscribe(String topic)
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
bool MqttClientKH2::delSubscribe(String topic)
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
 char cTopic[1+TOPIC_MAXLEN];          // helper array
 topic.toCharArray(cTopic,TOPIC_MAXLEN);
 unsubscribe(cTopic);
}

//_____add a (String) topic to publish array____________________
bool MqttClientKH2::addPublish(String topic)
{ return addPublish(topic,String(""),true); }

//_____add a (String) topic to publish array____________________
// if topic exists --> replace payload
bool MqttClientKH2::addPublish(String topic, String payload, 
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
bool MqttClientKH2::delPublish(String topic)
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
bool MqttClientKH2::publishString(String topic, String payload)
{
 return(publishString(topic, payload, false));
}

//_____convert String to array and publish______________________
bool MqttClientKH2::publishString(
  String topic, String payload, bool retain=false)
{
 //Serial.println("publishString: topic="+topic+", payload="+payload+", retain="+retain);
 if(payload.length()<0) return false;
 char top[1+TOPIC_MAXLEN];             // helper array
 char msg[1+PAYLOAD_MAXLEN];           // helper array
 topic.toCharArray(top,TOPIC_MAXLEN);
 payload.toCharArray(msg,PAYLOAD_MAXLEN);
 //Serial.println("publishString: published"); 
 return(publish(top,msg,retain));
}

//_____set array of registered subscribe topics_________________
// return: number of registered subcribe topics
int MqttClientKH2::setSubscribe(String aTopicSub[], int num)
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
int MqttClientKH2::setPublish(
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
void MqttClientKH2::subscribeString(String topic)
{
 char top[1+PAYLOAD_MAXLEN];           // helper array
 topic.toCharArray(top,TOPIC_MAXLEN);
 subscribe(top);
}


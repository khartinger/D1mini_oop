//_____D1_class_MqttClientKH2.h_______________170721-191225_____
// The class MqttClientKH2 extends class PubSubClient for
//  easy use of mqtt.
// You can use all commands of class PubSubClient as well.
// When PubSubClient lib is installed,
// delete PubSubClient files in directory src/mqtt
// Created by Karl Hartinger, July 21, 2017.
// Last changes: 2018-11-01 getMyIP() added
//               2019-12-22 getsMac() added
//               2019-12-23 WiFi and MQTT methods separated
// Hardware: D1 mini
// Released into the public domain.

#ifndef D1_CLASS_MqttClientKH2_H
#define D1_CLASS_MqttClientKH2_H
#include "Arduino.h"                   // 
#include <ESP8266WiFi.h>               // WiFi
#include "PubSubClient.h"              // 
#ifndef  MQTT_SSID
 #define MQTT_SSID      "ssid"         // wlan name
#endif
#ifndef  MQTT_PASS
 #define MQTT_PASS      ""             // no password
#endif
#ifndef  MQTT_SERVER
 #define MQTT_SERVER    "localhost"    // default server name
#endif
#ifndef  MQTT_PORT
 #define MQTT_PORT      1883           // default mqtt port
#endif
#define  SSID_SIZE      20             // max.len ssid
#define  PASS_SIZE      20             // max.len password
#define  MQTT_SIZE      20             // max.len mqttservername
#define  TOPIC_MAX      8              // max. topics to sub
#define  MQTT_RECONNECT_MS        4000
#define  TIMEOUT_WIFI_CONNECT_MS  5000 // wait for WLAN
#define  TOPIC_MAXLEN               48
#define  PAYLOAD_MAXLEN            200
#ifndef  DEBUG_MQTT
 #define DEBUG_MQTT               true // true=Serial output
#endif

class MqttClientKH2 : public PubSubClient {
 //-----properties----------------------------------------------
 protected:
  char ssid_[SSID_SIZE+1];             // WiFi newtwork name
  char pass_[PASS_SIZE+1];             // WiFi password
  char mqtt_[MQTT_SIZE+1];             // MQTT server (name or ip)
  int  port_;                          // mqtt port (def 1883)
  String aTopicSub_[TOPIC_MAX];        // subscribed topics
  String aTopicPub_[TOPIC_MAX];        // topics to publish
  String aPayloadPub_[TOPIC_MAX];      // value on (re)start
  bool   aRetainPub_[TOPIC_MAX];       // retain true|false
  int    numSub_;                      // number subscribed topics
  int    numPub_;                      // number topics to publish
  WiFiClient d1miniClient;             // WLAN client for MQTT
  String sMQTTClientName;              // MQTT client name
  String sMyIP;                        // client IP address
  long   millisLastConnected;          // last connection time [ms]
  bool   wifiConnectedNew;             // internal usage
  bool   wifiConnectedNewExtern;       // external usage
 //-----constructor & co----------------------------------------
 public:
  MqttClientKH2();
  MqttClientKH2(char* ssid, char* pwd, char* mqtt_server);
  MqttClientKH2(char* ssid, char* pwd, char* mqtt_server, int port);
 protected:
  void setup();                        // called by constructors
 //-----setter and getter methods-------------------------------
 public:
  int    getNumSub() { return numSub_; };
  int    getNumPub() { return numPub_; };
  void   clrSubscribe() { numSub_=0; };
  void   clrPublish() { numPub_=0; };
  void   setMQTTClientName(String sName) {sMQTTClientName=sName;};
  String getMQTTClientName() { return sMQTTClientName; };
  String getsState();
  String getsSSID();
  String getsMyIP();
  String getsMac();
  //-----methods for Wifi (WLAN)--------------------------------
  bool   WiFiSetup();                    // called by setup()
  bool   isWiFiConnected();              // is connected to WiFi
  bool   isWiFiConnectedNew();           // is new WiFi connection
  String getsSignal();                   // WiFi signal strength
  //-----methods for mqtt connection----------------------------
  bool   isConnected();                  //MUST always be called
  bool   connectWiFiMQTT();              // connect MQTT, wait
  bool   reconnect();                    // try to connect MQTT
  bool   sendPubSubTopics();             // send topics to MQTT
  void   printPubSubTopics2Serial(String clientId);
  //-----methods to define mqtt topics--------------------------
  bool   addSubscribe(String topic);
  bool   delSubscribe(String topic);
  bool   addPublish(String topic);
  bool   addPublish(String topic, String payload, bool retain);
  bool   delPublish(String topic);
  bool   publishString(String topic, String payload);
  bool   publishString(String topic, String payload, bool retain);
  //-----seldom used-------------------------------------------
  int    setSubscribe(String aTopicSub[], int num);
  int    setPublish(String aTopicPub[], String aPayload[], int num);
  void   subscribeString(String topic);
};
#endif

//_____D1_class_SimpleMqtt.cpp________________200705-201108_____
// The SimpleMqtt class is suitable for D1 mini (ESP8266) and 
// and ESP32 D1mini and extends the PubSubClient class to make
// MQTT easy to use.
// * For this purpose a "base" topic (topicbase, default is
//   simplemqtt/default) is defined, which is extended by the
//   following keywords:
// /get Request of the value specified in the payload
//      e.g. version query:
//      -t simplemqtt/default/get -m version
// /set Set a value e.g. 
//      -t simplemqtt/default/set/topicbase -m new_base
// * In response you get a MQTT message with the keyword /ret,
//    e.g. -t simplemqtt/default/ret/version -m 2020-12-08
// * Furthermore, all (arbitrary) topics defined in the constant
//   TOPIC_SUB are subscribed.
// * Furthermore, all commands of the PubSubClient class can 
//   still be used.
//
// In the user program the following string constants must be
// defined. Type of board:
// *  #define  D1MINI         1            // ESP8266 D1mini +pro
// OR #define  ESP32D1        2            // ESP32 D1mini
// Topic base and key words for get-, set, sub-, pub-action
// (key words comma separated or empty string):
// * TOPIC_BASE, TOPIC_GET, TOPIC_SET, TOPIC_SUB, TOPIC_PUB
// Furthermore, the following functions must be implemented
// in the user program:
// * void callback(char* topic, byte* payload, unsigned int length)
//   { client.callback_(topic, payload, length); }
// * String simpleGet(String sPayload) { return String(""); }
// * String simpleSet(String sTopic, String sPayload) { return String(""); }
// * void simpleSub(String sTopic, String sPayload) { }
// Finally, an object must be created in the user program, e.g.
// * SimpleMqtt client("ssid", "password", "mqttservername|ip");
// and in the loop() function the method 
// * client.doLoop();
// must be called, otherwise no MQTT messages are processed!
//
// Note: If the PubSubClient class is registered (installed)
//       in the IDE, the PubSubClient files in the directory
//       src/simplemqtt should be deleted.
// Created by Karl Hartinger, December 08, 2020.
// Changes:
// 2020-12-08 first release
// Hardware: D1 mini OR ESP32 D1mini
// Released into the public domain.

#include "D1_class_SimpleMqtt.h"

//**************************************************************
//    constructor & co
//**************************************************************

//_____constructor______________________________________________
SimpleMqtt::SimpleMqtt():PubSubClient(d1miniClient)
{
 ssid_=String(MQTT_SSID);
 pass_=String(MQTT_PASS);
 mqtt_=String(MQTT_SERVER);
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}

//_____constructor 2____________________________________________
SimpleMqtt::SimpleMqtt(char* ssid, char* pwd)
  :PubSubClient(d1miniClient)
{
 ssid_=String(ssid);
 pass_=String(pwd);
 mqtt_=String(MQTT_SERVER);
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}

/*
//_____constructor 3____________________________________________
SimpleMqtt::SimpleMqtt(char* ssid, char* pwd, 
  char* mqtt_server):PubSubClient(d1miniClient)
{
 ssid_=String(ssid);
 pass_=String(pwd);
 mqtt_=String(mqtt_server);
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}
*/

//_____constructor 4____________________________________________
SimpleMqtt::SimpleMqtt(char* ssid, char* pwd, 
  char* mqtt_server, char* topicbase)
  :PubSubClient(d1miniClient)
{
 ssid_=String(ssid);
 pass_=String(pwd);
 mqtt_=String(mqtt_server);
 port_=MQTT_PORT;
 sTopicBaseDefault=String(topicbase);
 setup();
}

//_____constructor 5____________________________________________
SimpleMqtt::SimpleMqtt(String sssid, String spwd, String smqtt_server)
:PubSubClient(d1miniClient)
{
 ssid_=sssid;
 pass_=spwd;
 mqtt_=smqtt_server;
 port_=MQTT_PORT;
 sTopicBaseDefault=SIMPLEMQTT_BASE;    // "path" of topics
 setup();
}

//_____setup (called by constructor)____________________________
void SimpleMqtt::setup()
{
 wifiWaitMsMax=TIMEOUT_WIFI_CONNECT_MS;
 wifiConnectingCounterMax=WIFI_CONNECTING_COUNTER;
 wifiConnectingCounter=0;              // start with connectingWiFiBegin
 sMyIP=NO_IP;                          // invalid IP
 randomSeed(micros());                 // start random numbers
 sMQTTClientName="D1_";                //
 sMQTTClientName+=String(random(0xffff), HEX);
 conState=CON_RESET;                   // WiFi + MQTT connection
 eepromSize_=EEPROM_SIZE;              //
 iGet=NOTHING_TODO;                    // no get answers to do
 iSet=NOTHING_TODO;                    // no set commands to do
 iRet=NOTHING_TODO;                    //
 iSub=NOTHING_TODO;                    //
 iPub=NOTHING_TODO;                    //
 numTopicGet=0;                        // no get topics yet
 numTopicSet=0;                        // no set topics yet
 numTopicSub=0;                        // no sub topics yet
 numTopicPub=0;                        // no pub topics yet
 eeprom_=new EEPROMClass;              // eeprom object
 begin(0);                             // sets sTopicBase
}

//_____init objects_____________________________________________
// read topic base from eeprom (or use default on error)
// iSource=0: called by constructor, 1: called by setup()
bool SimpleMqtt::begin(int iSource)
{
 int ret=true;
 //-----read topic from eeprom----------------------------------
 int iResult;
 sTopicBase=eepromReadTopicBase(iResult);
 if(iResult!=0) {
  sTopicBase=sTopicBaseDefault;        // "path" of topics
  if(DEBUG_MQTT) Serial.printf("begin(): topic base - use default %s\n",sTopicBase.c_str());
  ret=false;
 }
 if(DEBUG_MQTT) Serial.printf("begin(): topic base %s (from eeprom)\n",sTopicBase.c_str());
 return ret;
}

//_____init objects_____________________________________________
bool SimpleMqtt::begin() { return begin(1); }
 
//**************************************************************
// setter and getter methods
//**************************************************************

//_____set (used) EEPROM size___________________________________
void SimpleMqtt::setEepromSize(int eepromSize) {
 if(eepromSize>8 && eepromSize<=EEPROM_SIZE_MAX)
  eepromSize_= eepromSize; 
}

//_____Maximum Milliseconds to weit for WiFi to be connected____
void SimpleMqtt::setWiFiWaitingTime(int ms) 
{ if(ms>=100) wifiWaitMsMax=ms; }

//_____number of connectingWiFi() after connectingWiFiBegin()_
void SimpleMqtt::setWiFiConnectingCounter(int number)
{ if(number>0) wifiConnectingCounterMax=number; }


//_____get MQTT client state as string__________________________
String SimpleMqtt::getsState()
{
 String s1;
 int client_state=state();
 s1="#"+String(client_state)+" ";
 switch(client_state)
 {
  case MQTT_CONNECTION_TIMEOUT:        // -4
   s1+="MQTT timeout connection"; break;
  case MQTT_CONNECTION_LOST:           // -3
   s1+="MQTT lost connection"; break;
  case MQTT_CONNECT_FAILED:            // -2
   s1+="MQTT failed to connect"; break;
  case MQTT_DISCONNECTED:              // -1
   s1+="MQTT disconnected"; break;
  case MQTT_CONNECTED:                 //  0
   s1+="MQTT connected"; break;
  case MQTT_CONNECT_BAD_PROTOCOL:      //  1
   s1+="MQTT bad protocol"; break;
  case MQTT_CONNECT_BAD_CLIENT_ID:     //  2
   s1+="MQTT bad client ID"; break;
  case MQTT_CONNECT_UNAVAILABLE:       //  3
   s1+="MQTT unavailable"; break;
  case MQTT_CONNECT_BAD_CREDENTIALS:   //  4
   s1+="MQTT bad credentials"; break;
  case MQTT_CONNECT_UNAUTHORIZED:      //  5
   s1+="MQTT unauthorized"; break;
 default: s1+="Unknown state"; break;
 }
 return s1;
}

//_____client name of WiFi network______________________________
String SimpleMqtt::getsSSID() { return ssid_; }

//_____client IP address as string______________________________
String SimpleMqtt::getsMyIP() { return sMyIP; }

//_____get D1mini MAC address as string_________________________
String SimpleMqtt::getsMac()
{
 char cmac[18];                        // 12+5+1=18 bytes
 byte mac[6];                          // mac bytes
 WiFi.macAddress(mac);
 sprintf(cmac,"%02X:%02X:%02X:%02X:%02X:%02X",
  mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
 cmac[17]=0;
 String s1=String(cmac);
 return s1;
}

//_____get "path" for mqtt messages_____________________________
String SimpleMqtt::getsTopicBase() { return sTopicBase; }

//**************************************************************
//    methods to declare mqtt topics
//**************************************************************

//_____set "path" for MQTT commands (+ eeprom)__________________
// changes sTopicBase in RAM and EEPROM.
// Method does NOT subscribe new topics on MQTT broker! 
// (If you want this, use changeTopicBase() instead)
// return: 0=OK, 1=don't use EEPROM, 2=topic too long, 4=eeprom
//         write error, 8,16=eeprom read error
int SimpleMqtt::setTopicBase(String topicBasis)
{ 
 int ret=0;
 int len1=topicBasis.length();
 if(len1>=TOPIC_MAXLEN) 
 {
  if(DEBUG_MQTT) Serial.printf("setTopicBase(): failed! Topic %s too long",topicBasis.c_str());
  return 2;                            // topic too long
 }
 //------write new topic to eeprom------------------------------
 if(USE_EEPROM)
 {
  int len2=eepromWriteTopicBase(topicBasis);
  if(len1==len2) 
  {//.........write to eeprom ok................................
   int iResult;
   String s1=eepromReadTopicBase(iResult);
   if(iResult!=0) ret|=8;              // eeprom read error
  }
  else ret|=4;                         // eeprom write error
 }
 else ret|=1;                          // don't use eeprom
 if(DEBUG_MQTT) Serial.printf("setTopicBase(): %s ",topicBasis.c_str());
 if(DEBUG_MQTT) {if(ret>0) Serial.printf("failed! ret=%d\n",ret); else Serial.println("OK");} 
 return ret;
}

//_____set default "path" for MQTT commands (+ eeprom)__________
// return: 0=OK, 1=don't use EEPROM, 2=topic too long, 4=eeprom
//         write error, 8,16=eeprom read error
int SimpleMqtt::setTopicBaseDefault(String topicBasisDefault)
{ 
 int ret=0;
 if(DEBUG_MQTT) Serial.printf("setTopicBaseDefault(): %s ",topicBasisDefault.c_str());
 if(sTopicBaseDefault==SIMPLEMQTT_BASE)
  sTopicBaseDefault=topicBasisDefault;
 if(sTopicBase==SIMPLEMQTT_BASE)
 {
  sTopicBase=topicBasisDefault;
 }
 else
 {
  sTopicBase=getsTopicBase();
 }
 if(DEBUG_MQTT) {if(ret>0) Serial.printf("failed! ret=%d\n",ret); else Serial.println("OK");} 
 return ret;
}

//_____set all (comma separated) topics_________________________
void SimpleMqtt::setTopics(String sAllGet, String sAllSet, 
                           String sAllSub, String sAllPub)
{
 setTopicGet(sAllGet);
 setTopicSet(sAllSet);
 setTopicSub(sAllSub);
 setTopicPub(sAllPub);
}

//_____set all get-topics as comma separated strings____________
int SimpleMqtt::setTopicGet(String sAllGet)
{
 numTopicGet=splitString(sAllGet, aTopicGet);
 return numTopicGet;
}

//_____set all set-topics as comma separated strings____________
int SimpleMqtt::setTopicSet(String sAllSet)
{
 numTopicSet=splitString(sAllSet, aTopicSet);
 return numTopicSet;
}

//_____set all sub(scribe)-topics as comma separated strings____
int SimpleMqtt::setTopicSub(String sAllSub)
{
 numTopicSub=splitString(sAllSub, aTopicSub);
 return numTopicSub;
}
 
//_____set all pub(lish)-topics as comma separated strings______
int SimpleMqtt::setTopicPub(String sAllPub)
{
 numTopicPub=splitString(sAllPub, aTopicPub);
 return numTopicPub;
}

//**************************************************************
// methods for Wifi (WLAN)
//**************************************************************

//_____try to connect to WiFi, wait max. wifiWaitMsMax__________
// return: true=connected (OK)
bool SimpleMqtt::connectWiFi() {
 wifiConnectingCounter--;              //connectings after begin
 if(wifiConnectingCounter<1)
 {
  wifiConnectingCounter=wifiConnectingCounterMax;
  if(!connectingWiFiBegin()) return false;
 }
 return connectingWiFi();
}

//_____start a new WiFi connection______________________________
// stopp a connection, if one existed before
// return: true=connected (OK)
bool SimpleMqtt::connectingWiFiBegin()
{
 disconnectWiFi();
 if(isWiFiConnected()) 
 {//-----error: could not disconnect WiFi-----------------------
  conState|=BIT_CONN_ERROR;            // set error bit
  return false;
 }
 delay(20);
 if(DEBUG_MQTT) Serial.print("connectingWiFiBegin(): ");
#if defined(ESP8266) || defined(D1MINI)
 if(DEBUG_MQTT) Serial.print("mode() OK - ");  
 WiFi.mode(WIFI_STA);                  // D1mini is station
#endif
 // WiFi.config(ip, gateway, subnet);  // skipp for dhcp
 //wifi_station_set_auto_connect(true);
 //wifi_station_set_hostname(sWifiHostname.c_str());
 //-----try to connect to WiFi (access point)-------------------
 String s1=String(ssid_);              // WLAN name
 //WiFi.begin(ssid_, pass_);           // start connecting!
 WiFi.begin((char*) ssid_.c_str(), (const char *)pass_.c_str());    // start connecting!
 if(DEBUG_MQTT) Serial.println("WiFi connecting to "+s1); 
 if(isWiFiConnected()) 
 {//-----normally no such quick connection...-------------------
  sMyIP=WiFi.localIP().toString();          // get ip
 }
 else
 {//-----normal case--------------------------------------------
  sMyIP=NO_IP;                              // no IP
 }
 return true;                               // connection startet
}

//_____wait for WiFi connection to be established_______________
// Sets smyIP
// return: true=connection established
bool SimpleMqtt::connectingWiFi() { return connectingWiFi(1); }

//_____wait n times for WiFi connection to be established_______
// Waiting time = wifiWaitMsMax * attempts
// Sets smyIP
// return: true=connection established
bool SimpleMqtt::connectingWiFi(int attempts)
{
 //unsigned long nextMillis;
 //------check connection---------------------------------------
 if(isWiFiConnected()) 
 {
  sMyIP=WiFi.localIP().toString();          // get ip
  return true;                              // connection state
 }
 //-----try to connect------------------------------------------
 if(attempts<1) attempts=1;                 //try min. 1x
 int i=wifiWaitMsMax/100;                   //100ms waiting step
 if(i<1) i=1;                               // minimum 100ms
 i=i*attempts;                              // counter (100ms steps)
 if(DEBUG_MQTT) Serial.print("connectingWiFi() - Waiting for connect ");
 
 while(!isWiFiConnected() && (i>0))
 {
  //nextMillis=millis()+100;
  //while(millis()<nextMillis) yield();       // wait 100ms
  delay(100);                             // wait 100ms
  i--;                                      // decrement counter
  if(DEBUG_MQTT){Serial.print("."); if(i%50==0) Serial.println("");}
 }
 //-----connected to WLAN (access point)?----------------------
 if(i<1)                                    // counter=minimum
 { 
  if(!isWiFiConnected())
  {//-----not connected to WLAN---------------------------------
   if(DEBUG_MQTT) Serial.println("connectingWiFi(): time-out!");
   sMyIP=NO_IP;                             // no IP
   return false;                            // connection state
  }
 }
 //-----success WiFi new connection/reconnect------------------
 sMyIP=WiFi.localIP().toString();           // get IP
 if(DEBUG_MQTT) Serial.println("\nconnectingWiFi(): New connection to "+String(ssid_)+", IP="+sMyIP+" - OK");
 return true;                               // connection state
}

//_____disconnect a WiFi connection_____________________________
// return: true=disconnected, false=could not disconnect Wifi
bool SimpleMqtt::disconnectWiFi()
{
 int i=20;
 //bool bWiFiLast=((conState&BIT_WIFI_LAST)>0) ? true : false;
 //------(try to) disconnect------------------------------------
 while(isWiFiConnected() && (i>0))
 {
  i--;                                      // increment trials
  WiFi.disconnect();                        // disconnect WLAN 
  delay(50);                                // wait a litte bit
 }
 //------disconnected-------------------------------------------
 if(WiFi.status()==WL_DISCONNECTED) {
  //conState|=BIT_WIFI_DIS;
  if(DEBUG_MQTT) Serial.println("disconnectWiFi(): OK");
  return true;
 }
 //-----error while disconnecting-------------------------------
 if(!isWiFiConnected()) {
  //conState|=BIT_WIFI_DIS;
  if(DEBUG_MQTT) Serial.println("disconnectWiFi(): No Wifi - OK");
  return true;
 }
 //-----other error/state---------------------------------------
 if(DEBUG_MQTT){Serial.print("disconnectWiFi(): Error #");Serial.println(WiFi.status());}
 conState|=BIT_CONN_ERROR;                  // set error bit
 return false;
}

//_____is client connected to WiFi (WLAN)?______________________
// uses WiFi.status()
// If there was no change of status, the status word is not
// changed. The current status becomes the old status.
// return: true = WiFi connection ok
bool SimpleMqtt::isWiFiConnected() {
 //------get last two WiFi states (now and last)----------------
 bool bWiFiNow=(WiFi.status()==WL_CONNECTED);
 bool bWiFiLast=((conState&BIT_WIFI_LAST)>0) ? true : false;
 if(bWiFiNow && (!bWiFiLast))
 {//-----last 0 -> 1 now: new connection!-----------------------
  conState |= BIT_WIFI_CON;            // set new connection bit
  conState &= (~BIT_WIFI_DIS);         // forget disconnect
 }
 if((!bWiFiNow) && bWiFiLast)
 {//-----last 1 -> 0 now: connection lost!----------------------
  conState |= BIT_WIFI_DIS;            // set disconnection bit
  conState &= (~BIT_WIFI_CON);         // forget connect bit
  //.....WiFi lost -> MQTT connection lost......................
  conState &= (~BIT_MQTT_LAST);        // MQTT-state is no MQTT
  conState |= BIT_MQTT_DIS;            // MQTT lost
  conState &= (~BIT_MQTT_CON);         // no MQTT connection
  //.....next connecting with ConnectingBegin().................
  wifiConnectingCounter=0;
 }
 //-----Make the current status the old status------------------
 if(bWiFiNow) conState|=BIT_WIFI_LAST; // set last WiFi state
 else conState&=(~BIT_WIFI_LAST);      // clear last WiFi state
 return bWiFiNow;
}

//_____is WiFi new connected?___________________________________
// just read conState, NO WiFi.status() call!
// return: true = new connected
bool SimpleMqtt::isWiFiConnectedNew()
{
 bool wifiConNew = ((conState&BIT_WIFI_CON)>0) ? true : false;
 conState&=(~BIT_WIFI_CON);            // connection bit read
 return wifiConNew;
}

//_____is WiFi new disconnected?________________________________
// just read conState, NO WiFi.status() call!
// return: true = new disconnected
bool SimpleMqtt::isWiFiDisconnectedNew()
{
 bool wifiDisNew = ((conState&BIT_WIFI_DIS)>0) ? true : false;
 conState&=(~BIT_WIFI_DIS);            // disconnection bit read
 return wifiDisNew;
}

//_____get WiFi signal strength (or ? on error)_________________
// return: signal strength or ? on error
String SimpleMqtt::getsSignal() 
{
 if(isWiFiConnected()) return String(WiFi.RSSI());
 return String("?");
}

//**************************************************************
// methods for mqtt connection
//**************************************************************

//_____wait for WiFi connection, then connect to MQTT server____
// return: true=MQTT connected
bool SimpleMqtt::connectWiFiMQTT()
{
 //------is client NOT connected to WiFi?-----------------------
 //if(WiFi.status()!=WL_CONNECTED)
 if(!isWiFiConnected())
 {//-----try to (re)connect to WiFi-----------------------------
  if(!connectWiFi()) return false;     // not connected
 }
 return connectMQTT();                 // try to connect
}

//_____try to connect to MQTT server____________________________
// return: true = connected to MQTT broker
//         details in connState
bool SimpleMqtt::connectMQTT()
{
 int iRet=0;
 //------is client already connected to MQTT broker?------------
 if(isMQTTConnected()) return true;
 //------(try to) connect to MQTT server (normal case)----------
 for(int i=3; i>0; i--)
 {//----try to connect to MQTT server---------------------------
  setServer(mqtt_.c_str(), port_);
  if(PubSubClient::connect(sMQTTClientName.c_str()))
  {//---connected to MQTT server: subscribe topics--------------
   if(!isMQTTConnected()) return false;
   iRet=changeSubscribe(sTopicBase, sTopicBase); // 
   if(iRet==0 || iRet==32)
   {//-----subscribe all topics is OK---------------------------
    if(STARTINFO_ALLOW)
     publish(STARTINFO_TOPIC, sTopicBase.c_str());//start message
     if(DEBUG_MQTT) Serial.println("connectMQTT(): OK");
    return true;                       // connected!
   }
   else
   {//-----could not subscribe topics---------------------------
    if(DEBUG_MQTT) Serial.println("connectMQTT(): Subscribe Error");
    PubSubClient::disconnect();        // try again
   }
  }
  delay(MQTT_RECONNECT_MS);            // wait a little bit
 }
 //------not connected to MQTT broker---------------------------
 if(DEBUG_MQTT) Serial.println("connectMQTT(): MQTT error!");
 conState&=(~BIT_MQTT_LAST);
 conState&=(~BIT_MQTT_CON);            // MQTT not connected
 return false;                         // no connection
}

//_____is mqtt connection ok? (no: reconnect)___________________
// return: true=connected to MQTT server
// MUST always be called in main loop (for receive!)
bool SimpleMqtt::checkMQTT()
{
 if(!isMQTTConnected()) return false;  // no MQTT connection
 //-----if connected to broker, do loop function----------------
 PubSubClient::loop();                // message received?
 return true;
}

//_____is mqtt connection ok?___________________________________
// uses WiFi.status() and PubSubClient::connected()
// If there was no change of status, the status word is not
// changed. The current status becomes the old status.
// return: true = MQTT connection ok
bool SimpleMqtt::isMQTTConnected()
{
 if(!isWiFiConnected()) return false;  // NOT connected to WiFi
 //------get last two MQTT states (now and last)----------------
 bool bMqttNow =PubSubClient::connected();
 bool bMqttLast=((conState&BIT_MQTT_LAST)>0) ? true : false;
 if(bMqttNow && (!bMqttLast))
 {//-----last 0 -> 1 now: new connection!-----------------------
  conState |= BIT_MQTT_CON;            // set new connection bit
  conState &= (~BIT_MQTT_DIS);          // forget disconnect
 }
 if((!bMqttNow) && bMqttLast)
 {//-----last 1 -> 0 now: connection lost!----------------------
  conState |= BIT_MQTT_DIS;            // set disconnection bit
  conState &= (~BIT_MQTT_CON);         // forget connect bit
 }
 //-----Make the current status the old status------------------
 if(bMqttNow) conState|=BIT_MQTT_LAST; // set last WiFi state
 else conState&=(~BIT_MQTT_LAST);      // clear last WiFi state
 return bMqttNow;
}


//_____is MQTT new connected?___________________________________
// just read conState, NO PubSubClient::connected() call!
// return: true = new connection to MQTT broker
bool SimpleMqtt::isMQTTConnectedNew()
{
 bool mqttConNew = ((conState&BIT_MQTT_CON)>0) ? true : false;
 conState&=(~BIT_MQTT_CON);            // connection bit read
 return mqttConNew;
}

//_____is MQTT new disconnected?________________________________
// just read conState, NO PubSubClient::connected() call!
// return: true = new disconnection from MQTT broker
bool SimpleMqtt::isMQTTDisconnectedNew()
{
 bool mqttDisNew = ((conState&BIT_MQTT_DIS)>0) ? true : false;
 conState&=(~BIT_MQTT_DIS);            // disconnection bit read
 return mqttDisNew;
}

//**************************************************************
//    mqtt (main) loop control
//**************************************************************

//_____control mqtt in main loop (with reconnect)_______________
// calls checkMQTT(); that MUST be called every loop
bool SimpleMqtt::doLoop(void) { return(doLoop(true)); }
  
//_____control mqtt in main loop (without reconnect)____________
// calls checkMQTT(); that MUST be called every loop
// uses public functions 
// String mqttloopGet(String sPayload)
// String mqttloopSet(String sTopic, String sPayload)
// void   mqttloopSub(String sTopic, String sPayload)
// return: true=PubSubClient loop() done
bool SimpleMqtt::doLoop(bool tryToReconnect)
{
 bool bRet;
 //======SECTION 1: action at the beginning of loop=============
 //-----process set requests------------------------------------
 if(iSet>0)
 {
  for(int i=0; i<numTopicSet; i++)
  {
   if((iSet&(1<<i))>0)
   {
    String s1=simpleSet(aTopicSet[i],aPayloadSet[i]);
    if(s1.length()>0)
    {
     aPayloadSet[i]=s1;
     iRetSet|=(1<<i);
    }
    iSet&=(~(1<<i));
   }  
  }
 }
 //-----process sub requests------------------------------------
 if(iSub>0)
 {
  for(int i=0; i<numTopicSub; i++)
  {
   if((iSub&(1<<i))>0)
   {
    simpleSub(aTopicSub[i],aPayloadSub[i]);
    iSub&=(~(1<<i));
   }  
  }
 }
 //======SECTION 2: check for WiFi and MQTT connection==========
 bRet=checkMQTT();                // MUST be called for receive!
 if(bRet)
 {//-----connected to MQTT server-------------------------------
  if(iGet>0) createGetAnswer();
 }
 else
 {//-----not connected! Try to connect--------------------------
  if(tryToReconnect) connectWiFiMQTT();
 }
 //------send messages------------------------------------------
 if((iRet>0)||(iRetSet>0)||(iPub>0)) sendRet();
 return bRet;                               // result checkMQTT
}

//**************************************************************
//    internal callback method
//    MUST be called by global callback routine!
//**************************************************************

//_____internal callback method_________________________________
// MQTT: inspect all subscribed incoming messages
// * If "get" topic is valid, the appropriate bit is set in iGet
// * If "set" topic is valid, the appropriate bit is set in iSet
//   and the payload is copied to sSetPayload[]
// * Special topics are specially treated
void SimpleMqtt::callback_(char* topic, byte* payload, unsigned int length)
{
 int i=-1;
 //=====SECTION 1: convert payload to array (and show message)==
 //-----build get/set/...topic----------------------------------
 int lenTopic=strlen(topic);
 char cTopic[5+lenTopic];
 sprintf(cTopic,"%s/get",sTopicBase.c_str()); // "get" topic
 char cPayload[length+1];                   //char-array payload
 strncpy(cPayload,(char*)payload,length);   // copy payload bytes
 cPayload[length]=0;                        // set end char
 if(DEBUG_MQTT) Serial.printf("callback_(): topic '%s': %s\n",topic,cPayload);
 //======SECTION 2: GET request -> trigger answer===============
 if(strcmp(topic,cTopic)==0)
 {//-----check for valid get request (see array sGet[]----------
  for(i=0; i<numTopicGet; i++) {
   if(strcmp(aTopicGet[i].c_str(), cPayload)==0)
   { 
    if(DEBUG_MQTT) Serial.printf("callback_(): ==> MQTT process get '%s'\n",cPayload);
    iGet|=(1<<i); 
    i=-1;                               // finish for
    break; 
   }
  }
  if(DEBUG_MQTT && i!=-1) Serial.printf("callback_(): *** no valid get payload found ***\n");
 }
 //======SECTION 3: SET request -> trigger action===============
 // do action in main loop to save time in callback function (!)
 sprintf(cTopic,"%s/set/",sTopicBase.c_str()); // "set" topic
 lenTopic=strlen(cTopic);              // len of ".../set/"
 if(strncmp(topic,cTopic,lenTopic)==0) // check first part
 {//-----correct first part of set topic found------------------
  // check for valid 2nd part of set request (= aTopicSet[])
  char* settype=topic;                 // point to topic begin
  settype+=lenTopic;                   // point to topic command
  for(i=0; i<numTopicSet; i++) {
   if(strcmp(settype,aTopicSet[i].c_str())==0)
   {
    if(DEBUG_MQTT) Serial.printf("callback_(): ==> MQTT command set %s=%s\n",(aTopicSet[i]).c_str(),cPayload);
    iSet|=(1<<i);                       // trigger get request
    aPayloadSet[i]=String(cPayload);    // save payload
    i=-1;                               // finish for
    break; // set-command?
   }
  }
  if(DEBUG_MQTT && i!=-1) Serial.printf("callback_(): *** no valid set topic found ***\n");
 }
 //=====SECTION 4: special MQTT (input) messages================
 for(i=0; i<numTopicSub; i++) {
  if(strcmp(aTopicSub[i].c_str(),topic)==0){
   if(DEBUG_MQTT) Serial.printf("callback_(): ==> MQTT special: topic %s, payload %s\n",(aTopicSub[i]).c_str(),cPayload);
   iSub|=(1<<i);                       // trigger get request
   aPayloadSub[i]=String(cPayload);    // save payload
   break; // set-command?
  }
 }
}

//**************************************************************
//    mqtt working methods
//**************************************************************

//_____set new topic base, unsubscribe old topics, subscribe new
//     topics___________________________________________________
// return: true: topics changes, false: error
bool SimpleMqtt::changeTopicBase(String newBase)
{
 return changeTopicBase(sTopicBase, newBase);
}

//_____set new topic base, unsubscribe old topics, subscribe new
//     topics___________________________________________________
// return: true: topics changes, false: error
bool SimpleMqtt::changeTopicBase(String oldBase, String newBase)
{
 String tempBase=sTopicBase;
 int  iRet=setTopicBase(newBase);      //try to change topic name
 if(iRet!=0) 
 {//-----could not change sTopicBase----------------------------
  sTopicBase=tempBase;                 // reset old value
  return false;                        // change failed
 }
 sTopicBase=newBase;
 iRet=changeSubscribe(oldBase, newBase);
 //iRet: 0=OK,16=no mqtt-server,32=unsubscribe/64=subscribe error
 if(iRet==0)
 {
  if(DEBUG_MQTT) Serial.println("changeTopicBase(): OK");
  return true;
 }
 if((iRet&32)==32)
 {
  if(DEBUG_MQTT) Serial.println("changeTopicBase(): OK (unsubscribe failed)");
  return true;
 }
 if(DEBUG_MQTT) Serial.println("changeTopicBase(): failed");
 return false;
}

//_____prepare to send a message with topic out of aPayloadPub__
void SimpleMqtt::sendPubIndex(int index, String payload)
{
 if((index<0) || (index>=TOPIC_MAX)) return;
 iPub|=(1<<index);
 aPayloadPub[index]=payload;
}

//_____force (simulate) a get-, set-, sub- or pub-message_______
bool SimpleMqtt::simpleMqttDo(String type, String topic, String payload)
{
 //-----get message---------------------------------------------
 if(type=="get") {
  for(int i=0; i<numTopicGet; i++) {
   if(topic==aTopicGet[i]) {
    iGet|=(1<<i);
    return true;
   }
  }
  return false;
 }
 //-----set message---------------------------------------------
 if(type=="set") {
  for(int i=0; i<numTopicSet; i++) {
   if(topic==aTopicSet[i]) {
    aPayloadSet[i]=payload;
    iSet|=(1<<i);
    return true;
   }
  }
  return false;
 }
 //-----sub message---------------------------------------------
 if(type=="sub") {
  for(int i=0; i<numTopicSub; i++) {
   if(topic==aTopicSub[i]) {
    aPayloadSub[i]=payload;
    iSub|=(1<<i);
    return true;
   }
  }
  return false;
 }
 //-----pub message---------------------------------------------
 if(type=="pub") {
  for(int i=0; i<numTopicPub; i++) {
   if(topic==aTopicPub[i]) {
    aPayloadPub[i]=payload;
    iPub|=(1<<i);
    return true;
   }
  }
  return false;
 }
 return false;
}

//**************************************************************
//     connection state
//**************************************************************

//_____is error bit set?________________________________________
bool SimpleMqtt::isConnectError() {
 bool bConErr=(conState&BIT_CONN_ERROR)>0 ? true : false;
 return bConErr;
}

//_____clear connection error bit_______________________________
void SimpleMqtt::resetConnectError() {conState&=(~BIT_CONN_ERROR);}

//_____convert conState to unsigned long number_________________
unsigned long SimpleMqtt::getConState() {return conState;}

//_____convert conState to HEX string___________________________
String SimpleMqtt::getConStateHex() {
 unsigned long ul1=conState; 
 int i=sizeof(unsigned long);
 char buffer[i+1];
 buffer[i]=0;
 while(i>0)
 {
  i--;
  uint8_t ch=(ul1 & 0x0F) + '0';
  if(ch>'9') ch+=7;
  buffer[i]=ch;
  ul1>>=4;
 }
 String s1=String(buffer);
 return s1;
}

//**************************************************************
//    helper methods
//**************************************************************

//_____subscribe get-, set-, sub-topic__________________________
// return: true=OK, false on error
bool SimpleMqtt::subscribeAllTopics(){
 return subscribeAllTopics(sTopicBase); 
}

//_____subscribe get-, set-, sub-topic for given topic__________
// return: true=OK, false on error
bool SimpleMqtt::subscribeAllTopics(String topicBasis)
{
 bool ret=true;
 char ca[7+topicBasis.length()];
 if(!(PubSubClient::connected())) return false;
 sprintf(ca,"%s/get",topicBasis.c_str());// "get" topic
 ret&=subscribe(ca);
 sprintf(ca,"%s/set/#",topicBasis.c_str());// "get" topic
 ret&=subscribe(ca);
 for(int i=0; i<numTopicSub; i++)
  ret&=subscribe(aTopicSub[i].c_str());
 return ret;
}

//_____unsubscribe get-, set-, sub-topic________________________
// return: true=OK, false on error
bool SimpleMqtt::unsubscribeAllTopics()
{ return unsubscribeAllTopics(sTopicBase); }

//_____unsubscribe get-, set-, sub-topic________________________
// return: true=OK, false on error
bool SimpleMqtt::unsubscribeAllTopics(String topicBasis)
{
 bool ret=true;
 int len=topicBasis.length();
 char ca[7+len];
 if(!(PubSubClient::connected())) return false;
 sprintf(ca,"%s/get",topicBasis.c_str());   // "get" topic
 //ca[4+len]=0;
 ret&=unsubscribe(ca);
 sprintf(ca,"%s/set/#",topicBasis.c_str()); // "get" topic
 //ca[6+len]=0;
 ret&=unsubscribe(ca);
 for(int i=0; i<numTopicSub; i++)
  ret&=unsubscribe(aTopicSub[i].c_str());
 return ret;
}

//_____unsubscribe old, subscribe new topic base________________
// return: 0=OK, 16=no mqtt server, 32=unsubscribe error,
//         64=subscribe error
int SimpleMqtt::changeSubscribe(String oldTopic, String newTopic)
{
 int ret=0;                                 // no error
 if(DEBUG_MQTT) Serial.print("changeSubscribe(): ");
 //-----sTopicBase is new, try to update MQTT-server------------
 if(PubSubClient::connected())
 {
   
  //------unsubscribe old topics--------------------------------
  if(!unsubscribeAllTopics(oldTopic)) ret|=32;// unsubscribe error
  //-----subscribe new topics-----------------------------------
  if(!subscribeAllTopics(newTopic)) 
  {
   if(DEBUG_MQTT) Serial.printf("Could not subscribe Topics. ");
   ret|=64; // subscribe error
   if(STARTINFO_ALLOW)
    publish(STARTINFO_TOPIC, sTopicBase.c_str());//start message
  }
 }
 else
 {
  ret=16;
  if(DEBUG_MQTT) Serial.printf("failed! MQTT broker not connected. ");
 }
 if(DEBUG_MQTT) {if(ret>0) Serial.printf("ret=%d\n",ret); else Serial.println("OK");}
 return ret;                                // 
}

//_____send answers to get-/set-requests________________________
void SimpleMqtt::sendRet()
{
 //-----send (ret) answers for get request----------------------
 for(int i=0; i<numTopicGet; i++)
 {
  if((iRet&(1<<i))>0) 
  {
   String t1=sTopicBase;
   t1+="/ret/";
   t1+=aTopicGet[i];
   if(publish(t1.c_str(),aPayloadRet[i].c_str()))
    iRet&=(~(1<<i));
  }
 }
 //-----send (retset) answers for set requests------------------
 for(int i=0; i<numTopicSet; i++)
 {
  if((iRetSet&(1<<i))>0) 
  {
   String t1=sTopicBase;
   t1+="/ret/";
   t1+=aTopicSet[i];
   if(publish(t1.c_str(),aPayloadSet[i].c_str()))
   {
    iRetSet&=(~(1<<i));
    iSet&=(~(1<<i));
   }
  }
 }
 //-----send publish message------------------------------------
 for(int i=0; i<numTopicPub; i++)
 {
  if((iPub&(1<<i))>0) 
  {
   if(publish(aTopicPub[i].c_str(),aPayloadPub[i].c_str()))
   {
    iPub&=(~(1<<i));
   }
  }
 }
 //-----delete all requests anyway------------------------------
 iRet=0; iRetSet=0; iPub=0;
}

//_____convert Array to json format_____________________________
String SimpleMqtt::jsonArray(String what)
{
 int num=0;
 String s1="{";
 if(what=="get")
 { 
  num=numTopicGet;
  s1+="\"num\":";
  s1+=String(num);
  s1+=",\"val\":{";
  for(int i=0; i<num; i++)
  {
   if(i>0) s1+=",";
   s1+="\""+aTopicGet[i]+"\"";
  }
 }
 s1+="}}";
 return s1;
}

//_____split string to array 1__________________________________
int SimpleMqtt::splitString(String str, String aStr[])
{
 return splitString(str, aStr, ",", TOPIC_MAX);
}

//_____split string to array 2__________________________________
int SimpleMqtt::splitString(String str, String aStr[], String delimiter)
{
 return splitString(str, aStr, delimiter, TOPIC_MAX);
}

//_____split string to array 3___________________________________
// return: array aStr and number of elements in array aStr
int SimpleMqtt::splitString(String str, String aStr[], 
 String delimiter, int imax)
{
 int anz=0;
 int len1=delimiter.length();
 if(len1<1) return anz; 
 if(str.length()<1) return anz;
 int pos1=0,pos2=-1;
 while((pos2=str.indexOf(delimiter,pos1))>=0)
 {
  aStr[anz++]=str.substring(pos1,pos2);
  if(anz>=imax) return anz;
  pos1=pos2+len1;
 }
 aStr[anz++]=str.substring(pos1);
 return anz;
}

//**************************************************************
//     internal methods
//**************************************************************

//_____generate get answers in array aPayloadRet[]______________
// uses  : iGet, iRet, aPayloadRet[], numTopicXXX, aTopicXXX[]
//         XXX = Get, Set, Sub, Pub
// calls : external function doGetAnswer()
// result: answers in array aPayloadRet[]
void SimpleMqtt::createGetAnswer()
{
 if(iGet>0)
 {
  for(int i=0; i<numTopicGet; i++)
  {
   if((iGet&(1<<i))>0)
   {
    //-----"automatic" get answers-----------------------------
    if(aTopicGet[i]=="help") {
     String p1="\r\nget: ";                 // list all get
     for(int i=0; i<numTopicGet; i++) p1+=aTopicGet[i]+"|";
     p1+="\r\nset: ";                       // list all set
     for(int i=0; i<numTopicSet; i++) p1+=aTopicSet[i]+"|";
     p1+="\r\nsub: ";                        // list all sub
     for(int i=0; i<numTopicSub; i++) p1+=aTopicSub[i]+"|";
     p1+="\r\npub: ";                        // list all pub
     for(int i=0; i<numTopicPub; i++) p1+=aTopicPub[i]+"|";
     p1+="\r\n";
     aPayloadRet[i]=p1; 
     iRet|=(1<<i);
    }
    if(aTopicGet[i]=="version") {
     String p1="SimpleMqtt Version ";
     p1+=SIMPLEMQTT_VERSION;
     aPayloadRet[i]=p1;
     iRet|=(1<<i);
    }
    if(aTopicGet[i]=="ip") {
     aPayloadRet[i]=getsMyIP(); 
     iRet|=(1<<i);
    }
    //-----user answers [from global function doGetAnswer()]----
    String s1=simpleGet(aTopicGet[i]);
    if(s1.length()>0)
    {
     if(aTopicGet[i]=="help" && s1.charAt(0)=='+')
      s1=aPayloadRet[i]+s1.substring(1);    // append to help
     aPayloadRet[i]=s1;
     iRet|=(1<<i);
    }
   }
  }
  iGet=0;
 }
}

//**************************************************************
//     methods for eeprom read/write
//**************************************************************

//_____write topic to eeprom as topicBase_______________________
// Value in EEPROM: MQddtopicBase0UU0 dd=length of topicBase,
//                  0=VALUE 0 (not char!)
// return: length of topic (or -1 on error)
int SimpleMqtt::eepromWriteTopicBase(String topic)
{
 int ret=-1;
 if(!USE_EEPROM) return -1;
 //-----------fits topic into eeprom?---------------------------
 int len=topic.length();
 if(len>TOPIC_MAXLEN) return ret;
 if(len>(EEPROM_SIZE-8)) return ret;
 //-----------length OK, write topic + check values-------------
 char ca[len+8];
 ca[0]='M';
 ca[1]='Q';
 ca[2]=(len/10)+'0';
 ca[3]=(len-10*(len/10))+'0';
 ca[4]=0;
 strcat(ca,topic.c_str());
 ca[len+4]=0;                          // value 0
 ca[len+5]='U';                        // U=0x55=0101 0101
 ca[len+6]='U';                        // U=0x55=0101 0101 
 ca[len+7]=0;                          // value 0
//sprintf(ca,"MQ%02d%s\000UU\000",len,topic.c_str());//does not work
 ret=eepromWriteBlock(ca,0,len+8);     // write to EEPROM
 //if(DEBUG_MQTT) Serial.println("eepromWriteTopicBase(): ca="+String(ca)+", ret="+String(ret));
 if(ret!=(len+8)) ret=-1;              // write error
 return len;                           // OK: topic length
}

//_____read topic from eeprom or use default value______________
// Value in EEPROM: MQddtopicBase0UU0 dd=length of topicBase,
//                  0=VALUE 0 (not char!)
// return: read TopicBase ("" on error)
//         iResult >=0: OK (length),
//         -1 EEPROM not allowed, -2 wrong begin, 
//         -3 wrong length, -4 read error, -5 wrong end
String SimpleMqtt::eepromReadTopicBase(int& iResult)
{
 String s1="";
 if(USE_EEPROM) {
  char c4[5];
  int len4=eepromReadBlock(c4,0,4);
  if(len4==4)
  {//---------wrong length of begin sequence--------------------
   if((c4[0]=='M') && (c4[1]=='Q'))
   {//---------EEPROM starts with MQ-----------------------------
    int len1=10*(c4[2]-'0')+(c4[3]-'0');
    if(len1<=(EEPROM_SIZE-4))
    {//--------stored topic length fits into eeprom--------------
     char ca[len1+4];
     int len2=eepromReadBlock(ca,4,len1+4);
     if(len2==(len1+4))
     {//-------correct number of chars read----------------------
      if(ca[len1]==0 && ca[len1+1]=='U' && ca[len1+2]=='U' && ca[len1+3]==0)
      {//------tail of topic OK----------------------------------
       s1=String(ca);
       iResult=0;
      }
      else { iResult=-5; }
     }
     else { iResult=-4; }
    }
    else { iResult=-3; }
   }
   else { iResult=-2; }
  }
  else { iResult=-2; }
 }
 else { iResult=-1; }
 //-----------show debug info-----------------------------------
 if(DEBUG_MQTT) {
  if(iResult==0) Serial.println("eepromReadTopicBase(): "+s1+" - OK");
  else Serial.printf("eepromReadTopicBase(): failed! iResult=%d\n",iResult);
 }
 return s1;
}

//_____read topic from eeprom or use default value______________
// Value in EEPROM: MQddtopicBase0UU0 dd=length of topicBase,
//                  0=VALUE 0 (not char!)
// return: TopicBase (or "" on error)
String SimpleMqtt::eepromReadTopicBase()
{
 int iResult;
 String s1=eepromReadTopicBase(iResult);
 return s1;
}

//_____erase identifier of topicBase____________________________
// return true: erase ok, false: error occurred
bool SimpleMqtt::eepromEraseTopicBase()
{
 if(!USE_EEPROM) return false;
 char ca[5]={0,0,0,0,0};
 int ret=eepromWriteBlock(ca,0,5);
 if(ret==5) {
  if(DEBUG_MQTT) Serial.println("eepromEraseTopicBase() OK"); 
  int iResult;
  String s1=eepromReadTopicBase(iResult);
  if(iResult!=0) return true;
 }
 if(DEBUG_MQTT) Serial.println("eepromEraseTopicBase() Error"); 
 return false;
}

//_____read a block from eeprom_________________________________
// uses: EEPROM_SIZE
// return: number of read bytes or -1|-2|-3|-4 on error
size_t SimpleMqtt::eepromReadBlock(char* data, 
       unsigned long address, unsigned long len)
{
 unsigned long i;
 //-----check input---------------------------------------------
 if(len==0)    return -1;
 //if(len<0)     return -2;
 //if(address<0) return -3;
   if((address+len)>eepromSize_) len=eepromSize_-address;
 //-----read bytes----------------------------------------------
 if(DEBUG_MQTT) Serial.print("eepromReadBlock(): ");
 eeprom_->begin(eepromSize_);
 for(i=0; i<len; i++) {
  data[i]=(char)(eeprom_->read(address+i));
  if(DEBUG_MQTT) Serial.printf("%ld=%c=%d|",i,data[i],data[i]);
 }
 eeprom_->end();
 if(DEBUG_MQTT) Serial.printf("=> %ld bytes read.\n",len);
 return len;
}

//_____write a block to eeprom__________________________________
// uses: EEPROM_SIZE
// return: number of written bytes or -1|-2|-3|-4 on error
size_t SimpleMqtt::eepromWriteBlock(char* data, 
       unsigned long address, unsigned long len)
{
 unsigned long i=0, numBytes=0;
 uint8_t b1, b2;
 //-----check input---------------------------------------------
 if(len==0)    return -1;
 //if(len<0)     return -2;
 //if(address<0) return -3;
 //if((address+len)>EEPROM_SIZE) len=EEPROM_SIZE-address; // or return -4
 if((address+len)>eepromSize_) len=eepromSize_-address;
 
 //-----write bytes---------------------------------------------
 if(DEBUG_MQTT) Serial.print("eepromWriteBlock(): ");
 //if(eepromBegin_<3) eepromBegin();
 
 eeprom_->begin(eepromSize_);
 //.....write all bytes.........................................
 for(i=0; i<len; i++)
 {
  b1=data[i];
  eeprom_->write(address+i, b1);
  eeprom_->commit();
  delay(1);
  b2=eeprom_->read(address+i);
  if(b1==b2) numBytes++;
  if(DEBUG_MQTT) {Serial.printf("%ld=%c|",i,b2); }
 }
 eeprom_->end();
 if(len==numBytes)
 {
  if(DEBUG_MQTT) Serial.println(" ok");
  return numBytes;
 }
 if(DEBUG_MQTT) Serial.println(" ERROR");
 return -2;
}

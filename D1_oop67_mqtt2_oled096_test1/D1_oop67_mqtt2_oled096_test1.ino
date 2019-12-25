//_____D1_oop67_mqtt2_oled096_test1.ino_______191222-191225_____
// Connect to a WiFi and MQTT server and show connecting results 
// on a 0.96" OLED.
// When connected, a counter value is sent to the broker
// (topic `oled096_test1`) and received values are shown on OLED.

// * Show MAC address (line 7, inverted)
// * Try to connect to the given WLAN (ssid, password).
//   Plot dots on OLED (line 3) while waiting
// * Show name of WLAN and IP when connection is established
//   (line 2 and line 3)
// * Show state of MQTT connection (line 4)
// * Show counter value published (sent) to MQTT server 
//   (topic TOPIC_S_TEST1, line 5)
// * Show counter value got from MQTT server 
//   (same topic TOPIC_S_TEST1, line 6)
// Hardware: (1) WeMos D1 mini
//           (2) one color OLED 0.96" mounted on a proto shield 
//               (128x64 pixel, I2C-address 0x3C)       
// Important: Example needs a broker!
// Created by Karl Hartinger, December 25, 2019.
// Last modified: -
// Released into the public domain.
#define  DEBUG67        true  //false
#define  VERSION67      "2019-12-25 D1_oop67_mqtt_oled096_test1"
#define  MQTT_NAME      "D1_oop67_test1"

#include "src/mqtt2/D1_class_MqttClientKH2.h"
#include "src/screen096/D1_class_Screen096.h"
Screen096 display_;
long counterIn=0;

//-----MQTT-----------------------------------------------------
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi10", "12345678","10.1.1.1");
//......S=subscribe: supported mqtt receive topics..............
#define TOPIC_S_TEST1             ("oled096_test1")
//.....P=publish: supported mqtt send topics....................
#define TOPIC_P_TEST1             ("oled096_test1")
//.....number of actions........................................
#define NR_NOTHING            0        // # mqtt do nothing
#define NR_P_COUNTER          1        // # mqtt send counter
#define NR_S_COUNTER          1        // # mqtt received counter
//......"container" for all send and receive actions............
int     mqttSub=NR_NOTHING;            // 0=nothing
int     mqttPub=NR_NOTHING;            // 0=nothing

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int len_)
{
 //-----convert topic and payload to String (if necessary)------
 char cPayload[len_+1];                // char-array for payload 
 strncpy(cPayload,(char*)payload,len_);// copy payload bytes
 cPayload[len_]=0;                     // set end char
 // String sTopic=String(topic);
 // String sPayload=String(cPayload);
 if(DEBUG67)
 { Serial.print("Message topic '"); Serial.print(topic);
   Serial.print("': "); Serial.println(cPayload);
 }
 //-----is topic meant for me?----------------------------------
 if(strcmp(topic,TOPIC_S_TEST1)==0)
 {
  mqttSub|=NR_S_COUNTER;               // counter received
  counterIn=atol(cPayload);            // convert to int
 }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 {Serial.begin(115200); Serial.println("\n"+String(VERSION67));}
 //-----init 0.96" OLED display---------------------------------
 display_.begin();
 display_.useCP437(); // after line display_.begin();
 //-----write text to display-----------------------------------
 display_.screen16Clear(0,"? dBm",'c');
 String s1="MAC ";
 s1+=client.getsMac();
 display_.screen16(-7,s1);
 //-----setup mqtt and trigger toggle message-------------------
 client.addSubscribe(String(TOPIC_S_TEST1));
 client.addPublish(String(TOPIC_P_TEST1));
 client.setMQTTClientName(MQTT_NAME);
 client.setCallback(callback);
}

long loopCounter=0;

//_____main loop________________________________________________
void loop()
{
 String s1;
 // ***** do at the beginning of every loop ********************
 s1=client.getsState();                // get MQTT state
 display_.screen16(4, s1);             // display MQTT state

 // ***** handle WiFi ******************************************
 if(!client.isWiFiConnected()) 
 {//=====MQTT client is NOT connected to WiFi (WLAN)============
  //-----show title and waiting dots in line 3 of 0.96" OLED----
  display_.screen16(0,"? dBm",'c');    // 0 = line 1 with border
  display_.screen16(2,"WLAN connecting");
  display_.screen16Dot(3);             // show waiting dots
 }
 else
 {//====MQTT client is connected to WiFi (WLAN)=================
  //-----always show signal strength on 0.96" OLED--------------
  s1="Signal ";
  s1+=client.getsSignal();             // wifi signal strength
  s1+=" dBm";                          // unit
  display_.screen16(0,s1,'c');         // display signal strength
  display_.display();
  if(client.isWiFiConnectedNew())
  {//-----new WiFi connection: show properties on 0.96" OLED----
   s1="WLAN ";
   s1+=client.getsSSID();
   display_.screen16(2, s1);           // display WLAN SSID
   s1="MyIP ";
   s1+=client.getsMyIP();              // get IP address
   display_.screen16Dot(3, 0);         // set dotCounter to 0
   display_.screen16(3, s1);           // display MyIP
  }

  // ***** handle MQTT *****************************************
  if(client.isConnected()) //MUST always be called (for receive!)
  {//=====do the things that should be done when connected======
   //-----send counter value every 10th time--------------------
   if(loopCounter%10==0)
   {
    client.publish(TOPIC_P_TEST1, String(loopCounter).c_str());
    s1="Sent "; 
    s1+=String(loopCounter);
    display_.screen16(5, s1);          // display send value
   }
   //-----counter massage received?-----------------------------
   if((mqttSub & NR_S_COUNTER)>0)
   {
    mqttSub &= ~NR_S_COUNTER;
    s1="Rec  "; 
    s1+=String(counterIn);
    display_.screen16(6, s1);          // display received value
   }
  }
 }
 // ***** do at the end of every loop **************************
 loopCounter++; if(loopCounter>99999) loopCounter=0;
 delay(200);
}

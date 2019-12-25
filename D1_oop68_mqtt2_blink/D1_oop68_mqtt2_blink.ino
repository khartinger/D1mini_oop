//_____D1_oop68_mqtt2_blink.ino_______________191225-191225_____
// Simple example to test a MQTT "client-server"-connection.
// * Try to connect to the given WLAN (ssid, password) and 
//   MQTT server (name or ip, default port 1883)
//   If no success: blue led blinks 8x quickly, try again
//---On Success-------------------------------------------------
// * Send a message with topic 'oop68/led' and alternating
//   payload "0" or "1" every 2 seconds.
// * Subscribe the same message and turn blue led 1=on or 0=off
// * If connection lost: blue led blinks 8x quickly
// Hardware: (1) WeMos D1 mini
// Important: Example needs a broker!
// Created by Karl Hartinger, December 25, 2019.
// Last modified: -
// Released into the public domain.

#include "src/mqtt2/D1_class_MqttClientKH2.h"
const int pinBlueLED=2;                     // Pin # D1mini blue LED
bool turnLEDon=true;                        // turn LED on or off

//-----MQTT-----------------------------------------------------
//MqttClientKH2 client("..ssid..", "..password..","mqttservername");
MqttClientKH2 client("Raspi10", "12345678","10.1.1.1");

//......S=subscribe: supported mqtt receive topics..............
#define TOPIC_S_MQTT2_BLINK       ("oop68/led")
//.....P=publish: supported mqtt send topics....................
#define TOPIC_P_MQTT2_BLINK       ("oop68/led")

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int len_)
{
 //-----convert topic and payload to String (if necessary)------
 char cPayload[len_+1];                // char-array for payload 
 strncpy(cPayload,(char*)payload,len_);// copy payload bytes
 cPayload[len_]=0;                     // set end char
 //-----is topic meant for me?----------------------------------
 if(strcmp(topic,TOPIC_S_MQTT2_BLINK)==0)
 {
  Serial.print("In: ");Serial.println(char(payload[0]));
  if(len_==1)                          // only 1 char?
  { if(payload[0]=='1') digitalWrite(pinBlueLED,0);   // LED on
                   else digitalWrite(pinBlueLED,1);   // LED off
  }
 }
}

//_____blink blue LED Quickly___________________________________
void blink()
{
 for(int i=0; i<8; i++) {
  digitalWrite(pinBlueLED,0);          // LED on
  delay(100);
  digitalWrite(pinBlueLED,1);          // LED off
  delay(150);
  }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(115200); Serial.println();
 pinMode(pinBlueLED, OUTPUT);
 digitalWrite(pinBlueLED,1);           // LED off
 //-----setup mqtt and trigger toggle message-------------------
 client.addSubscribe(String(TOPIC_S_MQTT2_BLINK));
 client.addPublish(String(TOPIC_P_MQTT2_BLINK));
 client.setCallback(callback);
 while(!client.connectWiFiMQTT()) blink();
}

//_____main loop________________________________________________
void loop()
{
 // ***** handle MQTT ******************************************
 if(client.isConnected()) //MUST always be called (for receive!)
 {
  if(turnLEDon) client.publish(TOPIC_P_MQTT2_BLINK,"1");
           else client.publish(TOPIC_P_MQTT2_BLINK,"0");
 }
 else blink();
 turnLEDon=!turnLEDon;
 delay(2000);
}
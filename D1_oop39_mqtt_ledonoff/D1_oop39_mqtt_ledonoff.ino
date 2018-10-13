//_____D1_oop39_mqtt_ledonoff.ino_____________181013-181013_____
// Very Simple MQTT-Example (needs a broker!)
// * With topic led and message on or off the blue led on
//   D1 mini board is switched on or off
//
// Hardware: 
// (1) WeMos D1 mini
// Created by Karl Hartinger, October 13, 2018
// Last modified: -
// Released into the public domain.

#include "src/mqtt/D1_class_MqttClientKH.h"

#define TOPIC1             ("led")     //
const int pinLED=D4;                   // or 2
MqttClientKH client("..ssid..", "..password..","mqttservername");

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
//-----convert topic and payload to String, print to Serial-----
 String sTopic=String(topic);
 String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 if(sTopic==TOPIC1)
 {
  if(sPayload=="on")  digitalWrite(pinLED, LOW);
  if(sPayload=="off") digitalWrite(pinLED, HIGH);
 }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 pinMode(pinLED, OUTPUT); 
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(TOPIC1));
 client.setCallback(callback);
 client.reconnect();
}

//_____main loop________________________________________________
void loop() 
{
 client.isConnected();
 delay(100);
}

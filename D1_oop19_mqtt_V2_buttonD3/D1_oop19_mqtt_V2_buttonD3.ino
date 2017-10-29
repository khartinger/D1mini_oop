//_____D1_oop19_mqtt_V2_buttonD3.ino__________170721-171029_____
//
// Simple MQTT-Example (needs a broker!)
// * On Power-On a message "button/02" with payload -1 is sent.
// * When button D3 is pressed, this is printed to Serial and 
//   the value 1 is published under topic "button/02".
// * All messages received by topic "button/02" are printed
//   to Serial.   
//
// Hardware: 
// (1) WeMos D1 mini
// (2) 1-Button shield
#include "D1_class_MqttClientKH.h"
#include "D1_class_Din.h"
Din button_(D3);
MqttClientKH client("..ssid..", "..password..","mqttservername");

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
//-----print message to Serial----------------------------------
 String sTopic=String(topic);
 Serial.print("Message received for topic ");
 Serial.print(sTopic);
 Serial.print(": ");
  String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 Serial.println(sPayload);
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(9600); Serial.println("");
 //-----setup mqtt----------------------------------------------
 client.addSubscribe("button/02/#");
 client.addPublish("button/02", "-1");
 client.setCallback(callback);
 client.reconnect();
}

//_____main loop________________________________________________
void loop() 
{
 if(client.isConnected())
 {
  if(button_.is_falling_edge())
  {
   client.publish("button/02", "1", true);  // true=retain
   Serial.println("\n-----button pressed!-----");
  }
 }
}

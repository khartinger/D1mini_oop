//_____D1_oop19_mqtt_taster1.ino______________170721-170721_____
//
// Simple MQTT-Example (needs a broker!)
// * When button at D3 is pressed the value 1 is published under
//   the topic "button".
// * Messages received by topic "counter" are printed to Serial.
//
#include "D1_class_MqttClientKH.h"
#include "D1_class_Din.h"
Din button_(D3);
MqttClientKH client("..ssid..", "..password..","mqttservername");

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
//-----print message to Serial----------------------------------
 String sTopic=String(topic);
 Serial.print("Message for topic ");
 Serial.print(sTopic);
 Serial.print(": ");
 for (int i = 0; i < length; i++) 
 {
  Serial.print((char)payload[i]);
 }
 Serial.println();
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(9600); Serial.println("");
 //-----setup mqtt----------------------------------------------
 client.addSubscribe("counter");
 client.addPublish("button", "0");
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
   client.publish("button", "1", true);     // true=retain
   Serial.println("button pressed (button=1)");
  }
 }
}

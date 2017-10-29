//_____D1_oop19_mqtt_V2_counter_oled1.ino_____170721-171029_____
//
// Simple MQTT-Example (needs a broker!)
// * When message "button/xx" (xx=number of button) with 
//   payload 1 is received, a counter is incremented, 
//   the value is displayed on oled shield and
//   printed to Serial. (Payload 0 resets the counter.)
// * After that, a message "counter/xx/ok" is published with
//   counter value as payload.
//
#include "D1_class_MqttClientKH.h"
#include "D1_class_DisplayKH.h"
DisplayKH     display_;                // 
MqttClientKH client("..ssid..", "..password..","mqttservername");

unsigned long requestCounter=0;

//_____display counter value on oled____________________________
void display_counter(String sCounter)
{
 display_.screen13(1,sCounter,'c',false);
 display_.screen13(2,"MQTT");
 display_.screen13(3,"counter");
 display_.screen13(4,"171029 KH");
 display_.display();
}

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
 String sTopic=String(topic);
 //-----Convert payload to String, print message to Serial------
 String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 Serial.print("Message received. Topic '"+sTopic+"', ");
 Serial.println("payload='"+sPayload+"'");
 if(sTopic.indexOf("/ok")==9) Serial.println("");
 //-----process message "button/xx"-----------------------------
 String sButtonNr="";
 if(sTopic.startsWith("button"))
 {
  if((sTopic.charAt(6)=='/')&&(sTopic.length()==9))
  {
   if(isDigit(sTopic[7])&&isDigit(sTopic[8]))
   {
    sButtonNr=sTopic.substring(7,9);
    if(payload[0]=='0') requestCounter=0;
    if(payload[0]=='1') requestCounter++;
    String sCounter=String(requestCounter);
    Serial.println("requestCounter = "+sCounter);
    display_counter(sCounter);
    client.publishString("button/"+sButtonNr+"/ok", sCounter, true);
   }
  } 	
 }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(9600); Serial.println("");
 display_counter(String(requestCounter));
 //-----setup mqtt----------------------------------------------
 client.setCallback(callback);
 client.addSubscribe("button/#");
 client.reconnect();
}

//_____main loop________________________________________________
void loop() 
{
 client.isConnected();
 delay(20);
}

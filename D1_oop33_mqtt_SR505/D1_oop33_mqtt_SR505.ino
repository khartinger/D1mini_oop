//_____D1_oop33_mqtt_SR505.ino________________180309-180309_____
// * On Power-On a message "button/90" with payload -1 is sent.
// * When the HC-SR505 Mini PIR Motion Sensor detects a motion,
//   this is printed to Serial and the value 1 is published
//   under topic "button/90".
// * The duration since last motion is published
//   under topic "button/90/t".
// * All messages received by topic "button/90" are printed
//   to Serial.   
//
// Hardware: 
// (1) WeMos D1 mini
// (2) HC-SR505 Mini PIR Motion Sensor (5V, D6, GND)
// Important: Example needs a broker!
#include "D1_class_MqttClientKH.h"
#include "D1_class_Din.h"
#define  BUTTON_NAME              "button/90"
#define  BUTTON_NAME2             "button/90/t"
Din button_(D6);
MqttClientKH client("..ssid..", "..password..","mqttservername");

//_____process all subscribed incoming messages_________________
void callback(char* topic, byte* payload, unsigned int length)
{
//-----convert topic and payload to String, print to Serial-----
 String sTopic=String(topic);
 String sPayload="";
 for (int i=0; i<length; i++) sPayload+=(char)payload[i];
 Serial.print("Message received for topic "+sTopic+"=");
 Serial.println(sPayload);
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(9600); Serial.println("");
 //-----setup mqtt----------------------------------------------
 client.setClientName(String(BUTTON_NAME));
 client.addSubscribe(String(BUTTON_NAME)+"/#");
 client.addPublish(String(BUTTON_NAME),"-1", false);
 client.addPublish(String(BUTTON_NAME2),"-1", false);
 client.setCallback(callback);
 client.reconnect();
}

unsigned long d,h,m,s;
char ct[20];

//_____main loop________________________________________________
void loop() 
{
 if(client.isConnected())
 {
  if(button_.isRisingEdge())
  {
   //-----convert time since last event to day hh:mm:ss---------
   s=button_.getLastDuration()/1000;
   d=s/86400;
   s=s%86400;
   h=s/3600;
   s=s%3600;
   m=s/60;
   s=s%60;
   sprintf(ct,"%d %02d:%02d:%02d",d,h,m,s);
   //-----publish event------------------------------------------
   client.publish(BUTTON_NAME, "1", true);  // true=retain
   client.publish(BUTTON_NAME2, ct, true);  // true=retain
   Serial.println(String(ct)+"\nMotion detected!");
  }
 }
 else
 {
  Serial.println("\nError: Not connected to WLAN!");
  delay(1000);
 }
 if(button_.isFallingEdge())
 {
  String t=String(0.001*button_.getLastDuration(),1);
  Serial.println(t+"sec\nReady for next motion detection...");
 }
}

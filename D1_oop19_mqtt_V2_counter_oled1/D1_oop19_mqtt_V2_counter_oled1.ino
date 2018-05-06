//_____D1_oop19_mqtt_V2_counter_oled1.ino_____170721-180506_____
// (Part of a) simple MQTT-Example (needs a broker!): 
// * Connect to MQTT broker via WLAN
// * When message "button/xx" (xx=number of button) with 
//   payload 1 is received,
//   + counter value is incremented, 
//   + counter value and button number are displayed on oled
//   + counter value printed to Serial (115200Bd)
//   Payload 0 resets the counter
// * After that, a message "counter/xx/ok" is published with
//   counter value as payload.
//
// Hardware: WeMos D1 Mini
//           OLED  Shield (SSD1306, 64x48 pixel, I2C)

#include "src/mqtt/D1_class_MqttClientKH.h"
#include "src/screen1/D1_class_Screen1.h"
Screen1 display_;                 // 

//MqttClientKH client("..ssid..", "..password..","mqttservername");
MqttClientKH client("Raspi10", "12345678","10.1.1.1");

unsigned long requestCounter=0;

//_____display counter value on oled____________________________
void display_counter(String sCounter, String sButton)
{
 display_.screen13(1,sCounter,'c',false);
 display_.screen13(2,"MQTT count");
 display_.screen13(3,"Button "+sButton);
 display_.screen13(4,"180506 KH");
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
    display_counter(sCounter, sButtonNr);
    client.publishString("button/"+sButtonNr+"/ok", sCounter, true);
   }
  } 	
 }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 Serial.begin(115200); Serial.println("");
 display_counter(String(requestCounter), "--");
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

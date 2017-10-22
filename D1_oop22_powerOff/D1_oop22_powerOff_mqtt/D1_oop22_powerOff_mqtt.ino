//_____D1_oop22_powerOff_mqtt.ino_____________171012-171022_____
// MQTT-Client (needs a broker!)
// When SET-button S1 is pressed, the D1 mini starts and the LED
// lights red. The D1 mini tries to connect to a mqtt-broker.
// On success the led shines orange and the D1 mini
//   + publishes the value 1 under topic "button/01"
//   + publishes the voltage under topic "button/01/volt"
// Then D1 mini waits for a message `/button/01/ok`.
// If the message ok=1, D1 mini sets the LED to green, waits
// for 2 seconds and turns off the power (D6=1).   
// If there is no connection to the WLAN or no answer ok,
// power is turned off after 20 secs anyway (to save energy :).
// Hardware: 
// (1) WeMos D1 mini
// (2) D1_OnOff shield
// Important: supply voltage by D1_OnOff shield! (NOT by USB)

#include "D1_class_MqttClientKH.h"
#include "D1_class_Dout.h"
#define MILLIS_TIME2RUN           20000
#define PUBLISH_TOPIC1            "button/01"
#define PUBLISH_TOPIC2            "button/01/volt"

// ***** change this values to your own WLAN data!!!************
MqttClientKH client("..ssid..","..password..","mqttservername");

Dout   led_green(D7);             // green led @ D1_OnOff shield
Dout   led_red(D8);               // red led @ D1_OnOff shield
Dout   pin_relay(D6);             // D6=1: reset relay (off)
const  int pinAin=A0;             // analog input pin
//-----convert 10-bit-ad-value to voltage-----------------------
const  float ain_x2=1023, ain_y2=5.195, ain_x1=0, ain_y1=0;
const  float ain_k=(ain_y2-ain_y1)/(ain_x2-ain_x1);
const  float ain_d=(ain_x2*ain_y1-ain_x1*ain_y2)/(ain_x2-ain_x1);
//-----control program state------------------------------------
bool   published=false;           // messages published?
bool   answer_ok=false;           // answer from broker ok?
long   millis_end;                // time out - do power off

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
 //-----analyze received message--------------------------------
 if(sTopic==String(PUBLISH_TOPIC1)+"/ok")
 {
  if(sPayload=="1") answer_ok=true;
 }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 //-----init output pins----------------------------------------
 pin_relay.off();                 // let relay be set (external)
 led_green.off();                 // +
 led_red.on();                    // + led = red 
 //-----init Serial---------------------------------------------
 Serial.begin(9600); Serial.println("");
 //-----define end time-----------------------------------------
 millis_end=millis()+MILLIS_TIME2RUN;
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(PUBLISH_TOPIC1)+"/ok");
 client.addPublish(PUBLISH_TOPIC1, "0");    // start values
 client.addPublish(PUBLISH_TOPIC2, "-1");   // start values
 client.setCallback(callback);
 client.reconnect();
}

//_____main loop________________________________________________
void loop() 
{
 //-----when connected, publish values--------------------------
 if(client.isConnected())
 {
  if(!published)
  {
   led_green.on();                // led=orange
   led_red.on();                  // led=orange
   //-----measure voltage, publish messages---------------------
   int    ain1023=analogRead(pinAin);
   float  voltage=ain_k*ain1023 + ain_d;
   String sVoltage=String(voltage,3);
   client.publish(PUBLISH_TOPIC1, "1", true);
   client.publishString(String(PUBLISH_TOPIC2), sVoltage, true);
   Serial.println("Messages sent, Voltage: "+sVoltage); 
   published=true;                // messages sent
   delay(100);                    // show sent on led
  }
 }
 else //.....not connected -> led=red...........................
 {
  led_green.off();                // led=red
  led_red.on();                   // led=red
 }
 //-----answer from brooker?------------------------------------
 if(answer_ok)
 {
  led_green.on();                 // led=green
  led_red.off();                  // led=green
  delay(2000);                    // show success on led
  pin_relay.on();                 // reset relay (power off)
  delay(2000);                    // just for safety reasons
  answer_ok=false;                // e.g. wrong power connection
 }
 //-----timeout?------------------------------------------------
 if(millis()>millis_end)
 { 
  pin_relay.on();                 // reset relay (power off)
  led_green.off();                // led=off
  led_red.off();                  // led=off
  delay(2000);                    // just for safety reasons
 }
}

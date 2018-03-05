//_____D1_oop19_mqtt_V3_powerOff.ino__________171012-180305_____
// MQTT-Client (needs a broker!)
// When SET-button S1 is pressed, the D1 mini starts and the LED
// lights red. The D1 mini tries to connect to a mqtt-broker.
// On success the led shines orange and the D1 mini
//   + publishes the value 1 under topic "button/01"
//   + publishes the voltage under topic "button/01/volt"
//   + publishes the state of D3 under topic "button/01/D3"
// Then D1 mini waits for a message `/button/01/ok`.
// If the message ok=1, D1 mini sets the LED to green, waits
// for 1 second and turns off the power (D6=1).   
// If there is no connection to the WLAN or no answer ok,
// power is turned off after 20 secs anyway (to save energy :).
// Hardware: 
// (1) WeMos D1 mini
// (2) D1_OnOff shield
// Important: supply voltage by D1_OnOff shield! (NOT by USB)

#include "D1_class_MqttClientKH.h"
#include "D1_class_Dout.h"
#include "D1_class_Din.h"
#include "D1_class_Ain.h"
#define MILLIS_TIME2RUN           20000
#define PUBLISH_TOPIC1            "button/01"
#define PUBLISH_TOPIC2            "button/01/volt"
#define PUBLISH_TOPIC3            "button/01/Din"

// ***** change this values to your own WLAN data!!!************
MqttClientKH client("..ssid..","..password..","mqttservername");

Dout   led_green(D7);             // green led @ D1_OnOff shield
Dout   led_red(D8);               // red led @ D1_OnOff shield
Dout   pin_relay(D6);             // D6=1: reset relay (off)
Din    din(D5);                   // digital input
Ain    ain;                       // analog input pin
//-----control program state------------------------------------
bool   published=false;           // messages published?
bool   answer_ok=false;           // answer from broker ok?
long   millis_end;                // time out - do power off
//-----some more helpers----------------------------------------
long   counter;                   //received counter value if ok

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
  //.....check, if payload is just an int>=0....................
  int len1=sPayload.length();
  int i=0;
  while(isDigit(sPayload.charAt(i)) && (i<len1)) i++;
  if(i!=len1) return;             // not a digit -> not valid
  counter=sPayload.toInt();
  if(counter>=0) answer_ok=true;
 }
}

//_____setup Serial, WLAN and MQTT clients______________________
void setup() 
{
 //-----init output/output pins---------------------------------
 pin_relay.off();                 // let relay be set (external)
 led_green.off();                 // +
 led_red.on();                    // + led = red
 ain.setDecimals(3);                        // 3 decimal places
 ain.setRefPoints(0, 0.0, 1023, 5.195);     // new ref values
 //pinMode(pinDin, INPUT);                    // input
 //-----init Serial---------------------------------------------
 Serial.begin(9600); Serial.println("");
 //-----define end time-----------------------------------------
 millis_end=millis()+MILLIS_TIME2RUN;
 //-----setup mqtt----------------------------------------------
 client.addSubscribe(String(PUBLISH_TOPIC1)+"/ok");
 //client.addPublish(PUBLISH_TOPIC1, "-1");   // start values
 //client.addPublish(PUBLISH_TOPIC2, "-1");   // start values
 //client.addPublish(PUBLISH_TOPIC3, "-1");   // start values
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
   //-----measure voltage, get din------------------------------
   String sVoltage=ain.getsVoltage();
   String sDin=String(din.read());
   //-----publish messages--------------------------------------
   client.publish(PUBLISH_TOPIC1, "1", true);
   client.publishString(String(PUBLISH_TOPIC2), sVoltage, true);
   client.publishString(String(PUBLISH_TOPIC3), sDin, true);
   Serial.print("Messages sent, Voltage: "+sVoltage);
   Serial.println(", Din: "+sDin);
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
  Serial.println("Counter value: "+String(counter)); 
  led_green.on();                 // led=green
  led_red.off();                  // led=green
  answer_ok=false;                // e.g. wrong power connection
  delay(1000);                    // show success with led
  pin_relay.on();                 // reset relay (power off)
  led_green.off();                // led=off
  led_red.off();                  // led=off
  while(1) ESP.deepSleep(60 * 1000000L);
  delay(2000);                    // just for safety reasons
 }
 //-----timeout?------------------------------------------------
 if(millis()>millis_end)
 { 
  pin_relay.on();                 // reset relay (power off)
  led_green.off();                // led=off
  led_red.off();                  // led=off
  while(1) ESP.deepSleep(60 * 1000000L);
  delay(2000);                    // just for safety reasons
 }
}

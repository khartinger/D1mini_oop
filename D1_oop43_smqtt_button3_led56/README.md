# D1 mini: MQTT pushbutton with feed back
Sketch: D1_oop43_mqtt_button1_led56.ino, Version 2021-01-09   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

Pressing the button on D3 sends an MQTT request.   
When the program starts, the Duo LED lights up red and the D1 mini tries to connect to the MQTT broker. If this succeeds, the LED lights up green.   
If the button D3 is pressed,    
* the D1 mini publishes the topic `relay/1/set/relay` with the payload `toggle`,   
* the blue LED lights up continuously.   

Then the D1 mini waits for the topic `relay1/ret/lamp` with the payload 0 or 1. The result is indicated by the blue LED:   
* If there is no response, the blue LED remains continuously lit.   
* If the answer is 1, the blue LED flashes with long pulses (9:1).   
* If the response is 0, the blue LED flashes with short pulses (1:9).   
* For all other answers (payloads), the blue LED flashes quickly.
* After 10 seconds the blue LED is switched off.   

__*Important*__   
The example needs a MQTT broker and a MQTT relay that sends back a response, e.g. D1 mini with program `D1_oop77_smqtt_stm_relay1`.   

## Hardware 
1. WeMos D1 mini
2. Protoboard with pushbutton between D3 and GND, and a 2-pin Duo_LED between D5 and D6 (and a 100&#8486; or 220&#8486; resistor). Possibly 47k&#8486; resistor from D3 to 3V3.   

![D1 button duo-led](./images/D1_oop43_button3_led56.png "D1mini with button and duo led")   
_Figure 1: D1 mini with button and duo led_ 

![D1 circuit button duo-led](./images/D1_oop43_button3_led56_circuit.png "circuit D1mini with button and duo led")   
_Figure 2: Circuit D1mini with button and duo led_ 

## Software
The SimpleMqtt class extends the PubSubClient class for easy use of MQTT. All commands of the PubSubClient class can be used.   
If the library PubSubClient is installed in the Arduino IDE, the files `PubSubClient.*` in the directory `/src/simplemqtt` can be deleted.

### Creating an MQTT application
#### Setup of a broker  
E.g. install mosquitto on a Raspberry Pi and start the program     
`sudo apt-get install mosquitto`   
`sudo apt-get install mosquitto-clients`   
`sudo /etc/init.d/mosquitto start`.   

#### Create a client on the D1 mini
1. include class SimpleMqtt   
  `#include "src/simplemqtt/D1_class_SimpleMqtt.h"`   

2. create a SimpleMqtt object   
  `SimpleMqtt client("..ssid..", "..password..", "mqtt server name");`  
__*Don't forget: Adapt data to your own network!*__   

3. define a callback function to process the logged topics   
This function will be called for all logged in topics.   
```
//_______MQTT: inspect all subscribed incoming messages_________
void callback(char* topic, byte* payload, unsigned int length)
{
 client.callback_(topic, payload, length); // must be called!
}
```   

4. define functions for get, set and sub functionality
```
// sPayload: payload to message from TOPIC_GET
// auto answer: for help (+), version, ip (can be overwritten)
// return: ret answer payload for get request
String simpleGet(String sPayload)
{
 //-------------------------------------------------------------
 if(sPayload=="version") return String(VERSION43);
 //-------------------------------------------------------------
 //...ToDo...
 //-------------------------------------------------------------
 return String(""); // wrong Get command
}

//_______execute set requests___________________________________
// sTopic from TOPIC_SET, sPayload: payload to topic
// return: ret answer payload for set command
String simpleSet(String sTopic, String sPayload)
{
 //-------------------------------------------------------------
 return String(""); // wrong set command
}

//_______execute sub requests___________________________________
// sTopic from TOPIC_SUB, sPayload: payload to topic
// return: no automatic answer
void simpleSub(String sTopic, String sPayload)
{
}
```   

5. prepare the use of MQTT in the function setup()   

```
 //------setup mqtt---------------------------------------------
 client.setLanguage('e'); // e=english, d=german
 client.setCallback(callback); // call back routine
 client.setTopicBaseDefault(TOPIC_BASE);
 client.setTopics(TOPIC_GET,TOPIC_SET,TOPIC_SUB,TOPIC_PUB);
 client.begin(); // read eeprom...
```
   Possibly also connect to the MQTT broker: ` while(!client.connectWiFiMQTT());`
  
6. in the loop() function  
Call `client.doLoop()` to execute the cyclic polling of the WLAN and MQTT (including automatic reconnection if the WLAN connection was interrupted).   
```
 //======(3) do mqtt messages (send, receive)===================
 client.doLoop(); // mqtt loop
```

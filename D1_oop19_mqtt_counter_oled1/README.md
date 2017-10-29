# Di mini: Simple MQTT example using class MqttClientKH

The class MqttClientKH extends the class PubSubClient for an easy use of mqtt. You can use all commands from class PubSubClient as well.
When library PubSubClient is installed in arduino ide, delete in directory `/libs` the files `PubSubClient.cpp` and `PubSubClient.h` and change `#include "libs/PubSubClient.h"` to `#include <PubSubClient.h>`.

This sketch connects to a MQTT server, then:
* When message "button" with payload 1 is received, a counter is incremented, the value is displayed on oled shield and printed to Serial. (Payload 0 resets the counter.)
* After that, a message "counter" is published with the counter value.
## Hardware
* WeMos D1 mini
* OLED Shield: SSD1306, 64x48 pixel, I2C   
![D1mini oled mqtt counter](./images/D1_OLED_mqtt1.png "D1mini with oled showing mqtt counter")
---

## How to make a D1mini mqtt application
1. Setup a broker  
E.g. install mosquitto on Raspberry Pi and start it  
`sudo apt-get install mosquitto`  
`sudo apt-get install mosquitto-clients`  
`sudo /etc/init.d/mosquitto start`  

2. Make a D1 client
* Include the class MqttClientKH  
  `#include "D1_class_MqttClientKH.h"`  
* Define a MqttClientKH object  
  `MqttClientKH client("..ssid..", "..password..","mqtt server name");`  
__*Don't forget to change WiFi data to your network values!*__
* Define a callback function to process subscribed messages (is called by every subscribed topic)  
  `void callback(char* topic, byte* payload, unsigned int length)`  
* in function setup() do mqtt setup:  
  `client.addSubscribe("topic");` subscribe topic (=input). Handle it in  callback()!  
  `client.addPublish("topic", "startvalue");` publish massage by given topic  
  `client.setCallback(callback);` set (name of) callback function  
  `client.reconnect();` connect to WLAN and mqtt server  
* in function loop()  
  call `client.isConnected()` to process cyclic check for WLAN and mqtt (automatic reconnect, if connection is lost)  

## Class diagram
| class MqttClientKH (extends PubSubClient) |     |
| ----------------------------------------- | --- |
| + __MqttClientKH__(char* ssid, char* pwd, char* mqtt_server, int port) |     |
| + int  getNumSub()    |     |
| + int  getNumPub()    |     |
| + void clrSubscribe() | sets numSub_ to 0 |
| + void clrPublish()   | sets numPub_ to 0 |

| --- *methods to setup WLAN and mqtt connection* --- |     |
| --------------------------------------------------- | --- |
| + bool setup_wifi()  |     |
| + bool reconnect()   |     |
| + bool __isConnected()__ |     |

| --- *methods to define mqtt topics* --- |     |
| --------------------------------------- | --- |
| + bool __addSubscribe__(String topic) |     |
| + bool __delSubscribe__(String topic) |     |
| + bool __addPublish__(String topic, String payload) |     |
| + void __publishString__(String topic, String payload) |     |
| + void __publishString__(String topic, String payload, bool retained) |     |
| + int  setSubscribe(String aTopicSub[], int num) |     |
| + int  setPublish(String aTopicPub[], String aPayload[], int num) |     |
| + void subscribeString(String topic) |     |

| --- *properties* --- |     |
| -------------------- | --- |
| ~ char ssid_[SSID_SIZE+1] | SSID_SIZE = 20 |
| ~ char pass_[PASS_SIZE+1] | PASS_SIZE = 20 |
| ~ char mqtt_[MQTT_SIZE+1] | name of mqtt server (MQTT_SIZE=20) |
| ~ int  port_ | mqtt port (default 1883) |
| ~ String aTopicSub_[TOPIC_MAX] | array with all subscribed topics (TOPIC_MAX=8) |
| ~ String aTopicPub_[TOPIC_MAX] | array with all topics to publish |
| ~ String aPayloadPub_[TOPIC_MAX] | value on (re)start |
| ~ int numSub_ | number of subscribed topics |
| ~ int numPub_ | number of topics to publish |
| ~ WiFiClient d1miniClient   |     |
| ~ long millis_lastConnected |     |

More about class PubSubClient see https://github.com/knolleary/pubsubclient


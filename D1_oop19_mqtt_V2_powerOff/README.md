# D1_oop19_mqtt_V2_powerOff
When SET-button S1 on D1_PowerOff shield is pressed, the D1 mini starts and the LED lights red. The D1 mini tries to connect to a mqtt-broker (via WLAN):   
`MqttClientKH client("..ssid..", "..password..","mqttservername");`   
On success the led shines orange and D1 mini publishes the messages `/button/01` (payload 1) and `/button/01/volt` (= power supply voltage x.xxx).   
Then D1 mini waits for a message `/button/01/ok`. If the message ok=1, D1 mini sets the LED to green, waits for 2 seconds and turns off the power (D6=1).   
If there is no connection to the WLAN or no answer ok, power is turned off after 20 secs (=MILLIS_TIME2RUN) anyway (to save energy :).

__*Don't forget to change WiFi data to your network values*__ in line   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`  

### Hardware
* WeMos D1 mini
* D1_PowerOff shield   

**Important: D1 mini power supply via D1_PowerOff shield!** (NOT by USB)

![D1_PowerOff_0_shield](./images/D1_PowerOff_shield.png "D1_PowerOff_shield")   

---
# Additional infos
The class **MqttClientKH** extends the class *PubSubClient* for an easy use of mqtt. So you can use all commands from class *PubSubClient* as well.   
When library *PubSubClient* is installed in arduino ide, delete directory `/libs` and change   
`#include "libs/PubSubClient.h"` to `#include <PubSubClient.h>`.    
&nbsp;   
   
   
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
| + __MqttClientKH__(char* ssid, char* pwd, char* mqtt_server, int port) | default values: mqtt_server="localhost", port=1883  |
| + int  getNumSub()    | get number of subscribed topics |
| + int  getNumPub()    | get number of topics to publish |
| + void clrSubscribe() | set numSub_ to 0 |
| + void clrPublish()   | set numPub_ to 0 |
| + void setClientName(String sName) | set client name |
| + String getClientName() | get client name |

| --- *methods to setup WLAN and mqtt connection* --- |     |
| --------------------------------------------------- | --- |
| + bool setup_wifi()  | (try to) connect to the WiFi network, true on success. |
| + bool reconnect()   | check for connect, if not: try to reconnect |
| + bool __isConnected()__ | is mqtt connection ok? (no: reconnect after MQTT_RECONNECT_MS |
| + bool sendPubSubTopics() | MQTT: send all PubSub topics to broker, called by reconnect() |
| + void printPubSubTopics2Serial() | just for test purpose |

| --- *methods to define mqtt topics* --- |     |
| --------------------------------------- | --- |
| + bool __addSubscribe__(String topic) | add a (String) topic to subscribe list (array) |
| + bool __delSubscribe__(String topic) | convert String to array and unsubscribe |
| + bool __addPublish__(String topic, String payload, bool retain) | add a (String) topic to publish list (array) |
| + bool __delPublish__(String topic) | delet publish topic from list |
| + void __publishString__(String topic, String payload) | convert String to array and publish (without register it) |
| + void __publishString__(String topic, String payload, bool retained) | convert String to array and publish |
| + int  setSubscribe(String aTopicSub[], int num) |set array of registered subscribe topics |
| + int  setPublish(String aTopicPub[], String aPayload[], int num) | set array of subscribe topics to register |
| + void subscribeString(String topic) | convert String to array and subscribe |

| --- *properties* --- |     |
| -------------------- | --- |
| ~ char ssid_[SSID_SIZE+1] | SSID_SIZE = 20 |
| ~ char pass_[PASS_SIZE+1] | PASS_SIZE = 20 |
| ~ char mqtt_[MQTT_SIZE+1] | name of mqtt server (MQTT_SIZE=20) |
| ~ int  port_ | mqtt port (default 1883) |
| ~ String aTopicSub_[TOPIC_MAX] | array with all subscribed topics (TOPIC_MAX=8) |
| ~ String aTopicPub_[TOPIC_MAX] | array with all topics to publish |
| ~ String aPayloadPub_[TOPIC_MAX] | array with values on (re)connect network |
| ~ bool   aRetainPub_[TOPIC_MAX]  | array with retain values (true|false) |
| ~ int numSub_ | number of subscribed topics |
| ~ int numPub_ | number of topics to publish |
| ~ WiFiClient d1miniClient   | WLAN client for MQTT |
| ~ String sClientName        | MQTT client name |
| ~ long millis_lastConnected | last connection time in ms |

More about class *PubSubClient* see https://github.com/knolleary/pubsubclient

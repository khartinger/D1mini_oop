# D1 mini: Show MQTT connection data on 0.96" OLED
Sketch: D1_oop67_mqtt2_oled096_test1.ino, Version 2019-12-25   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

Connect to a WiFi and MQTT server (broker) and show connecting results on a 0.96" OLED. When connected, a counter value is sent to the broker (topic `oled096_test1`) and received values are shown on OLED.
* Show MAC address (line 7, inverted)
* Try to connect to the given WLAN (ssid, password).   
  Plot dots on OLED (line 3) while waiting
* Show name of WLAN and IP when connection is established (line 2 and line 3)
* Show state of MQTT connection (line 4)
* Show counter value published (sent) to MQTT server (topic TOPIC_S_TEST1, line 5)
* Show counter value got from MQTT server (same topic TOPIC_S_TEST1, line 6)

__*Important*__: Example needs a broker!   
Set WiFi and MQTT server data in line   
```
MqttClientKH2 client("..ssid..", "..password..","mqtt server name");   
```

## Hardware 
1. WeMos D1 mini
2. one color OLED 0.96" mounted on a proto shield (128x64 pixel, I2C-address 0x3C)       

![D1mini connecting to MQTT-Broker](./images/D1_oop67_mqtt2_connecting.png "D1mini connecting to MQTT-Broker")   
_Figure 1: D1mini connecting to MQTT-Broker_ 

![D1mini connected to MQTT-Broker](./images/D1_oop67_mqtt2_connected.png "D1mini connected to MQTT-Broker")   
_Figure 2: D1mini connected to MQTT-Broker_ 

##Software
The class MqttClientKH2 extends the class PubSubClient for an easy use of mqtt. You can use all commands from class PubSubClient as well.
When library PubSubClient is installed in arduino ide, delete files `PubSubClient.*` in directory `/src/mqtt`.

### How to make a D1mini mqtt application
1. Setup a broker  
E.g. install mosquitto on Raspberry Pi and start it   
`sudo apt-get install mosquitto`    
`sudo apt-get install mosquitto-clients`    
`sudo /etc/init.d/mosquitto start`   

2. Make a D1mini client
* Include the class MqttClientKH2  
  `#include "src/mqtt/D1_class_MqttClientKH2.h"`  
* Define a MqttClientKH2 object  
  `MqttClientKH2 client("..ssid..", "..password..","mqtt server name");`  
__*Don't forget to change WiFi data to your network values!*__
* Define a callback function to process subscribed messages (is called by every subscribed topic)  
  `void callback(char* topic, byte* payload, unsigned int length)`  
* in function setup() do mqtt setup:  
  `client.addSubscribe("topic");` subscribe topic (=input). Handle it in  callback()!  
  `client.addPublish("topic", "startvalue");` publish massage by given topic  
  `client.setCallback(callback);` set (name of) callback function  
  `client.connectWiFiMQTT();` connect to WLAN and mqtt server, wait max. 5 seconds   
  OR   
  let the connection being done by the cylic call of `client.isConnected()` in `loop()`   
* in function loop()  
  call `client.isConnected()` to process cyclic check for WLAN and mqtt (automatic reconnect, if connection is lost)  


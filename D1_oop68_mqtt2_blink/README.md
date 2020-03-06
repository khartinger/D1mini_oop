# D1 mini: Simple MQTT example
Sketch: D1_oop68_mqtt2_blink.ino, Version 2019-12-25   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   
Simple example to test a MQTT "client-server"-connection.   
* Try to connect to the given WLAN (ssid, password) and MQTT server (name or ip, default port 1883).   
If no success: blue led blinks 8x quickly, try again   

**On Success**   
* Send a message with topic `oop68/led` and alternating payload "0" or "1" every 2 seconds.   
* Subscribe the same message and turn blue led 1=on or 0=off   
* On connection lost: blue led blinks 8x quickly and D1mini tries to reconnect   

__*Important*__: Example needs a broker!   
Set WiFi and MQTT server data in line   
```
MqttClientKH2 client("..ssid..", "..password..","mqtt server name");   
```

## Hardware 
1. WeMos D1 mini

## Software
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
* in function loop()  
  call `client.isConnected()` to process cyclic check for WLAN and mqtt (automatic reconnect, if connection is lost)  

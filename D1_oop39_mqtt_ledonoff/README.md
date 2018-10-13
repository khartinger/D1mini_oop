# D1 mini: Switch blue LED by MQTT
Version 2018-10-13   
[German version](./LIESMICH.md "German version")   

This is a very simple example to demonstrate a MQTT subscriber.  
If the D1 mini receives a topic `led` with message `on` it switches the blue LED on, on message `off` it switches the led off. 

__*Don't forget to change WiFi data to your network values*__ in line   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`  

### Hardware
(1) WeMos D1 mini   

### Software
Uses `PubSubClient` and `D1_class_MqttClientKH` (see directory `src\mqtt`)   

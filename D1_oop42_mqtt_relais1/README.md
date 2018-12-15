# D1 mini: Switch a relay by MQTT
Version 2018-11-11, file: `D1_oop42_mqtt_relais1.ino`   
[German version](./LIESMICH.md "German version")   

This example shows how to switch a lamp on or off by a D1 mini with relay-shield and a mqtt message.   
Additional the D1 mini measures the alternating current by a current transformer ASM-010 and a selfmade amplifier-shield with a INA122.   

__Important__: This sketch needs a mqtt broker, for example a Raspberry Pi as access point for WLAN `Raspi10` with password `12345678`, a static ip address `10.1.1.1` and mosquitto installed. See [https://github.com/khartinger/Raspberry-as-mqtt-broker](https://github.com/khartinger/Raspberry-as-mqtt-broker "Raspberry pi as broker")   
__*You also can change WiFi data to your network values*__ in line   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`    

## MQTT messages   
To test the system you have to
* connect to the WLAN (see above)   
* open a command window   
(MS Windows cmd or a putty connection to Raspberry Pi with user `pi`, password `raspberry` or `..ras..` or something else)   
* show all relay1 messages entering `mosquitto_sub -h 10.1.1.1 -t relay1/# -v`    
* open a second command window   
* send one of the following commands e.g.   
`mosquitto_pub -h 10.1.1.1 -t relay1/get -m help`   
* In the first window you should see something like 
```
pi@raspberrypi:~ $ mosquitto_sub -h 10.1.1.1 -t relay1/# -v
relay1/get help
relay1/ret/help get: help|version|function|ip|all|lamp|current|current0|
set: lamp|current0|
```

### _set_ Commands   
* Topic `relay1/set/lamp` with payload `on`, `off` or `toggle` sets the relay on, off or changes the state.   
* Topic `relay1/set/current0` sets the "current is zero" limit (value in mA). Current values from 0 to this limit give the result "no current" (lamp=0). This is necessary because of electrical interferences.   

### _get_ Requests   
* Topic `relay1/get` with payload `help` lists all mqtt commands, that the system understands.   
* Topic `relay1/get` with payload `version` gives software date and filename.   
* Topic `relay1/get` with payload `function` describes what the application does.   
* Topic `relay1/get` with payload `ip` gives the ip address of the D1 mini.   
* Topic `relay1/get` with payload `all` sends all get messages.    
* Topic `relay1/get` with payload `lamp` gives the state of the lamp as 0 or 1 (lamp is off or on).   
* Topic `relay1/get` with payload `current` gives the current value in mA.   
* Topic `relay1/get` with payload `current0` gives the present "current is zero" limit in mA.  

## Hardware
(1) WeMos D1 mini   
(2) Relais shield   
(3) D1_INA122_V2 180924 (Selfmade) or Analog in and Current transformer ASM-010   
![D1 INA122 relay](./images/D1_ina122_relay.png "D1mini with selfmade INA122 shield and relay shield")   
_D1mini with selfmade INA122 shield, relay shield, current transformer ASM-010 and lamp_

## Software
This sketch uses the classes   
* `D1_class_MqttClientKH` and `PubSubClient`    
* `D1_class_Relais1` and `D1_class_Ain`    
* `D1_class_Statemachine`   

(see directory `src`)   


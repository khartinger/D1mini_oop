# D1 mini: Switching a lamp via a relay with MQTT
Version 2020-12-08 D1_oop77_smqtt_stm_relay1.ino    
[German version](./LIESMICH.md "German version")   

This program for a D1 mini or ESP32 D1mini is used to switch a relay or an (AC) lamp via MQTT and to measure the current. When the D1 mini receives a topic `relay/1/set/relay` with the content (payload) `on`, it switches the relay on, with `off` it switches it off and with `toggle` it toggles it.   
The two Leds are used to indicate the system status:   
* DUO-LED red: Not connected to WLAN
* DUO-LED orange: Connected to WLAN but not to the MQTT broker
* DUO-LED green: MQTT connection to broker OK.
* The second (green) led flashes briefly every 2.5 seconds to indicate that the D1 mini is working.   

__*Don't forget to change WiFi data to your network values*__ in line   
`SimpleMqtt client("..ssid..", "..password..","mqtt server name");`  

![D1mini with INA122- and relay shield](./images/D1_smqtt_INA122_relay1.png "D1mini with INA122- and relay shield")    
_Figure 1: D1mini with INA122- and relay shield_   

### Hardware
1. WeMos D1 mini   
2. relay shield   
3. Selfmade shield D1_INA122_V3 with AC current transformer (zB ASM-010) at analog-in   
4. (green) LED at D8, Duo-LED at D6 (green)/D7 (red)   

### Details about MQTT-messages   
* All topics for D1 mini start with `relay/1/`   
* All MQTT-__requests__ use the same topic `relay/1/get`, the type of information is given by the payload.   
*Possible payloads are defined in array sGet[].*   
* Answers are sent by using the topic `relay/1/ret/[sGet]`. [sGet] names the type of information. The payload contents the result.   
* Commands for the D1 mini use the topic `relay/1/set/[sSet]`. [sGet] names the type of command. The payload contents the value to be set.   
*Possible payloads are defined in array sSet[].*   

__*Examples*__  
* Topic `relay/1/get` with content (payload) `help` results in a topic `relay/1/ret/help` with payload `get: help|version|ip|topicbase|eeprom|all|lamp|relay|current|current0|debug   
set: topicbase|eeprom|relay|current0|debug`
* Topic `relay/1/set/relay` with payload `on` results in an answer topic `relay/1/ret/relay` with  payload `1` and the relay is switched on.

### Test with mosquitto publisher and subscriber
1. Connect to the Wifi (WLAN), that the MQTT server uses.
2. Open a console in Windows or Linux and show (subscribe) all messages for the D1 mini:   
`mosquitto_sub -h 10.1.1.1 -t "relay/1/#" -v`  
3. Open a second console in Windows or Linux.   
Switch on the relay with    
`mosquitto_pub -h 10.1.1.1 -t "relay/1/set/relay" -m on`  
Switch off the relay with    
`mosquitto_pub -h 10.1.1.1 -t "relay/1/set/relay" -m off`  
Toggle the relay with   
`mosquitto_pub -h 10.1.1.1 -t "relay/1/set/relay" -m toggle`  

### Software
This example uses the following files (classes):   
* `PubSubClient` and `D1_class_SimpleMqtt` (see directory `src\simplemqtt`)   
* `D1_class_Relay2` , `D1_class_Relay2AC` und `D1_class_CurrentAC` (see directory `src\relay2AC`)   
* `D1_class_Statemachine` (see directory `src\statemachine`)   

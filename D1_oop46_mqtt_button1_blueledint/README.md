# Di mini: Button sends MQTT command, feedback by blue led
Version 2018-11-01, file: `D1_oop46_mqtt_button1_blueledint.ino`   
[German version](./LIESMICH.md "German version")   

Press button D3 sends a mqtt-request via a broker to a D1 mini with  relay-shield.  
On start the blue LED lights and D1 mini tries to connect to a mqtt-broker. On success led is turned off, otherwise it blinks quickly (on:off = 0.1s:0.1s).   
When button D3 is pressed, D1 mini   
* publishes topic `relay1/set/lamp` with payload `toggle`   
* publishes info about the button under topic `button1/ret/function`   
* blinks the blue led slowly (on:off = 0.5s:0.5s)   

Then D1 mini waits for topic `relay1/ret/lamp`, payload 0 or 1.   
* If there is no answer, the blue led goes on blinking slowly (on:off = 0.5s:0.5s).   
* If the answer is 0, blue led blinks short pulses (on:off = 0.1s:0.9s).   
* If the answer is 1, blue led blinks long pulses (on:off = 0.9s:0.1s).   

After 15 seconds the blue led is turned off (to save energy ;).   

__Important 1__: This sketch needs a mqtt broker, for example a Raspberry Pi as access point for WLAN `Raspi10` with password `12345678`, a static ip address `10.1.1.1` and mosquitto broker installed. See [https://github.com/khartinger/Raspberry-as-mqtt-broker](https://github.com/khartinger/Raspberry-as-mqtt-broker "Raspberry pi as broker")   
__*You also can change WiFi data to your network values*__ in line   
`MqttClientKH client("..ssid..", "..password..","mqtt server name");`    

__Important 2__: This sketch needs a receiver for accepting the message and  sending back an answer. Receiver can be a D1 mini with relais-shield. See e.g. [https://github.com/khartinger/D1mini_oop/tree/master/D1_oop42_mqtt_relais1](https://github.com/khartinger/D1mini_oop/tree/master/D1_oop42_mqtt_relais1 "D1 mini: Switch a relay by MQTT")

## MQTT messages   
To test the button without a relay shield you have to
* connect to the WLAN (see above)   
* open a command window   
(MS Windows cmd or a putty connection to Raspberry Pi with user `pi`, password `raspberry` or `..ras..` or something else)   
* show all relay1 messages entering `mosquitto_sub -h 10.1.1.1 -t relay1/# -v`    
* open a second command window   
* press button D3 (-> blue led blinks slowly)
* window 2: Send the following command to the button   
`mosquitto_pub -h 10.1.1.1 -t relay1/ret/lamp -m 0`   
* The blue led now blinks in short pulses (this means, that 0 was received = lamp is off).
* In the first window you should see something like 
```
pi@raspberrypi:~ $ mosquitto_sub -h 10.1.1.1 -t relay1/# -v
relay1/set/lamp toggle
relay1/ret/lamp 0
```

### _get_ Requests   
* Topic `button1/get` with payload `help` lists all mqtt commands, that the system understands.   
* Topic `button1/get` with payload `version` gives software date and filename.   
* Topic `button1/get` with payload `function` describes that the button sends a message.   
* Topic `button1/get` with payload `ip` gives the ip address of the D1 mini.   
* Topic `button1/get` with payload `all` sends all get messages.    

## Hardware
(1) WeMos D1 mini   
(2) 1-Button shield (D3)   
![D1 button1](./images/D1_buttonD3neu.png "D1mini with 1-Button-Shield")   
_D1mini with 1-Button-Shield_


## Software
This sketch uses the classes   
* `D1_class_MqttClientKH` and `PubSubClient`    
* `D1_class_BlueLed`   
* `D1_class_Din`   
* `D1_class_Statemachine`   

(see directory `src`)   

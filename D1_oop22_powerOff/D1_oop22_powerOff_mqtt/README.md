# D1_oop22_PowerOff_mqtt
When SET-button S1 is pressed, the D1 mini starts and the LED lights red. The D1 mini tries to connect to a mqtt-broker (via WLAN):   
`MqttClientKH client("..ssid..", "..password..","mqttservername");`   
On success the led shines orange and D1 mini publishes the messages `/button/01` (payload 1) and `/button/01/volt` (= power supply voltage x.xxx).   
Then D1 mini waits for a message `/button/01/ok`. If the message ok=1, D1 mini sets the LED to green, waits for 2 seconds and turns off the power (D6=1).   
If there is no connection to the WLAN or no answer ok, power is turned off after 15 secs anyway (to save energy :).

### Hardware
* WeMos D1 mini
* D1_PowerOff shield   

**Important: D1 mini power supply via D1_PowerOff shield!** (NOT by USB)
&nbsp;

# Additional infos
The class **MqttClientKH** extends the class **PubSubClient** for an easy use of mqtt. You can use all commands from class PubSubClient as well.

When library PubSubClient is installed in arduino ide, delete directory `/libs` and change   
`#include "libs/PubSubClient.h"` to `#include <PubSubClient.h>`.   
&nbsp;   
   
### How to make the D1 mini mqtt application work
1. Setup a broker  
E.g. install mosquitto on Raspberry Pi and start it  
`sudo apt-get install mosquitto`  
`sudo apt-get install mosquitto-clients`  
`sudo /etc/init.d/mosquitto start`  

2. Prepare broker answer for D1 mini with D1_oop22_PowerOff_mqtt
Raspberry Pi: Enter this at command line   
`mosquitto_pub -t button/01/ok -m 1 -r`   
Switch -r ensures, that the message is sent to the D1 mini, whenever it asks for the last valid message. Without -r you have to answer after every request.

3. Make a D1 client
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
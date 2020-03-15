# D1 mini: Switch the blue LED of another D1 mini by MQTT
Version 2020-03-08, Sketch: D1_oop72_mqtt2_button2blueled.ino    
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

The button of this D1 mini controls the blue LED of another D1 mini, on that e. g. the program `D1_oop71_mqtt2_blueled` is running.   
The blue LED of a D1 mini can be controlled by MQTT:   
* If the button is pressed, the D1 mini sends a message with topic `oop71/set/blueled` and payload `2`.   
* Receiving a message with topic `oop72/set/blueled` and payload 0, 1 or 2 turns the blue LED off, on or toggles it.   
* If there is no connection to the MQTT server, the blue LED blinks quickly.   

To demonstate that the program is still running, the blue LED is not only turned off or on, but blinks with cyle time 2s:
* "off" = 1.9s LED off, 0.1s LED on (= short pulses)
* "on"  = 0.1s LED off, 1.9s LED on (= short breaks)

### Structure of MQTT messages
* Request `oop71/get` _help_         : asks for all sGet-, sSet-, sSub- and sPub-topics
* Request `oop71/get` _version_      : asks for version of program (e.g. 2020-03-08 D1_oop71_mqtt2_blueled)
* Request `oop71/get` _ip_           : asks for ip of the D1mini
* Request `oop71/get` _blueled_      : asks for state of blue LED
* Command `oop71/set/blueled` _0|1|2_: sets blue LED off|on|toggle
* Answer  `oop71/ret/blueled` _0|1_  : sends answer get-request

For ease of use, the loop uses a state machine (class `Statemachine`) and the class `StatemachineBlink` to control the LED.   
State machine: cycle time 1 minute (600 states each 0.1s)   

__*Important*__: Example needs a broker!   
Set WiFi and MQTT server data in line   
`MqttClientKH2 client("..ssid..", "..password..","mqtt server name");`   


## Hardware 
1. WeMos D1 mini

## Software
The program uses four classes:   
* `Statemachine`: step (state) counter for timing
* `StatemachineBlink`: controls LED blinking (needs class `Statemachine`)
* `MqttClientKH2`: Class extends class `PubSubClient` for an easy use of mqtt. You can use all commands from class PubSubClient as well.
* `Din`: This class helps to notice a change of the button state (falling edge).

__*Note*__: When library PubSubClient is installed in arduino ide, delete files `PubSubClient.*` in directory `/src/mqtt`.

### Ideas on this program
* __*Name of topics and payloads*__: To use the program as a template for other programs, all MQTT names are stored in arrays or defined as constants. Example:   
`sGet[]` contains all payloads for requests with topics `.../get`, e.g. sGet[GETMAX72]={"help","version","ip","blueled"}   
`sSet[]` contains the last part of topics `.../set/...` to set values, e.g. sSet[SETMAX72]={"blueled"}   
`sSub[]` contains "different" topics, the D1 mini listens to, e. g.   
`sPub[]` contains "different" topics, the D1 mini sends to, e. g.   
`sSub[SUBMAX72]={"oop71/ret/blueled"};`   
`sPub[]` contains "different" topics, the D1 mini sends to, e. g.   
`sPub[PUBMAX72]={"oop71/set/blueled"};`   

* `callback()`: This function is called by every topic that is registered (subscribed) by the MQTT server. To keep the runtime of this function short, only __bits are set__ in variables ("container") `iGet`, `iSet` or `iSub`. Payloads for iSet (or iSub) are stored in `sSetPayload[]` (or `sSubPayload[]`).   
* `sendRet()`: This function sends all MQTT messages, whose bits are set in `iRet` or `iPub`, back.   
* `doGet()`: This function handles all get-actions (bits with value 1 in iGet) and must be called periodically by the main loop() function. For every MQTT-request it prepairs an answer, stores it as payload in `sRetPayload[]` and sets the bit in "container" `iRet`.   
* `doSet()`: This function is similar to `doGet()`, but it also handles changes on parameters of the program or hardware using the payload stored in `sSetPayload[]`.   
For every MQTT-request it does the required action. After that, it usually prepares an answer, stores it as payload in `sRetPayload[]` and sets the bit in "container" `iRet`.   
In this case it turns off or on the blue LED.   

The loop() function consists of seven parts:
```
void loop() {
 //======SECTION 1: action at the beginning of loop=============
 int state=stm.loopBegin();            // state begin 
 // ...
 //======SECTION 2: check for WiFi and MQTT connection==========
 if(client.isWiFiConnected())
 {
  if(client.isConnected()) //MUST always be called (for receive!)
  {//====SECTION 3: action for WiFi=YES, MQTT=YES===============
   // ...
  }//====END OF SECTION 3=======================================
  else
  {//====SECTION 4: action for WiFi=YES, MQTT=NO================
   // ...
  }//====END OF SECTION 4======================================= 
 }//=====END OF SECTION 2: End of if=true=======================
 else
 {//=====SECTION 5: action for WiFi=NO (-> MQTT=NO)=============
  // ...
  //=====END OF SECTION 5=======================================
 }//=====END OF SECTION 2: End of else==========================
 //======SECTION 6: action for WiFi=DONT_CARE, MQTT=DONT_CARE===
 if(iGet>0) doGet();
 if(iSet>0) doSet(); 
 // ...
 //======SECTION 7: prepare for next state and wait=============
 led1.doBlink(stm);                    // blue led action
 stm.loopEnd();                        // state end
}
```

__*Summary*__   
The complete procedure of setting the blue led and generating an answer is as follow:   
1. Pressing the button sets Bit 0 in "container" `iPub` and stores the value `2`  (= toggle) in `sPubPayload[0]`.   
2. If the MQTT-server is ready, function `loop()` calls `sendRet()` in section 3 (iPub is greater than 0).
3. Function `sendRet()` sends (publishes) a message with topic sPub[0]=`oop71/set/blueled` and payload `2` (= value of sPubPayload[0]) to toggle the blue LED of the other D1 mini.   
------    
_If an answer comes in:_   

4. Function `callback()` finds in section 4 the "different" message with topic sSub[0]=`oop71/ret/blueled`, sets bit 0 in `iSet` und stores the payload in `sSetPayload[0]`.   
5. In `loop()` section 6 the value of `iSet` is greater than 0, so `doSet()` is called.   
6. In `doSet()` the LED is toggled (bit i=0) and an answer is triggered:   
 `iRet|=(1<<3);` sets bit 3 in iRet to send the same message as MQTT-message sGet[3]="blueled" with payload `sRetPayload[3]=String(iBlueLed);`.   
7. In `loop()` section 3 the value of iRet is greater than 0, so `sendRet()` is called.   
8. In `sendRet()` a message with topic TOPIC_71_RET + sGet[3] = `oop71/ret/blueled` and payload "0" or "1" (= value of sRetPayload[3]) is published.   

Listening to the messages with mosquitto results e. g. in   
```   
mosquitto_sub -h 10.1.1.1 -t "#" -v
oop71/set/blueled 2
oop71/ret/blueled 1
oop72/ret/blueled 1
```   
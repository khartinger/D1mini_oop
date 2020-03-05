# D1 mini: State machine with blinking LED
Version 2020-02-25, File: D1_oop38_statemachineBlink.ino   
[German version](./LIESMICH.md "Deutsche Version")   

This program lets a led blink using the classes `Statemachine` and `StatemachineBlink`.   
You need to know...   
* the pin number, where the LED is wired (e.g. D4)
* LED is on, if pin voltage is 3V3 or 0V (= inverted)
* the state number where the LED starts blinking the first time
* the duration for LED is on (in states)
* the duration for LED is off (in states)
* the number of blinks (-1 = endless)

### Hardware
1. WeMos D1 mini

### Software
The program uses the following classes   
* `Statemachine`: timing (600 states with 0.1 sec each)   
* `StatemachineBlink`: switch LED on or off   

### Constructor
`StatemachineBlink led1(LED1_PIN, LED1_INVERT, STATE_LED1_START, STATES_LED1_ON, STATES_LED1_OFF, LED1_BLINK_NUM);`   
__*Examples:*__   
_Assumption_: The duration of one state of the statemachine is 0.1 sec   
* `StatemachineBlink led1(D4,true,1,5,5,3);` ... Blue LED blinks on every statemachine cycle start 3x at one-second intervals
* `StatemachineBlink led1(D4,true,20,1,9,5);` ... Blue LED flashes two seconds after statemachine cycle start 5 times
* `StatemachineBlink led7(D7,false,1,5,15,-1);` and   
`StatemachineBlink led8(D8,false,6,15,5,-1);` ... LEDs at D7 and D8 build an endless flashing light. LED D8 lights 3 times longer than D7.

### (Main-)loop
The main loop() function may look like this:
```
void loop() {
 statemachine.loopBegin();                  // state begin
 led1.doBlink(statemachine);                // blink LED
 statemachine.loopEnd();                    // state end
}
```


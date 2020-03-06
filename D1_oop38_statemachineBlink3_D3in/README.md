# D1 mini: State machine with blinking LED
Version 2020-03-06, Sketch: D1_oop38_statemachineBlink3_D3in.ino   
[German version](./LIESMICH.md "Deutsche Version")   

This program demonstrates the control of a LED using the classes `Statemachine` and `StatemachineBlink`. The mode is selected by a button:   
* Press button D3 short turns the blue led on   
* Press button D3 long turns the blue led off   
* Press button D3 twice lets the blue led blink quickly   

### Hardware
1. WeMos D1 mini
2. Button shield (D3)

### Software
The program uses the following classes   
* `Statemachine`: timing (10 states with 0.1 sec each)   
* `StatemachineBlink`: switch LED on or off, LED blinking   
* `Din`: digital in. Button pressed, clicked, double clicked

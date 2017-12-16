# D1 mini - Demo: Digital in and time measuring (class Din, version 2)
Sketch: D1_oop01_Din2.ino, Version 2017-12-13   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

Demonstrate time measuring methods of class Din:   
* get duration since last change (= edge) and print this time in seconds (10 per line)
* read button status (0|1) and show it on blue led (button pressed -> led is on)
* check for falling and rising edge, print a message if true
* while button is pressed, print P on Serial   

Cycle time 0,2 sec, results are printed on Serial (115200 Baud).

## Hardware
* WeMos D1 mini
* Button shield (D3) or wire from D3 to GND

![Image: D1mini with button shield](./images/D1_buttonD3neu.png "D1mini with button shield")

&nbsp;

**Example for Serial output:**
```
Press button D3!
0.01s 0.21s 0.41s 0.61s 0.81s 1.01s 1.21s 1.41s 1.61s 1.81s 
2.01s 2.21s 2.41s 2.61s 2.81s 3.01s 3.21s 
Last Duration: 3214 ms ---> Button pressed!
P 0.20s P 0.40s P 0.60s P 0.80s P 1.00s P 1.20s P 1.40s P 1.60s P 1.80s P 2.00s 
P 2.20s P 2.40s P 2.60s P 2.80s P 3.00s P 3.20s 
Last Duration: 3203 ms ---> Button released!
0.20s 0.40s 0.60s 0.80s 1.00s 1.20s 1.40s 1.60s 1.80s 
Last Duration: 1801 ms ---> Button pressed!
P 0.20s P 0.40s 
Last Duration: 401 ms ---> Button released!
0.20s 0.40s 0.60s 0.80s 1.00s 1.20s 1.40s 
Last Duration: 1401 ms ---> Button pressed!
P 0.20s P 0.40s 
Last Duration: 401 ms ---> Button released!
0.20s 0.40s 0.60s 0.80s 1.00s 1.20s 1.40s 1.60s 1.80s 2.00s 
2.20s 2.40s 2.60s 2.80s 3.00s 3.20s 3.40s 3.60s 3.80s 4.00s
```

## Class diagram Din
| class Din                     |                                               |
| ----------------------------- | --------------------------------------------- |
| + __Din__()                   | default constructor, set pin to D3, calls setup() |
| + __Din__(int num)            | set input pin, call setup()                   |
| ~ void setup(int num)         | setup properties: input mode, lastMlli_, ...  |
| ===== *setter, getter methods (no hardware access)* ===== |                   |
| + void setClickMilli(unsigned long ms_max) | maximal duration of a part of a click |
| + int  getLast(void)          | get last input value without pin read         |
| + unsigned long getDuration() | get duration of actual state (0 or 1) in msec (since last pin edge) |
| + unsigned long getLastDuration() | get last duration of pin change in msec without pin read |
| ===== *working methods* ===== |                   |
| + int  __read__(void)         | read input, check for edge, process click parts, return val    |
| + bool __isPressed__(void)    | call read(), return true (= button pressed = 0V) or false (3V3) |
| + bool __isPressed__(unsigned long ms_min) | call read(), return true, if input is pressed (=0V) for more than given ms |
| + bool __isFallingEdge__(void)| call read(), return true if button is pressed  |
| + bool __isRisingEdge__(void) | call read(), return true if button is released |
| + bool __isClicked__()        | call read(), return true if button is 1x clicked (duration less than clickMilli_) |
| + bool __isDoubleClicked__()  | call read(), return true if button is 2x clicked (duration less than clickMilli_) |

&nbsp;

|  *properties*                 |                                            |
| ----------------------------- | ------------------------------------------ |
| ~ int dinNum_                 | pin number (default D3 = GPIO0)            |
| ~ int din_                    | digital input value of last read operation |
| ~ int lastEdge_               | 0=no edge (after edge value is read), 1=rising edge, 2=falling edge |
| ~ int clickMilli_             | max. click duration |
| ~ int click1                  | state of 1x click (0=no, 1=falling edge, 2=rising edge, 3=end ok |
| ~ int click2                  | state of double click (0=no, 1=falling edge, 2=rising edge, 3=falling edge, 4=rising edge, 5=end ok |
| ~ int clickL                  | state of long click (0=no, 1= falling edge) |
| ~ unsigned long lastMilli_    | absolute time in ms since last edge (change of pin value). Value of milli() |
| ~ unsigned long lastDuration_ | time (duration) in ms of this state (since last change if pin value) | 
  
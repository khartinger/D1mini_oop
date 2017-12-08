# D1 mini: Demo - program for class Din (digital input, version 2)
Sketch: D1_oop01_Din1.ino, Version 2017-12-07   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

Demonstrate methods of class Din:   
* get duration since last change (= edge) and print this time in seconds
* read button status (0|1) and show it on blue led (button pressed -> led is on)
* check for falling and rising edge, print a message if true
* if button is pressed, print P on Serial   

Cycle time 0,2s, results are printed on Serial (115200 Baud).

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
| + Din()                       | default constructor, set pin to D3, calls setup() |
| + Din(int num)                | set input pin, call setup()                   |
| ~ void setup(int num)         | setup properties: input mode, lastMlli_, ...  |
| ===== *setter, getter, working methods* ===== |                               |
| + int  get(void)              | read input, check for edge, return val        |
| + bool isPressed(void)        | call get(), return true (= button pressed = 0V) or false (3V3) |
| + bool isFallingEdge(void)    | call get(), return true if button is pressed  |
| + bool isRisingEdge(void)     | call get(), return true if button is released |
| + int  getLast(void)          | get last input value without pin read         |
| + unsigned long getDuration() | get duration of actual state (0 or 1) in msec (since last pin edge) |
| + unsigned long getLastDuration() | get last duration of pin change in msec without pin read |

&nbsp;

|  *properties*                 |                                            |
| ----------------------------- | ------------------------------------------ |
| ~ int dinNum_                 | pin number (default D3 = GPIO0)            |
| ~ int din_                    | digital input value of last read operation |
| ~ int lastEdge_               | 0=no edge (after edge value is read), 1=rising edge, 2=falling edge |
| ~ unsigned long lastMilli_    | absolute time in ms since last edge (change of pin value). Value of milli() |
| ~ unsigned long lastDuration_ | time (duration) in ms of this state (since last change if pin value) | 
  
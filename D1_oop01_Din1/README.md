# D1 mini - Demo: click methods of class Din (digital input, version 2)
Sketch: D1_oop01_Din1.ino, Version 2017-12-13   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

Demonstrate button clicked methods of class Din:   
* if falling edge: print message to Serial
* if rising  edge: print message to Serial
* if button pressed longer than 1 sec: print message to Serial
* if button is clicked short once: print message to Serial
* if button is clicked short twice: print message to Serial
* while button is pressed, print P on Serial

Cycle time 0,1 sec, print results to Serial (115200 Baud) and show them via blue LED.

## Hardware
* WeMos D1 mini
* Button shield (D3) or wire from D3 to GND

![Image: D1mini with button shield](./images/D1_buttonD3neu.png "D1mini with button shield")

&nbsp;

**Example for Serial output:**
```
Press button D3!
v  Falling Edge
^  Rising Edge
Click!
v  Falling Edge
LongClick!
^  Rising Edge
v  Falling Edge
^  Rising Edge
v  Falling Edge
^  Rising Edge
DoubleClick!
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

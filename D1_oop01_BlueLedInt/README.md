# D1 mini: Demo for class BlueLedInt
Sketch: D1_oop01_BlueLedInt.ino, Version 2018-11-01   
[Deutsche Version](./LIESMICH.md "Deutsche Version")   

Demonstrate methods of class `BlueLedInt`:   
1. turn blue led on 2 sec, off 1 sec
2. turn on blue led slowly 1x
3. blinking while delay is done
4. stop blinking after 2.5x (led remains on)
5. wait a second and start blinking again
6. loop: automatic blink (0,1s on 0,9s off)

## Hardware
* WeMos D1 mini

![Image: D1mini](./images/D1mini_171203.png "D1mini")


## Software

Pin number, pin value and blinking time for led on/off are stored in a structure blueLedTimer
```
struct {
 int pinNum=BLUELEDINT_PIN;
 int pinVal=BLUELEDINT_OFF;
 int msOn  =BLUELEDINT_BLINK;
 int msOff =BLUELEDINT_BLINK;
} blueLedTimer;
```
<br>

| class BlueLedInt |     |
| ---------------------------------  | -------- |
| + BlueLedInt() | constructor (default digital out is D4, not inverted) |
| + + BlueLedInt(int pin) | constructor with output pin number (=GPIO, not inverted) |
| ~ void setup(int pin) | setup output pin and properties |
| ===== *setter, getter, working methods* ===== |     |
| + void set(int val)           | set output pin physical:  0=0V, 1=3V3 |
| + void on(void)               | set output pin logical on (output value 0|1 depending on invert_)  |
| + void off(void)              | set output pin logical off (output value 0|1 depending on invert_) |
| + void toggle()               | change output on <--> off  |
| + void blink(int on_ms,int off_ms) | blink duration         |
| + void blinkOn()                   | start blinking (again) |
| + void blinkOff()                  | stopp blinking         |
| + void brightness(int persentOn)   | brightness 0..100 [%]  |
| + void brightnessOn()              | brightness on          |
| + void brightnessOff()             | brightness off         |
  
&nbsp;

|  *properties*               |                                 |
| --------------------------- | ------------------------------- |
| ~ int  brightness_          | Brigthness 0..100 [%]           |
| ~ usePwm__                  | If true: Use PWM for brightness |
  
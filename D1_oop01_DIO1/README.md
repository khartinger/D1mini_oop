# D1_oop01_DIO1 
This example shows how to use the classes Din and Led.

The status of a button at D3 (=DGPIO0) is shown on blue LED (D4=GPIO2).
+ If the button is pressed, the input is zero (0) and this causes the led to light up.
+ If the button is pressed or released, a message is sent to Serial (9600Bd).

## Hardware
* WeMos D1 mini
* 1-Button Shield

![D1mini with 1-BUTTON Shield](./images/D1_1ButtonShield.png "D1mini with 1-BUTTON Shield")

## Class diagrams   

| class Din    |   
| ---------    |   
| - void setup(int num) <br> - int  din_num <br> - int  din_old <br> - int  last_edge  |   
| + Din() <br> + Din(int num) <br> + int  get(void) <br> + bool isPressed(void) <br> + bool is_falling_edge(void) <br> + bool is_rising_edge(void) |   
   
| class Led |
| --------- |
| - void setup(int num) <br> - int  led_num |
| + Led() <br> + Led(int num) <br> + void set(int on_0) <br> + void on(void) <br> +  void off(void) |

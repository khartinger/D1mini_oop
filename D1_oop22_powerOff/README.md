# Di mini: D1_PowerOff shield

The D1_PowerOff shield is used to power a D1 mini by a battery via a relay.
When the SET-button on the D1_PowerOff shield is pressed, a bipolar relay is set and the power remains switched on even when the button is released.
[Software: pin D6 must be switched off = 0V!].

### Overview of the attributes of the D1_PowerOff shield

* 2x LEDs (pin D7 = green, D8 = red)
* 1x output pin D6 to reset relay
* 1x button to set relay
* 1x button to reset relay
* 1x measuring battery voltage (A0):   
  |-----5V  (0...5,4V)   
 220k   
  +-----A0  (0...3.2V)   
 220k   
  +-----ADC (0...1V)   
 100k   
  +-----GND   

**Important: D1 mini supply voltage via D1_PowerOff shield!**


![D1_PowerOff_0_Circuit](./images/D1_PowerOff_0_CircuitA.jpg "D1_PowerOff_0_Circuit")

K1 ... connector for supply voltage (e.g. battery)

![D1_PowerOff_0_CompA](./images/D1_PowerOff_0_CompA.jpg "D1_PowerOff_0_CompA")
![D1_PowerOff_0_SolderA](./images/D1_PowerOff_0_SolderA.jpg "D1_PowerOff_0_SolderA")

The difference between *_0 and *_90 shield is just the type of SET-button (0...button direction is top, 90...button in front direction)

# Software examples

**D1_oop22_PowerOff_6sec**

When the SET-button is pressed the LED lights for 2 seconds green, yellow and red. Then the supply voltage is switched off.   
Possible error: If the power voltage is connected to USB, the program starts from the beginning...

                                                 